var openedWebsockets = new Array();
var recvBuffer = new Array();

function createWebSocket(wsURL) {
    Module["noExitRuntime"] = true;
    console.log('creating websocket : ' + Pointer_stringify(wsURL));
    var ws = new WebSocket(Pointer_stringify(wsURL));
    if (!ws) {
	console.log("Error : unable to open WebSocket at url : " + wsURL + "\n");
    }
    var wsId = openedWebsockets.length;
    openedWebsockets.push(ws);
    
    ws.onmessage = function (evt) { 
	var received_msg = evt.data;
	console.log('received : ' + received_msg);
	recvBuffer.push(received_msg);
    };
    ws.onopen = function(){
	alert('Websocket is open');
    }
    ws.onclose = function () { 
	alert('Websocket has been closed');
    };
    return wsId;
}

function sendJS(wsId, gremlinRequest) {
    var ws = openedWebsockets[wsId];
    waitForSocketConnection(ws, function(){
        console.log("message sent!!!");
        ws.send(Pointer_stringify(gremlinRequest));
    });
}

function checkAnswerJS(wsId, timeout, answer) {
    var ws = openedWebsockets[wsId];
    console.log("receiving : " + recvBuffer.length);
    // setTimeout(function(){
    if (recvBuffer.length == 0) {
	return false;
    }
    else {
	var resp = recvBuffer.pop();
	writeStringToMemory(resp,answer);
	console.log(resp);
	return true;
    }
    // }, timeout);
}

function waitForSocketConnection(socket, callback){
    setTimeout(
        function () {
            if (socket.readyState === 1) {
                console.log("Connection is made")
                if(callback != null){
                    callback();
                }
                return;
            } else {
                console.log("wait for connection...")
                waitForSocketConnection(socket, callback);
            }
        }, 5); // wait 5 milisecond for the connection...
}

function isRecvBufferEmpty(){
    return (recvBuffer.length == 0);
}

