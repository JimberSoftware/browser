

class SocketHandler {

    constructor() {
        this.socketOpenFunctions = [];
        this.socketClosedFunctions = [];
        this.binaryMessageHandlers = [];
        this.asciiMessageHandlers = [];
        this.browserReadyFunctions = [];
        this.hasBeenOpen = false;
        this.timesNotConnected = 0;
        this.addAsciiMessageHandler((cmd) => {
            if (cmd[0] == "socketready") {
                this.ready();
            }
        })
    }

    init(location) {
        this.location = location;
        if (this.websocket) {
            this.websocket.close(3001);
        }

        this.websocket = new WebSocket(this.location);

        this.websocket.handler = this;
        // this.websocket.onopen = this.onOpen; // moved to socket sending a 'readysocket' message
        this.websocket.onclose = this.onClose;
        this.websocket.onmessage = this.onMessage;
        this.websocket.onerror = this.onError;

    }

    addSocketOpenHandler(fnc) {
        this.socketOpenFunctions.push(fnc);
    }
    addBinaryMessageHandler(fnc) {
        this.binaryMessageHandlers.push(fnc);
    }
    addAsciiMessageHandler(fnc) {
        this.asciiMessageHandlers.push(fnc);
    }
    addSocketClosedHandler(fnc) {
        this.socketClosedFunctions.push(fnc);
    }
    addBrowserReadyHandler(fnc) {
        this.browserReadyFunctions.push(fnc);
    }
    async sendMessage(msg) {
        if (this.websocket && this.websocket.readyState !== 0) {
            this.websocket.send(msg);
        } else {
            // console.log(`websocket is not connected`)
        }

    }
    ready() {
        for (let fnc of this.browserReadyFunctions) {
            fnc();
        }
    }
    // Context in following functions is 'websocket': beware!
    onOpen() {
        console.log(this)
        this.timesNotConnected = 0;
        this.handler.hasBeenOpen = true;
        for (let fnc of this.handler.socketOpenFunctions) {
            fnc();
        }
    }
    onClose(e) {
        console.log("ws closed", e)
        if (this.handler.hasBeenOpen) {
            this.handler.hasBeenOpen = false
            for (let fnc of this.handler.socketClosedFunctions) {
                fnc(e);
            }
        }
        else {
            if (e.code == 3001) {
                console.log('ws closed 3001');
                this.handler.websocket = null;
            } else {
                this.handler.websocket = null;
                // console.log('ws connection error retry in 150 ms');
                console.log(this.handler.timesNotConnected)
                if (this.handler.timesNotConnected > 2) {
                    import('../state/PhysicalBrowser.js').then(module => {
                        module.physicalBrowser.initConnection()
                      });
                    return;
                }
                setTimeout(() => {
                    this.handler.timesNotConnected++;
                    this.handler.init(this.handler.location);
                }, 1000);
            }
        }


    }
    onMessage(msg) {
        if (msg.data instanceof Blob) {
            for (let fnc of this.handler.binaryMessageHandlers) {
                fnc(msg);
            }
        }
        else {
            var res = msg.data.split(";");

            this.handler.asciiMessageHandlers.forEach(fnc => {
                fnc(res)
            });
        }
    }
    onError(e) {
        if (this.websocket && this.websocket.readyState == 1) {
            console.log('ws normal error: ' + e.type);
        }

    }

};

export default SocketHandler;