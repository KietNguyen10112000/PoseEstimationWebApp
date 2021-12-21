{
  "targets": [
    {
      "target_name": "processing",
      "sources": [ "src/addons/Processing.cpp" ],
      'msvs_settings': {
        'VCLinkerTool': {
          'AdditionalOptions': [ '/NODEFAULTLIB:library' ],
        },
      },
      'configurations': {
        'Debug': {
          'msvs_settings': {
            'VCCLCompilerTool': {
              # 0 - MultiThreaded (/MT)
              # 1 - MultiThreadedDebug (/MTd)
              # 2 - MultiThreadedDLL (/MD)
              # 3 - MultiThreadedDebugDLL (/MDd)
              'RuntimeLibrary': 1,
            }
          }
        },
        'Release': {
          'msvs_settings': {
            'VCCLCompilerTool': {
              'RuntimeLibrary': 2,  # shared release
            },
          },
        },
      },
      'win_delay_load_hook': 'false',
      'include_dirs': [
        'D:\\program\\tensorflow\\include',
        'D:\\program\\tensorflow\\src\\tensorflow_src\\tensorflow\\lite\\tools\\cmake\\native_tools\\flatbuffers\\flatbuffers\\include',
        'D:\\program\\opencv\\build\\include',
        'D:\\program\\tensorflow\\src\\tensorflow_src',
        'D:\\program\\ffmpeg\\libffmpeg\\installed\\x64-windows\\include'
      ],
      'libraries': [
        '-lD:\\program\\tensorflow\\src\\tflite_build_gpu\\pthreadpool\\Release\\pthreadpool.lib',
        '-lD:\\program\\tensorflow\\src\\tflite_build_gpu\\Release\\tensorflow-lite.lib',
        '-lD:\\program\\tensorflow\\src\\tflite_build_gpu\\_deps\\abseil-cpp-build\\absl\\base\\Release\\absl_base.lib',
        '-lD:\\program\\tensorflow\\src\\tflite_build_gpu\\_deps\\abseil-cpp-build\\absl\\base\\Release\\absl_exponential_biased.lib',
        '-lD:\\program\\tensorflow\\src\\tflite_build_gpu\\_deps\\abseil-cpp-build\\absl\\base\\Release\\absl_log_severity.lib',
        '-lD:\\program\\tensorflow\\src\\tflite_build_gpu\\_deps\\abseil-cpp-build\\absl\\base\\Release\\absl_malloc_internal.lib',
        '-lD:\\program\\tensorflow\\src\\tflite_build_gpu\\_deps\\abseil-cpp-build\\absl\\base\\Release\\absl_raw_logging_internal.lib',
        '-lD:\\program\\tensorflow\\src\\tflite_build_gpu\\_deps\\abseil-cpp-build\\absl\\base\\Release\\absl_spinlock_wait.lib',
        '-lD:\\program\\tensorflow\\src\\tflite_build_gpu\\_deps\\abseil-cpp-build\\absl\\base\\Release\\absl_throw_delegate.lib',
        '-lD:\\program\\tensorflow\\src\\tflite_build_gpu\\_deps\\abseil-cpp-build\\absl\\container\\Release\\absl_hashtablez_sampler.lib',
        '-lD:\\program\\tensorflow\\src\\tflite_build_gpu\\_deps\\abseil-cpp-build\\absl\\container\\Release\\absl_raw_hash_set.lib',
        '-lD:\\program\\tensorflow\\src\\tflite_build_gpu\\_deps\\abseil-cpp-build\\absl\\debugging\\Release\\absl_debugging_internal.lib',
        '-lD:\\program\\tensorflow\\src\\tflite_build_gpu\\_deps\\abseil-cpp-build\\absl\\debugging\\Release\\absl_demangle_internal.lib',
        '-lD:\\program\\tensorflow\\src\\tflite_build_gpu\\_deps\\abseil-cpp-build\\absl\\debugging\\Release\\absl_stacktrace.lib',
        '-lD:\\program\\tensorflow\\src\\tflite_build_gpu\\_deps\\abseil-cpp-build\\absl\\debugging\\Release\\absl_symbolize.lib',
        '-lD:\\program\\tensorflow\\src\\tflite_build_gpu\\_deps\\abseil-cpp-build\\absl\\flags\\Release\\absl_flags.lib',
        '-lD:\\program\\tensorflow\\src\\tflite_build_gpu\\_deps\\abseil-cpp-build\\absl\\flags\\Release\\absl_flags_commandlineflag.lib',
        '-lD:\\program\\tensorflow\\src\\tflite_build_gpu\\_deps\\abseil-cpp-build\\absl\\flags\\Release\\absl_flags_commandlineflag_internal.lib',
        '-lD:\\program\\tensorflow\\src\\tflite_build_gpu\\_deps\\abseil-cpp-build\\absl\\flags\\Release\\absl_flags_config.lib',
        '-lD:\\program\\tensorflow\\src\\tflite_build_gpu\\_deps\\abseil-cpp-build\\absl\\flags\\Release\\absl_flags_internal.lib',
        '-lD:\\program\\tensorflow\\src\\tflite_build_gpu\\_deps\\abseil-cpp-build\\absl\\flags\\Release\\absl_flags_marshalling.lib',
        '-lD:\\program\\tensorflow\\src\\tflite_build_gpu\\_deps\\abseil-cpp-build\\absl\\flags\\Release\\absl_flags_private_handle_accessor.lib',
        '-lD:\\program\\tensorflow\\src\\tflite_build_gpu\\_deps\\abseil-cpp-build\\absl\\flags\\Release\\absl_flags_program_name.lib',
        '-lD:\\program\\tensorflow\\src\\tflite_build_gpu\\_deps\\abseil-cpp-build\\absl\\flags\\Release\\absl_flags_reflection.lib',
        '-lD:\\program\\tensorflow\\src\\tflite_build_gpu\\_deps\\abseil-cpp-build\\absl\\hash\\Release\\absl_city.lib',
        '-lD:\\program\\tensorflow\\src\\tflite_build_gpu\\_deps\\abseil-cpp-build\\absl\\hash\\Release\\absl_hash.lib',
        '-lD:\\program\\tensorflow\\src\\tflite_build_gpu\\_deps\\abseil-cpp-build\\absl\\hash\\Release\\absl_wyhash.lib',
        '-lD:\\program\\tensorflow\\src\\tflite_build_gpu\\_deps\\abseil-cpp-build\\absl\\numeric\\Release\\absl_int128.lib',
        '-lD:\\program\\tensorflow\\src\\tflite_build_gpu\\_deps\\abseil-cpp-build\\absl\\status\\Release\\absl_status.lib',
        '-lD:\\program\\tensorflow\\src\\tflite_build_gpu\\_deps\\abseil-cpp-build\\absl\\strings\\Release\\absl_cord.lib',
        '-lD:\\program\\tensorflow\\src\\tflite_build_gpu\\_deps\\abseil-cpp-build\\absl\\strings\\Release\\absl_strings.lib',
        '-lD:\\program\\tensorflow\\src\\tflite_build_gpu\\_deps\\abseil-cpp-build\\absl\\strings\\Release\\absl_strings_internal.lib',
        '-lD:\\program\\tensorflow\\src\\tflite_build_gpu\\_deps\\abseil-cpp-build\\absl\\strings\\Release\\absl_str_format_internal.lib',
        '-lD:\\program\\tensorflow\\src\\tflite_build_gpu\\_deps\\abseil-cpp-build\\absl\\synchronization\\Release\\absl_graphcycles_internal.lib',
        '-lD:\\program\\tensorflow\\src\\tflite_build_gpu\\_deps\\abseil-cpp-build\\absl\\synchronization\\Release\\absl_synchronization.lib',
        '-lD:\\program\\tensorflow\\src\\tflite_build_gpu\\_deps\\abseil-cpp-build\\absl\\time\\Release\\absl_civil_time.lib',
        '-lD:\\program\\tensorflow\\src\\tflite_build_gpu\\_deps\\abseil-cpp-build\\absl\\time\\Release\\absl_time.lib',
        '-lD:\\program\\tensorflow\\src\\tflite_build_gpu\\_deps\\abseil-cpp-build\\absl\\time\\Release\\absl_time_zone.lib',
        '-lD:\\program\\tensorflow\\src\\tflite_build_gpu\\_deps\\abseil-cpp-build\\absl\\types\\Release\\absl_bad_any_cast_impl.lib',
        '-lD:\\program\\tensorflow\\src\\tflite_build_gpu\\_deps\\abseil-cpp-build\\absl\\types\\Release\\absl_bad_optional_access.lib',
        '-lD:\\program\\tensorflow\\src\\tflite_build_gpu\\_deps\\abseil-cpp-build\\absl\\types\\Release\\absl_bad_variant_access.lib',
        '-lD:\\program\\tensorflow\\src\\tflite_build_gpu\\_deps\\clog-build\\Release\\clog.lib',
        '-lD:\\program\\tensorflow\\src\\tflite_build_gpu\\_deps\\cpuinfo-build\\Release\\cpuinfo.lib',
        '-lD:\\program\\tensorflow\\src\\tflite_build_gpu\\_deps\\farmhash-build\\Release\\farmhash.lib',
        '-lD:\\program\\tensorflow\\src\\tflite_build_gpu\\_deps\\fft2d-build\\Release\\fft2d_fftsg.lib',
        '-lD:\\program\\tensorflow\\src\\tflite_build_gpu\\_deps\\fft2d-build\\Release\\fft2d_fftsg2d.lib',
        '-lD:\\program\\tensorflow\\src\\tflite_build_gpu\\_deps\\flatbuffers-build\\Release\\flatbuffers.lib',
        '-lD:\\program\\tensorflow\\src\\tflite_build_gpu\\_deps\\ruy-build\\ruy\\profiler\\Release\\ruy_profiler_instrumentation.lib',
        '-lD:\\program\\tensorflow\\src\\tflite_build_gpu\\_deps\\ruy-build\\ruy\\Release\\ruy_allocator.lib',
        '-lD:\\program\\tensorflow\\src\\tflite_build_gpu\\_deps\\ruy-build\\ruy\\Release\\ruy_apply_multiplier.lib',
        '-lD:\\program\\tensorflow\\src\\tflite_build_gpu\\_deps\\ruy-build\\ruy\\Release\\ruy_blocking_counter.lib',
        '-lD:\\program\\tensorflow\\src\\tflite_build_gpu\\_deps\\ruy-build\\ruy\\Release\\ruy_block_map.lib',
        '-lD:\\program\\tensorflow\\src\\tflite_build_gpu\\_deps\\ruy-build\\ruy\\Release\\ruy_context.lib',
        '-lD:\\program\\tensorflow\\src\\tflite_build_gpu\\_deps\\ruy-build\\ruy\\Release\\ruy_context_get_ctx.lib',
        '-lD:\\program\\tensorflow\\src\\tflite_build_gpu\\_deps\\ruy-build\\ruy\\Release\\ruy_cpuinfo.lib',
        '-lD:\\program\\tensorflow\\src\\tflite_build_gpu\\_deps\\ruy-build\\ruy\\Release\\ruy_ctx.lib',
        '-lD:\\program\\tensorflow\\src\\tflite_build_gpu\\_deps\\ruy-build\\ruy\\Release\\ruy_denormal.lib',
        '-lD:\\program\\tensorflow\\src\\tflite_build_gpu\\_deps\\ruy-build\\ruy\\Release\\ruy_frontend.lib',
        '-lD:\\program\\tensorflow\\src\\tflite_build_gpu\\_deps\\ruy-build\\ruy\\Release\\ruy_have_built_path_for_avx.lib',
        '-lD:\\program\\tensorflow\\src\\tflite_build_gpu\\_deps\\ruy-build\\ruy\\Release\\ruy_have_built_path_for_avx2_fma.lib',
        '-lD:\\program\\tensorflow\\src\\tflite_build_gpu\\_deps\\ruy-build\\ruy\\Release\\ruy_have_built_path_for_avx512.lib',
        '-lD:\\program\\tensorflow\\src\\tflite_build_gpu\\_deps\\ruy-build\\ruy\\Release\\ruy_kernel_arm.lib',
        '-lD:\\program\\tensorflow\\src\\tflite_build_gpu\\_deps\\ruy-build\\ruy\\Release\\ruy_kernel_avx.lib',
        '-lD:\\program\\tensorflow\\src\\tflite_build_gpu\\_deps\\ruy-build\\ruy\\Release\\ruy_kernel_avx2_fma.lib',
        '-lD:\\program\\tensorflow\\src\\tflite_build_gpu\\_deps\\ruy-build\\ruy\\Release\\ruy_kernel_avx512.lib',
        '-lD:\\program\\tensorflow\\src\\tflite_build_gpu\\_deps\\ruy-build\\ruy\\Release\\ruy_pack_arm.lib',
        '-lD:\\program\\tensorflow\\src\\tflite_build_gpu\\_deps\\ruy-build\\ruy\\Release\\ruy_pack_avx.lib',
        '-lD:\\program\\tensorflow\\src\\tflite_build_gpu\\_deps\\ruy-build\\ruy\\Release\\ruy_pack_avx2_fma.lib',
        '-lD:\\program\\tensorflow\\src\\tflite_build_gpu\\_deps\\ruy-build\\ruy\\Release\\ruy_pack_avx512.lib',
        '-lD:\\program\\tensorflow\\src\\tflite_build_gpu\\_deps\\ruy-build\\ruy\\Release\\ruy_prepacked_cache.lib',
        '-lD:\\program\\tensorflow\\src\\tflite_build_gpu\\_deps\\ruy-build\\ruy\\Release\\ruy_prepare_packed_matrices.lib',
        '-lD:\\program\\tensorflow\\src\\tflite_build_gpu\\_deps\\ruy-build\\ruy\\Release\\ruy_system_aligned_alloc.lib',
        '-lD:\\program\\tensorflow\\src\\tflite_build_gpu\\_deps\\ruy-build\\ruy\\Release\\ruy_thread_pool.lib',
        '-lD:\\program\\tensorflow\\src\\tflite_build_gpu\\_deps\\ruy-build\\ruy\\Release\\ruy_trmul.lib',
        '-lD:\\program\\tensorflow\\src\\tflite_build_gpu\\_deps\\ruy-build\\ruy\\Release\\ruy_tune.lib',
        '-lD:\\program\\tensorflow\\src\\tflite_build_gpu\\_deps\\ruy-build\\ruy\\Release\\ruy_wait.lib',
        '-lD:\\program\\tensorflow\\src\\tflite_build_gpu\\_deps\\xnnpack-build\\Release\\XNNPACK.lib',
        '-lD:\\program\\opencv\\build\\x64\\vc15\\lib\\opencv_world440.lib',
        '-lD:\\program\\ffmpeg\\libffmpeg\\installed\\x64-windows\\lib\\avcodec.lib',
        '-lD:\\program\\ffmpeg\\libffmpeg\\installed\\x64-windows\\lib\\avdevice.lib',
        '-lD:\\program\\ffmpeg\\libffmpeg\\installed\\x64-windows\\lib\\avfilter.lib',
        '-lD:\\program\\ffmpeg\\libffmpeg\\installed\\x64-windows\\lib\\avformat.lib',
        '-lD:\\program\\ffmpeg\\libffmpeg\\installed\\x64-windows\\lib\\avutil.lib',
        '-lD:\\program\\ffmpeg\\libffmpeg\\installed\\x64-windows\\lib\\opus.lib',
        '-lD:\\program\\ffmpeg\\libffmpeg\\installed\\x64-windows\\lib\\swresample.lib',
        '-lD:\\program\\ffmpeg\\libffmpeg\\installed\\x64-windows\\lib\\swscale.lib',
        '-lD:\\program\\ffmpeg\\libffmpeg\\installed\\x64-windows\\lib\\vpx.lib'
        
      ]
    }
  ]
}