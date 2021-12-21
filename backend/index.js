require('dotenv').config();

const processing = require('./build/Release/processing');

console.log(processing.Hello());

setInterval(processing.Interval, 16);

const asyncMutex = require('async-mutex');

//crictical resources
var counter = 0;
const clients = {};
const mutex = new asyncMutex.Mutex();

function config(socket, msg) {
    try {
        //console.log(msg)
        socket.mydata = JSON.parse(msg.utf8Data)
        socket.mydata.counter = 0
        console.log(socket.mydata)
    } catch (error) {
        socket.mydata = null
    }
}

function onMessage(socket, msg) {

    if(msg.type == 'utf8') {
        config(socket, msg)
        return
    }

    if(msg.type != 'binary') return
    
    //console.log(buffer)
    /* let buffer = Buffer.from(msg.binaryData)
    fs.writeFileSync(`./resource/media/binary/${socket.mydata.counter}`, buffer, 'binary', (err) => {console.log(err)})
    socket.mydata.counter++ */

    if (socket.mydata.counter == 0) {
        socket.mydata.counter = 1
        return
    }

    let arr = Uint8Array.from(msg.binaryData)
    let buffer = arr.buffer
    socket.processingCore.SendBuffer(buffer)

}

// callback function to run when we have a successful websocket connection request
function onRequest(socket) {

    // get origin of request 
    var origin = socket.origin + socket.resource;

    // accept socket origin 
    var websocket = socket.accept(null, origin);

    mutex.runExclusive(function() {
        websocket.processingCore = processing.NewProcessingCore()

        websocket.processingCore.callback = function(buffer, bufferMap, pose) {
            //let bufMap = new Uint32Array(bufferMap)
            //console.log(bufMap)
            //let _pose = new Float32Array(pose)
            //console.log(_pose)

            let arr = new Uint32Array(3)
            arr[0] = buffer.byteLength
            arr[1] = bufferMap.byteLength
            arr[2] = pose.byteLength

            let resultBuffer = new Uint8Array(4 * 3 + buffer.byteLength + bufferMap.byteLength + pose.byteLength)

            resultBuffer.set(new Uint8Array(arr.buffer))
            resultBuffer.set(new Uint8Array(buffer), 12)
            resultBuffer.set(new Uint8Array(bufferMap), buffer.byteLength + 12)
            resultBuffer.set(new Uint8Array(pose), bufferMap.byteLength + buffer.byteLength + 12)

            websocket.send(Buffer.from(resultBuffer))
            
            /* websocket.send("NewPackage");
            websocket.send(Buffer.from(buffer))
            websocket.send(Buffer.from(bufferMap))
            websocket.send(Buffer.from(pose)) */
        }

        console.log(`New request => counter: ${counter}`)
        clients[`${counter}`] = websocket
        websocket.myid = counter

        counter++
    })

    // websocket message event for when message is received
    websocket.on('message', function(message) {
        if(!message || !websocket) return;
        onMessage(websocket, message)
    });
    // websocket event when the connection is closed 
    websocket.on('close', function() {
        console.log("closed")
        mutex.runExclusive(function() {
            //clients[`${websocket.id}`] = null
            delete clients[`${websocket.myid}`]
            console.log(`Remove client ${websocket.myid}`)
        })
    });

    websocket.on('error', function(e) {
        console.log(e)
    });
    
}


const fs = require('fs');


const options = {
    key: fs.readFileSync('./cert/test.key'),
    cert: fs.readFileSync('./cert/test.der')
};

const app = require('https').createServer(options, function(request, response) {
    const frontEndHtml = fs.readFileSync('../frontend/index.html');
    response.write(frontEndHtml)
    response.end()
});

// require websocket and setup server.
const WebSocketServer = require('websocket').server;
WebSocketServer.binaryType = 'arraybuffer';

// wait for when a connection request comes in 
new WebSocketServer({
    httpServer: app, 
    autoAcceptConnections: false,
    maxReceivedFrameSize: 100000000,
	maxReceivedMessageSize: 100000000,
}).on('request', onRequest);

// listen on app port 
app.listen(process.env.PORT, process.env.HOST, () => {
    console.log(`Server is listening at ${process.env.HOST}:${process.env.PORT}`);
});

//handle exceptions and exit gracefully 
process.on('unhandledRejection', (reason, promise) => {
  process.exit(1);
});