#include <node.h>

#include <mutex>

#include <iostream>

#include <map>

#include <fstream>

#include <tensorflow-lite/interpreter.h>
#include <tensorflow/lite/model_builder.h>

#include <tensorflow-lite/interpreter_builder.h>
#include <tensorflow/lite/kernels/register.h>
#include <tensorflow/lite/optional_debug_tools.h>

#ifdef TFL_CAPI_EXPORT
#undef TFL_CAPI_EXPORT
#define TFL_CAPI_EXPORT
#endif // TFL_CAPI_EXPORT
#include <tensorflow/lite/delegates/gpu/delegate.h>
#include <tensorflow/lite/c/c_api.h>

//#include <tensorflow/lite/delegates/gpu/api.h>
//#include <tensorflow/lite/delegates/gpu/gl_delegate.h>
//#include "tensorflow/lite/delegates/gpu/metal_delegate_internal.h"
//#include <tensorflow/lite/delegates/gpu/cl/buffer.h>

#include <opencv2/opencv.hpp>

#pragma once
extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/log.h>
#include <libswscale/swscale.h>
}

#include <windows.h>
#pragma comment(lib, "winmm.lib")

#define SLEEP_TIME 1
#define PREDICTED_FRAME_LIMIT 3

static int video_stream_index = -1;
static int open_input_file(const char* filename, AVCodecContext** _dec_ctx, AVFormatContext** _fmt_ctx, const AVCodec** codec)
{
	auto& fmt_ctx = *_fmt_ctx;
	auto& dec_ctx = *_dec_ctx;

	const AVCodec*& dec = *codec;
	int ret;

	if ((ret = avformat_open_input(&fmt_ctx, filename, NULL, NULL)) < 0)
	{
		av_log(NULL, AV_LOG_ERROR, "Cannot open input file\n");
		return ret;
	}

	if ((ret = avformat_find_stream_info(fmt_ctx, NULL)) < 0)
	{
		av_log(NULL, AV_LOG_ERROR, "Cannot find stream information\n");
		return ret;
	}

	/* select the video stream */
	ret = av_find_best_stream(fmt_ctx, AVMEDIA_TYPE_VIDEO, -1, -1, &dec, 0);
	if (ret < 0)
	{
		av_log(NULL, AV_LOG_ERROR, "Cannot find a video stream in the input file\n");
		return ret;
	}
	video_stream_index = ret;

	/* create decoding context */
	dec_ctx = avcodec_alloc_context3(dec);
	if (!dec_ctx)
		return AVERROR(ENOMEM);
	avcodec_parameters_to_context(dec_ctx, fmt_ctx->streams[video_stream_index]->codecpar);

	/* init the video decoder */
	if ((ret = avcodec_open2(dec_ctx, dec, NULL)) < 0)
	{
		av_log(NULL, AV_LOG_ERROR, "Cannot open video decoder\n");
		return ret;
	}

	return 0;
}

struct PosePair
{
	int start;
	int end;
};

struct ColoredPosePair
{
	int start;
	int end;
	std::string color;
};

std::map<std::string, cv::Vec3b> COLOR = {
	{ "r", {255, 0, 0} },
	{ "g",{0, 255, 0} },
	{ "b", {0, 0, 255}}
};

ColoredPosePair KEYPOINT_EDGE_INDS_TO_COLOR[] = {
	{0, 1, "r"},
	{0, 2, "g"},
	{1, 3, "r"},
	{2, 4, "g"},
	{0, 5, "r"},
	{0, 6, "g"},
	{5, 7, "r"},
	{7, 9, "r"},
	{6, 8, "g"},
	{8, 10, "g"},
	{5, 6, "b"},
	{5, 11, "r"},
	{6, 12, "g"},
	{11, 12, "b"},
	{11, 13, "r"},
	{13, 15, "r"},
	{12, 14, "g"},
	{14, 16, "g"}
};

std::map<std::string, int> KEYPOINT_DICT = {
	{"nose", 0},
	{"left_eye", 1},
	{"right_eye", 2},
	{"left_ear", 3},
	{"right_ear", 4},
	{"left_shoulder", 5},
	{"right_shoulder", 6},
	{"left_elbow", 7},
	{"right_elbow", 8},
	{"left_wrist", 9},
	{"right_wrist", 10},
	{"left_hip", 11},
	{"right_hip", 12},
	{"left_knee", 13},
	{"right_knee", 14},
	{"left_ankle", 15},
	{"right_ankle", 16}
};

template<typename T>
std::vector<T*> Reshape(T* arr, int row, int col)
{
	std::vector<T*> ret;
	ret.resize(row);

	for (size_t i = 0; i < row; i++)
	{
		ret[i] = &arr[i * col];
	}

	return ret;
}

void DrawPose(cv::Mat& img, float* _pose)
{
	auto h = img.rows;
	auto w = img.cols;

	auto pose = Reshape(_pose, 17, 3);

	for (auto& p : KEYPOINT_EDGE_INDS_TO_COLOR)
	{
		auto& start = pose[p.start];
		auto& end = pose[p.end];

		auto y1 = (int)(start[0] * h);
		auto x1 = (int)(start[1] * w);

		auto y2 = (int)(end[0] * h);
		auto x2 = (int)(end[1] * w);

		cv::line(img, { x1,y1 }, { x2,y2 }, COLOR[p.color]);
	}
}

v8::Global<v8::Object> g_clients;

//return 1 on lock occupied
//return 0 in ow
inline int WaitToLock(std::mutex& locker, unsigned long perCheckTime = 16, long long timeout = LLONG_MAX)
{
	long long count = 0;
	while (!locker.try_lock())
	{
		count += perCheckTime;
		Sleep(perCheckTime);
		if (count > timeout)
		{
			return 0;
		}
	}

	return 1;
}

inline void Release(std::mutex& locker)
{
	locker.unlock();
}

class ProcessingCore
{
public:
	//everything must syn or multiple core cause tensorflow keras back-end not allow multiple-thread accessing
	//static std::mutex mutex;

	//static std::vector<v8::Global<v8::ArrayBuffer>> buffers;

	//not own
	v8::Isolate* m_isolate = 0;
	v8::Global<v8::Function> m_callback;
	v8::Global<v8::Object> m_this;

public:
	//own
	std::vector<uint8_t> m_buffer;
	std::vector<uint32_t> m_bufferMap;
	std::vector<float> m_poses;

public:
	inline void ProcessBuffer(v8::Isolate* isolate, v8::Local<v8::ArrayBuffer> buffer)
	{
		/*if (WaitToLock(mutex))
		{
			if (WaitToLock(thread->m_bufferMutex))
			{

				Release(thread->m_bufferMutex);
			}
			buffers.push_back(v8::Global<v8::ArrayBuffer>(isolate, buffer));
			Release(mutex);
		}*/
	}

	inline void GetCallback(v8::Isolate* isolate, v8::Local<v8::Object> _this)
	{
		auto context = isolate->GetCurrentContext();
		auto callback = _this->Get(context, v8::String::NewFromUtf8(isolate, "callback").ToLocalChecked());

		if (callback.IsEmpty()) return;

		auto _callback = callback.ToLocalChecked();

		if (!_callback->IsFunction()) return;

		auto func = v8::Local<v8::Function>::Cast(_callback);

		m_callback = v8::Global<v8::Function>(isolate, func);
		m_this = v8::Global<v8::Object>(isolate, _this);
	}

	inline void CallCallback()
	{
		if (m_callback.IsEmpty()) goto end;

		if (m_buffer.size() == 0) goto end;

		auto callback = m_callback.Get(m_isolate);
		auto _this = m_this.Get(m_isolate);

		v8::Local<v8::ArrayBuffer> arg[] = {
			v8::ArrayBuffer::New(m_isolate, m_buffer.data(), m_buffer.size() * sizeof(uint8_t)),
			v8::ArrayBuffer::New(m_isolate, m_bufferMap.data(), m_bufferMap.size() * sizeof(uint32_t)),
			v8::ArrayBuffer::New(m_isolate, m_poses.data(), m_poses.size() * sizeof(float))
		};

		/*memcpy(arg[0]->GetContents().Data(), m_buffer.data(), m_buffer.size() * sizeof(uint8_t));
		memcpy(arg[1]->GetContents().Data(), m_bufferMap.data(), m_bufferMap.size() * sizeof(uint32_t));
		memcpy(arg[2]->GetContents().Data(), m_poses.data(), m_poses.size() * sizeof(float));*/


		callback->Call(m_isolate->GetCurrentContext(), _this, 3, (v8::Local<v8::Value>*)arg);

	end:	
		m_buffer.clear();
		m_bufferMap.clear();
		m_poses.clear();
	}

};

int ReadFunc(void* _ptr, uint8_t* buf, int buf_size);
int64_t SeekFunc(void* ptr, int64_t pos, int whence);

std::unique_ptr<tflite::FlatBufferModel> g_model;

std::mutex g_coreMutex;
ProcessingCore* g_core = 0;

class ProcessingThread
{
public:
	std::unique_ptr<tflite::Interpreter> m_interpreter;
	TfLiteTensor* m_inputTensor = 0;
	uint8_t* m_inputBuffer = 0;
	TfLiteTensor* m_outputTensor = 0;
	float* m_outputBuffer = 0;

	cv::Mat m_image;
	cv::Mat m_downScaledImage;

	std::vector<float*> m_pose;

public:
	const AVCodec* m_codec = 0;
	AVCodecContext* m_ctx = 0;
	AVFormatContext* m_fmtCtx1 = 0;
	AVFormatContext* m_fmtCtx2 = 0;
	AVPacket* m_pkt = 0;
	SwsContext* m_sctx = 0;
	AVFrame* m_frame = 0;

public:
	bool m_isRunning = true;
	std::thread* m_thread = 0;

public:
	std::mutex m_bufferMutex;

	//where js side place buffer that need process
	std::string m_buffers;
	std::vector<size_t> m_bufferChunkSize;

	//then buffer will go here to ready for processing
	std::string m_streamBuffer;
	//and then go to stream to process
	std::stringstream m_stream;
	std::vector<size_t> m_streamChunkSize;

public:
	std::vector<cv::Mat> m_predictedFrame;
	std::vector<float> m_predictedPose;
	std::vector<uint8_t> m_imgEncodeBuf;

public:
	void LoadModel(const char* path)
	{
		//"D:\\program\\tensorflow\\model\\pose\\model_movenet_singlepose_thunder.tflite"
		if(!g_model) g_model = tflite::FlatBufferModel::BuildFromFile(path);

		if (g_model == nullptr)
		{
			std::cout << "[ERROR]\n";
			exit(-1);
		}

		tflite::ops::builtin::BuiltinOpResolver resolver;
		tflite::InterpreterBuilder builder(*g_model, resolver);

		TfLiteGpuDelegateOptionsV2 options = TfLiteGpuDelegateOptionsV2Default();

		auto* delegate = TfLiteGpuDelegateV2Create(&options);

		builder.AddDelegate(delegate);

		if (builder(&m_interpreter) != kTfLiteOk)
		{
			std::cout << "[ERROR]\n";
			exit(-1);
		}

		if (m_interpreter->AllocateTensors() != kTfLiteOk)
		{
			std::cout << "[ERROR]\n";
			exit(-1);
		}

		m_inputTensor = m_interpreter->tensor(m_interpreter->inputs()[0]);

		m_inputBuffer = m_interpreter->typed_input_tensor<uint8_t>(0);

		m_outputBuffer = m_interpreter->typed_output_tensor<float>(0);

		m_outputTensor = m_interpreter->tensor(m_interpreter->outputs()[0]);

		m_pose = Reshape(m_outputBuffer, 17, 3);

		std::cout.precision(6);
	}

	void DrawPose(cv::Mat& img)
	{
		auto h = img.rows;
		auto w = img.cols;

		auto& pose = m_pose;

		for (auto& p : KEYPOINT_EDGE_INDS_TO_COLOR)
		{
			auto& start = pose[p.start];
			auto& end = pose[p.end];

			auto y1 = (int)(start[0] * h);
			auto x1 = (int)(start[1] * w);

			auto y2 = (int)(end[0] * h);
			auto x2 = (int)(end[1] * w);

			cv::line(img, { x1,y1 }, { x2,y2 }, COLOR[p.color]);
		}
	}

	void InitFFMPEG()
	{
		m_buffers.reserve(4 * 1024 * 1024);

		m_pkt = av_packet_alloc();
		AVPacket& pkt = *m_pkt;

		const AVCodec*& codec = m_codec;
		AVCodecContext*& ctx = m_ctx;
		AVFormatContext*& fmt_ctx2 = m_fmtCtx2;
		fmt_ctx2 = avformat_alloc_context();

		fmt_ctx2->flags = AVFMT_FLAG_CUSTOM_IO;

		const int iBufSize = 32 * 1024;
		BYTE* pBuffer = new BYTE[iBufSize];

		std::fstream pInStream = std::fstream(
			".\\resource\\media\\binary\\0",
			std::ios::binary | std::ios::in);

		m_stream << pInStream.rdbuf();

		AVIOContext* pIOCtx = avio_alloc_context(pBuffer, iBufSize,		// internal Buffer and its size
			0,															// bWriteable (1=true,0=false) 
			this,														// user data ; will be passed to our callback functions
			ReadFunc,
			0,
			SeekFunc);

		//open bootstrap config file 
		AVFormatContext*& fmt_ctx = m_fmtCtx1;
		open_input_file(".\\resource\\media\\binary\\0", &ctx, &fmt_ctx, &codec);

		fmt_ctx2->iformat = fmt_ctx->iformat;
		fmt_ctx2->oformat = fmt_ctx->oformat;
		fmt_ctx2->pb = pIOCtx;

		auto r1 = avformat_open_input(&fmt_ctx2, "", NULL, NULL);

		if (r1 < 0)
		{
			av_log(NULL, AV_LOG_ERROR, "Cannot open input file\n");
			exit(-1);
		}

		m_frame = av_frame_alloc();

		m_image = cv::Mat(ctx->height, ctx->width, CV_8UC(3));

		m_sctx = sws_getContext(ctx->width, ctx->height,
			AV_PIX_FMT_YUV420P, ctx->width, ctx->height,
			AV_PIX_FMT_BGR24, 0, 0, 0, 0);
	}

	void Start()
	{
		m_imgEncodeBuf.resize(32 * 1024 * 1024);

		std::cout << "[Loading FFmpeg ...]\n";
		InitFFMPEG();
		std::cout << "[FFmpeg loaded]\n";
		m_thread = new std::thread(Loop, this);
		m_thread->detach();
	}

	inline void FlushResult()
	{
		if (m_predictedFrame.size() > PREDICTED_FRAME_LIMIT)
		{
			if (WaitToLock(g_coreMutex))
			{
				if (g_core)
				{
					g_core->m_poses.insert(g_core->m_poses.begin(), m_predictedPose.begin(), m_predictedPose.end());

					for (auto& img : m_predictedFrame)
					{
						cv::imencode(".jpg", img, m_imgEncodeBuf);
						g_core->m_buffer.insert(g_core->m_buffer.end(), m_imgEncodeBuf.begin(), m_imgEncodeBuf.end());
						g_core->m_bufferMap.push_back(m_imgEncodeBuf.size());
					}
				}

				Release(g_coreMutex);
			}

			m_predictedFrame.clear();
			m_predictedPose.clear();
		}
	}

	void Predict()
	{
		size_t channels = m_image.channels();

		cv::resize(m_image, m_downScaledImage, { 256,256 }, 0, 0, cv::InterpolationFlags::INTER_CUBIC);

		::memcpy(&m_inputBuffer[0], m_downScaledImage.ptr(), 256 * 256 * channels);

		m_interpreter->Invoke();

		DrawPose(m_image);

		m_predictedFrame.push_back(m_image.clone());
		m_predictedPose.insert(m_predictedPose.end(), m_outputBuffer, m_outputBuffer + 17 * 3);

		FlushResult();
	}

	inline void NextChunkToStream()
	{
		std::cout << "[NextChunkToStream]\n";

		auto* pStream = &m_stream;
		auto& streamStack = m_streamChunkSize;

		pStream->str("");
		pStream->clear();

		auto nextSize = streamStack.back();
		streamStack.pop_back();

		auto begin = &m_streamBuffer.back() - nextSize;

		pStream->write(begin, nextSize);

		m_streamBuffer.resize(m_streamBuffer.size() - nextSize);
	}

	inline void NextStreamBuffer()
	{
		std::cout << "[NextStreamBuffer]\n";

		auto* pStream = &m_stream;
		auto& streamStack = m_streamChunkSize;

		auto& queue = m_bufferChunkSize;
		auto& streamBuffer = m_streamBuffer;

		//std::cout << "[QUEUE SIZE] " << queue.size() << "\n";
		for (long long i = queue.size() - 1; i != -1; i--)
		{
			auto nextSize = queue[i];

			streamStack.push_back(nextSize);

			auto begin = &m_buffers.back() - nextSize;
			streamBuffer.append(begin, nextSize);

			m_buffers.resize(m_buffers.size() - nextSize);

		}

		queue.clear();
		//std::cout << m_buffers.size() << "\n";
		//assert(m_buffers.size() == 0);

		//thread->m_buffers.clear();
	}

public:
	static void Loop(ProcessingThread* thread)
	{
		//cv::namedWindow("Image", cv::WindowFlags::WINDOW_NORMAL);
		//cv::resizeWindow("Image", 640, 360);

		int outLineSize[1] = { 3 * thread->m_ctx->width };

		while (thread->m_isRunning)
		{
			auto ret = av_read_frame(thread->m_fmtCtx2, thread->m_pkt);

			if (ret < 0)
			{
				break;
			}

			if (thread->m_pkt->size)
			{
				ret = avcodec_send_packet(thread->m_ctx, thread->m_pkt);

				if (ret < 0)
				{
					fprintf(stderr, "Error sending a packet for decoding\n");
					exit(1);
				}

				while (ret >= 0)
				{
					ret = avcodec_receive_frame(thread->m_ctx, thread->m_frame);

					if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
					{
						break;
					}
					else if (ret < 0)
					{
						av_log(NULL, AV_LOG_ERROR, "Error while receiving a frame from the decoder\n");
						return;
					}

					sws_scale(thread->m_sctx, thread->m_frame->data, thread->m_frame->linesize, 0, 
						thread->m_ctx->height, &thread->m_image.data, outLineSize);

					thread->Predict();

					//demoshow
					//cv::imshow("Image", thread->m_image);

					//cv::waitKey(SLEEP_TIME);
				}
			}
		}
	}
};


using _IStream = std::stringstream;

int ReadFunc(void* _ptr, uint8_t* buf, int buf_size)
{
	ProcessingThread* thread = (ProcessingThread*)_ptr;
	_IStream* pStream = &thread->m_stream;

	auto readByte = pStream->read((char*)buf, buf_size).gcount();

	auto& streamStack = thread->m_streamChunkSize;

	if (readByte == 0)
	{
		while (thread->m_isRunning)
		{
			Sleep(64);

			//if (streamStack.size() != 0)
			//{
			//	thread->NextChunkToStream();

			//	//get more buffer to work
			//	readByte = pStream->read((char*)buf, buf_size).gcount();

			//	return readByte;
			//}		
			//else 
			if (thread->m_buffers.size() != 0)
			{
				if (WaitToLock(thread->m_bufferMutex))
				{
					//std::cout << "[BUFFER SIZE] " << thread->m_buffers.size() << "\n";
				
					//thread->NextStreamBuffer();

					//thread->NextChunkToStream();

					pStream->str("");
					pStream->clear();
					pStream->write(thread->m_buffers.c_str(), thread->m_buffers.size());
					thread->m_buffers.clear();

					readByte = pStream->read((char*)buf, buf_size).gcount();

					//std::cout << "[READ BYTE] " << readByte << "\n";

					Release(thread->m_bufferMutex);
					
					return readByte;
				}
			}
		}
		return AVERROR_EOF;
	}

	return readByte;
}

int64_t SeekFunc(void* _ptr, int64_t pos, int whence)
{
	//std::cout << "SeekFunc() called\n";

	ProcessingThread* thread = (ProcessingThread*)_ptr;
	_IStream* pStream = &thread->m_stream;

	if (whence != AVSEEK_SIZE)
	{
		if (!pStream->seekg(pos, whence))
		{
			return -1;
		}
	}
	else
	{
		return -1;
	}

	return pStream->tellg();
}

//just single thread for testing
ProcessingThread* g_thread = 0;

void Hello(const v8::FunctionCallbackInfo<v8::Value>& args) 
{
	v8::Isolate* isolate = args.GetIsolate();
	auto message = v8::String::NewFromUtf8(isolate, "Hello world from C++").ToLocalChecked();
	args.GetReturnValue().Set(message);
}


template<typename T>
struct WeakCallBackInfoStruct
{
	v8::Persistent<v8::Value> persistent;
	T* ptr = 0;

	inline static void Destructor(const v8::WeakCallbackInfo<WeakCallBackInfoStruct<T>>& data)
	{
		auto param = data.GetParameter();
		param->persistent.Reset();
		delete param->ptr;
		delete param;

		if (WaitToLock(g_coreMutex))
		{
			g_core = 0;
			Release(g_coreMutex);
		}
		
	}
};

void SendBuffer(const v8::FunctionCallbackInfo<v8::Value>& args)
{
	if (args.Length() != 1) return;

	v8::Isolate* isolate = args.GetIsolate();

	auto arr = args[0];

	//if (!arr->IsArray()) return;

	

	if (!arr->IsArrayBuffer())
	{
		return;
	}

	const uint8_t* buffer = 0;
	size_t bufferSize = 0;
	v8::Local<v8::ArrayBuffer> jsbuf = v8::Local<v8::ArrayBuffer>::Cast(arr);

	//std::cout << *jsbuf << "\n";

	auto ptr = jsbuf->GetBackingStore();

	//std::cout << ptr << "\n";

	buffer = (const uint8_t*)(ptr->Data());

	bufferSize = jsbuf->ByteLength();

	//std::cout << "[IsArrayBuffer] " << (void*)buffer << "  -->  " << bufferSize << "\n";


	if (buffer)
	{
		auto _this = args.This();
		auto ptr = _this->GetAlignedPointerFromInternalField(0);

		auto core = (ProcessingCore*)ptr;

		if (core->m_callback.IsEmpty())
		{
			core->GetCallback(isolate, _this);
		}

		//core->ProcessBuffer(isolate, arr);

		if (WaitToLock(g_thread->m_bufferMutex))
		{
			//std::cout << "[SendBuffer] " << bufferSize << "\n";
			g_thread->m_buffers.append((char*)buffer, bufferSize);

			//g_thread->m_bufferChunkSize.push_back(bufferSize);

			Release(g_thread->m_bufferMutex);
		}
	}
	
	//std::cout << "[SendBuffer]\n";

}

void Interval(const v8::FunctionCallbackInfo<v8::Value>& args)
{
	if (g_core)
	{
		if (WaitToLock(g_coreMutex))
		{
			g_core->CallCallback();
			Release(g_coreMutex);
		}
	}
}

void NewProcessingCore(const v8::FunctionCallbackInfo<v8::Value>& args)
{
	v8::Isolate* isolate = args.GetIsolate();
	auto context = isolate->GetCurrentContext();

	v8::Local<v8::ObjectTemplate> templ = v8::ObjectTemplate::New(isolate);
	templ->SetInternalFieldCount(1);

	templ->Set(isolate, "SendBuffer", v8::FunctionTemplate::New(isolate, SendBuffer));

	auto ret = templ->NewInstance(context).ToLocalChecked();

	auto p = new ProcessingCore();

	ret->SetAlignedPointerInInternalField(0, p);

	auto callback = new WeakCallBackInfoStruct<ProcessingCore>();
	callback->ptr = p;
	auto& obj = callback->persistent;

	obj.Reset(isolate, ret);

	obj.SetWeak(callback,
		WeakCallBackInfoStruct<ProcessingCore>::Destructor,
		v8::WeakCallbackType::kParameter
	);

	g_core = p;
	g_thread->m_predictedFrame.clear();
	g_core->m_isolate = isolate;
	
	std::cout << "[NewProcessingCore]\n";

	args.GetReturnValue().Set(ret);
}


void Init()
{
	if (!g_thread)
	{
		g_thread = new ProcessingThread();
		g_thread->LoadModel(".\\resource\\tf_model\\model_movenet_singlepose_thunder.tflite");
		g_thread->Start();
	}
}

void Initialize(v8::Local<v8::Object> exports) 
{
	NODE_SET_METHOD(exports, "Hello", Hello);
	NODE_SET_METHOD(exports, "NewProcessingCore", NewProcessingCore);

	NODE_SET_METHOD(exports, "Interval", Interval);

	std::cout << "[BEGIN]\n";
	Init();
	std::cout << "[END]\n";
}

NODE_MODULE(module_name, Initialize)