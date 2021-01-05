import Config from '../config.js'
import Command from './models/Command.js'
import { socketHandler } from './state/SocketHandler.js';
import Touch from './models/Touch.js'
import MouseEvent from './MouseEvent.js';
import SystemTools from './SystemTools.js';
import { virtualBrowser } from './state/VirtualBrowser.js';
import { ClientToServerCommand } from './Proto.js'
import { audioStreamerContext } from './state/AudioStreamer.js';
import { initUser } from './user.js';

class PhysicalBrowser {

    constructor() {
        window.ph = this;
        socketHandler.addBrowserReadyHandler(this.setDevicePixelRatio.bind(this))
        socketHandler.addBrowserReadyHandler(this.onStart.bind(this))
        socketHandler.addBrowserReadyHandler(this.onResize.bind(this))
        socketHandler.addBrowserReadyHandler(this.startPing.bind(this))


        // socketHandler.addSocketOpenHandler(this.onHashChange) //trick to make remote browse to initial
        // socketHandler.addSocketClosedHandler(this.onHashChange)
        // socketHandler.addSocketOpenHandler(this.removeWindowIdParam.bind(this))
        window.addEventListener('resize', this.onResize.bind(this))
        window.addEventListener('hashchange', this.onHashChange.bind(this));
        window.addEventListener("contextmenu", this.onContextMenu.bind(this), false);
        window.addEventListener('save', this.onPrintPage, false);
        window.addEventListener("beforeunload", this.beforeunload.bind(this), false);
        window.print = this.onPrintPage;
        document.addEventListener('copy', this.onCopy);
        document.addEventListener('paste', this.onPaste);
        window.addEventListener("mouseover", function (e) {
            audioStreamerContext.createAudioContext();
        }.bind(this), { once: true })
        this.isMobile = isMobileOrTablet() ? 1 : 0;
        // this.windowId = this.getParameterByName("windowId") != undefined ? this.getParameterByName("windowId") : 1;
        this.browsersecret = this.getParameterByName("s");
        this.browserId = this.getParameterByName("browserId");
        this.downloadsEnabled = this.getParameterByName("downloadsEnabled");
        this.fakeMobileInput = document.getElementById("fakeInputForMobile")
        this.fakeMobileInput.oninput = this.onMobileInput.bind(this)
        this.fakeMobileInput.onblur = this.onInputBlur.bind(this)

        this.keysToCancel = ['Tab'];
        this.resizeTimeout;
        this.lastTouch;
        this.isMobileTyping = false;
        this.isTouching = false;
        this.isMiddleMouseClicking = false;
        this.lastMiddleMouseClick = { keep: true }
        this.moveInterval = undefined;
        this.resizedByBackend = false;
        this.currentRatio = window.devicePixelRatio;

        this.changedRatioStr = `(resolution: ${window.devicePixelRatio}dppx)`;
        this.currentMediaQueryList = window.matchMedia(this.changedRatioStr);
        this.updateDeviceFunc = this.updateDevicePixelRatio.bind(this);
        this.currentMediaQueryList.addListener(this.updateDeviceFunc);

    }

    beforeunload() {
        var c = new Command();
        c.setContent(ClientToServerCommand.WINDOWCLOSED, [this.windowId]);
        c.send();
    }

    // unload(windowId) {
    //     if (!windowId) {
    //         windowId = this.windowId
    //     }
    //     var c = new Command();
    //     c.setContent(ClientToServerCommand.POPUPCLOSED, [windowId]);
    //     c.send();
    // }

    getParameterByName(name) {
        let params = new URLSearchParams(location.search);
        return params.get(name);
    }

    updateDevicePixelRatio() {
        this.currentRatio = window.devicePixelRatio;

        this.currentMediaQueryList.removeListener(this.updateDeviceFunc);
        this.changedRatioStr = `(resolution: ${window.devicePixelRatio}dppx)`;
        this.currentMediaQueryList = window.matchMedia(this.changedRatioStr);
        this.currentMediaQueryList.addListener(this.updateDeviceFunc);

        this.setDevicePixelRatio();
    }

    addEventListeners() {
        var videoContainer = document.getElementById("video-container")
        videoContainer.addEventListener('mousedown', this.onMouseEvent.bind(this));
        videoContainer.addEventListener('mousemove', this.onMouseEvent.bind(this));
        videoContainer.addEventListener('mouseup', this.onMouseEvent.bind(this));
        videoContainer.addEventListener('wheel', this.onWheelEvent.bind(this));

        videoContainer.addEventListener('touchstart', this.onTouch.bind(this));
        videoContainer.addEventListener('touchend', this.onTouch.bind(this));
        videoContainer.addEventListener('touchcancel', this.onTouch.bind(this));
        videoContainer.addEventListener('touchmove', this.onTouch.bind(this));

        const inputField = document.getElementById("fakeInputForMobile");

        inputField.addEventListener('keydown', this.onKeyDown.bind(this));
        inputField.addEventListener('keyup', this.onKeyUp.bind(this));
    }

    onContextMenu(e) {
        if(e.target.localName == "input") {
            return;
        }
        e.preventDefault();
    }

    /* Events */
    onHashChange() {
        // if(window.location)
        var url = window.location.hash.substr(1);
        if (url === "") {
            url = "http://www.google.be";
        }
        virtualBrowser.currentUrl = url;
        var sendBrowse = function (url) {
            var c = new Command();
            c.setContent(ClientToServerCommand.CHANGEURL, [url]);
            c.send();
        }
        if (url.charAt(0) == "$") {
            browseToAlias(url.substring(1), sendBrowse);
        } else {
            _aliasBrowsing = false;
            sendBrowse(url);
        }

    }

    onResize(e) {
        if (!this.resizedByBackend) {
            let container = document.getElementById('video-container');
            if (container == null) return;  // Race condition with Vue init
            let width = window.isSafari() ? container.offsetWidth / window.devicePixelRatio : container.offsetWidth;
            let height = window.isSafari() ? container.offsetHeight / window.devicePixelRatio : container.offsetHeight;
            clearTimeout(this.resizeTimeout);

            var c = new Command();
            c.setContent(ClientToServerCommand.RESIZE, [width, height]);
            c.send();
        }
        this.resizedByBackend = false;
    }

    onStart() {
        var c = new Command();
        c.setContent(ClientToServerCommand.INIT, [this.isMobile]);
        c.send();

        // if (this.isMobile) {
        //     virtualBrowser.changeQuality(1)
        // }
    }

    onKeyDown(evt) {
        if (evt.ctrlKey || evt.metaKey) {
            this.sendClipboard();
        }
        if (this.keysToCancel.includes(evt.key)) {
            evt.preventDefault()
        }

        // dirty fix for macos clipboard
        if (evt.metaKey && navigator.platform.toUpperCase().indexOf('MAC') >= 0) {
            var c = new Command();
            let code = evt.code;
            let text = evt.key;
            c.setContent(ClientToServerCommand.KEYBOARDEVENT, [code, text, '2', 0x04000000]);
            c.send();
            return;
        }

        // if (_searching)
        //     return;
        evt = evt || window.event;
        let text = evt.key;
        let code = evt.code;

        if (this.isMobile) {
            if (text != "Enter") {
                return;
            }
            code = "Enter";
        }
        if (text === ";") {
            text = "semicolonneke"
        }


        // var c = new Command();
        // c.setContent(ClientToServerCommand.KEYBOARDEVENT, ["KeyV", text, '2', 0x04000000]);
        // c.send();
        // console.log(c)


        var c = new Command();
        c.setContent(ClientToServerCommand.KEYBOARDEVENT, [code, text, '2', SystemTools.translateModifiers(evt)]);
        c.send();

        if (evt.keyCode == 70 && evt.ctrlKey) { // f and control
            cancelEvent(evt);

            var el = document.getElementById("search");
            if (el.style.display == 'block') {
                // el.style.display = 'none';
                _searching = false
            } else {
                el.style.display = 'block';
                _searching = true;
            }
            document.getElementById("search").select().focus();

            return false;
        }
        if (evt.which == 27) {
            _searching = false
            document.getElementById("search").style.display = 'none';
        }

        // CTRL + s  for saving a page  //TODO this should just print page
        if (evt.keyCode == 83 && evt.ctrlKey) {
            // cancelEvent(evt);
            savePage();
        }

        // CTRL + s  for printing //TODO this should just print page
        if (evt.keyCode == 80 && evt.ctrlKey) {
            // cancelEvent(evt);
            printPage();
        }
    }

    async sendClipboard() {
        // We need to do this otherwise the 'paste' command arrives too soon and the clipboard content in the docker is not updated
        navigator.permissions.query({ name: "clipboard-read" }).then(result => {
            if (result.state !== "granted" && result.state !== "prompt") return

            var c = new Command();
            navigator.clipboard.read().then(clipboardItem => {
                for (let i = 0; i < clipboardItem.length; i++) {
                    if (clipboardItem[i].types[0] === "text/plain") {
                        clipboardItem[i].getType("text/plain").then((blob) => {
                            blob.text().then(text => {
                                var cm = new Command();
                                cm.setContent(ClientToServerCommand.PASTEEVENT, ["text", btoa(text)]);
                                cm.send();
                            })
                        })
                    } else if (clipboardItem[i].types[0] === "image/png") {
                        clipboardItem[i].getType("image/png").then((blob) => {
                            var fr = new FileReader;
                            fr.onload = function (a) {
                                var cm = new Command();
                                cm.setContent(ClientToServerCommand.PASTEEVENT, ["image", a.target.result.split(",")[1]]);
                                cm.send();
                                // console.log("sent")
                            }.bind(this);
                            fr.readAsDataURL(blob)
                        })
                    }
                }
            })
        })
    }

    onKeyUp(evt) {
        let text = evt.key;
        let code = evt.code;
        var c = new Command();
        c.setContent(ClientToServerCommand.KEYBOARDEVENT, [code, text, '1', SystemTools.translateModifiers(evt)]);
        c.send();
    }

    onSavePage() {

    }

    onSearch() {

    }

    onCopy(e) {
        e.clipboardData.setData('text/plain', virtualBrowser.selectionData);
        e.preventDefault();
    }

    onPaste(ev) {
        for (var items = ev.clipboardData.items || [], i = 0; i < items.length; i++) {
            let item = items[i]
            // We have `sendClipBoard` function, But if a user denies access to their clipboard, at least we can provide some functionality
            if (item.type === "text/plain") {
                item.getAsString((result) => {
                    var cm = new Command();
                    cm.setContent(ClientToServerCommand.PASTEEVENT, ["text", btoa(result)]);
                    cm.send();

                })
            } else if (item.type === "image/png") {
                const f = item.getAsFile();
                if (f) {
                    var g = new FileReader;
                    g.onload = function (a) {
                        // console.log(a.target.result)
                        var cm = new Command();
                        cm.setContent(ClientToServerCommand.PASTEEVENT, ["image", a.target.result.split(",")[1]]);
                        cm.send();
                        // console.log("sent")
                    }.bind(this);
                    g.readAsDataURL(f)
                }
            }
        }
        // var cData = e.clipboardData.getData('text/plain')
        // e.clipboardData.getData
        // var c = new Command();
        // c.setContent(ClientToServerCommand.PASTEEVENT, [cData]);
        // c.send();
    }

    onPrintPage(e) {
        e.cancelEvent()
        var c = new Command();
        c.setContent(ClientToServerCommand.PRINT, []);
        c.send();

    }

    openPopup() {
        // console.log("openPopup ")
        return;
        setTimeout(() => {
            if (virtualBrowser.toOpenPopup.length > 0) {
                let winId = virtualBrowser.toOpenPopup;
                // let popup = window.open(`?windowId=${winId}&s=${virtualBrowser.browserSecret}&browserId=${virtualBrowser.browserId}&downloadsEnabled=${virtualBrowser.downloadsEnabled}`, "mywindow", `menubar=0,resizable=0,width=${virtualBrowser.toOpenPopupRect.width},height=${virtualBrowser.toOpenPopupRect.height},top=${virtualBrowser.toOpenPopupRect.y},left=${virtualBrowser.toOpenPopupRect.x}`);
                let popup = window.open(`?windowId=${winId}`, "mywindow", `menubar=0,resizable=0,width=${virtualBrowser.toOpenPopupRect.width},height=${virtualBrowser.toOpenPopupRect.height},top=${virtualBrowser.toOpenPopupRect.y},left=${virtualBrowser.toOpenPopupRect.x}`);
                // popup.onbeforeunload = () => { this.unload(winId); };
                virtualBrowser.popupWindow = popup;
                virtualBrowser.toOpenPopup = "";
            }
        }, 250)
    }

    onMouseEvent(event) {
        if (this.isMobile) {
            return;
        }

        this.handleMiddleMouseClick(event);

        if (event.type == 'mousedown') {
            this.openPopup();
        }

        if (this.isMiddleMouseClicking || event.button == 1) return;

        var c = new Command();
        c.setContent(ClientToServerCommand.MOUSEEVENT, [btoa(JSON.stringify(new MouseEvent(event)))]);
        c.send();
    }

    handleMiddleMouseClick(event) {
        if (event.type == 'mousedown') {
            clearInterval(this.moveInterval);
            if (event.button === 1) { //middle mouse click
                event.preventDefault();
                if (this.isMiddleMouseClicking) {
                    document.body.style.cursor = "default";
                    this.isMiddleMouseClicking = false;
                } else {
                    document.body.style.cursor = "all-scroll";
                    this.isMiddleMouseClicking = true;
                    this.lastMiddleMouseClick = {
                        x: event.clientX,
                        y: event.clientY,
                        keep: true
                    }
                }
            }
        }

        if (event.type == 'mouseup') {
            if (event.button === 1) { //middle mouse click
                event.preventDefault();
                clearInterval(this.moveInterval);
                if (!this.lastMiddleMouseClick.keep) {
                    document.body.style.cursor = "default";
                    this.isMiddleMouseClicking = false;
                    this.lastMiddleMouseClick.keep = true;
                }
            }
        }

        if (event.type == 'mousemove') {
            clearInterval(this.moveInterval);
            if (this.isMiddleMouseClicking) {
                if (event.movementX != 0 || event.movementY != 0) {
                    var moveFromOriginX = this.lastMiddleMouseClick.x - event.clientX;
                    var moveFromOriginY = this.lastMiddleMouseClick.y - event.clientY;
                    if (moveFromOriginX < 0 && moveFromOriginY < 0 || moveFromOriginX > 0 && moveFromOriginY > 0) document.body.style.cursor = "se-resize";
                    if (moveFromOriginX < 0 && moveFromOriginY > 0 || moveFromOriginX > 0 && moveFromOriginY < 0) document.body.style.cursor = "sw-resize";
                    if (moveFromOriginX < 25 && moveFromOriginX > -25 && moveFromOriginY > -25 && moveFromOriginY < 25) {
                        document.body.style.cursor = "all-scroll";
                        return;
                    }
                    this.moveInterval = setInterval(() => {
                        var c = new Command();
                        c.setContent(ClientToServerCommand.MOUSEWHEELEVENT, [0, 0, 0, 0, -moveFromOriginX / 7, -moveFromOriginY / 7, 0]);
                        c.send();
                    }, 1);
                    this.lastMiddleMouseClick.keep = false;
                }
            }
        }
    }

    onWheelEvent(event) {
        event.preventDefault();

        var c = new Command();
        var isFirefox = navigator.userAgent.toLowerCase().indexOf('firefox') > -1;
        var deltaX = event.deltaX;
        var deltaY = event.deltaY;

        if (isFirefox) {
            deltaX = deltaX * 15;
            deltaY = deltaY * 15;
        }

        c.setContent(ClientToServerCommand.MOUSEWHEELEVENT, [event.layerX, event.layerY, event.clientX, event.clientY, deltaX, deltaY, SystemTools.translateModifiers(event)]);
        c.send();
    }

    onTouch(event) {
        var object = {};
        object["type"] = "touch";
        object["name"] = name;
        object["time"] = new Date().getTime();
        object["event"] = event.type;
        object["changedTouches"] = [];
        object["stationaryTouches"] = [];

        this.openPopup();

        for (var i = 0; i < event.changedTouches.length; ++i) {
            const targetTouch = event.changedTouches[i];
            const offsetTop = targetTouch.target.getBoundingClientRect().top
            const touch = new Touch(targetTouch);
            touch.clientY = targetTouch.clientY - offsetTop
            touch.pageY = targetTouch.pageY - offsetTop
            object.changedTouches.push(touch);
        }

        for (var i = 0; i < event.targetTouches.length; ++i) {
            var targetTouch = event.targetTouches[i];
            if (object.changedTouches.findIndex(function (touch) {
                // cancelEvent(event);
                return touch.identifier === targetTouch.identifier;
            }) === -1) {
                object.stationaryTouches.push(touch);
            }
        }
        var c = new Command();
        c.setContent(ClientToServerCommand.TOUCHEVENT, [btoa(JSON.stringify(object))]);
        c.send();

        // cancelEvent(event);
    }

    onInputBlur(event) {
        if (event.relatedTarget) {
            event.relatedTarget.onblur = () => {
                this.fakeMobileInput.focus(); // hehehe
            }
            return;
        }
        if (virtualBrowser.inputVisible) {
            event.stopImmediatePropagation();
            event.preventDefault();
            this.fakeMobileInput.focus();
        }
    }


    onMobileInput(event) {
        if (!this.isMobile) return;

        this.isMobileTyping = true;

        var x = this.fakeMobileInput.value;
        x = x.replace("^", "");

        var charAdded = x.charAt(x.length - 1);

        if (event.inputType == "deleteContentBackward") {
            charAdded = "Backspace";

            // this.onKeyDown({ 'which': 8, 'key': charAdded });

            // this.onKeyUp({ 'which': 8, 'key': charAdded });

            this.fakeMobileInput.value = "^";
            var c = new Command();
            c.setContent(ClientToServerCommand.KEYBOARDEVENT, ["Backspace", "Backspace", '0', SystemTools.translateModifiers(event)]);
            console.log(c)
            c.send();
            setTimeout(() => {
                this.fakeMobileInput.setSelectionRange(this.fakeMobileInput.value.length, this.fakeMobileInput.value.length);
            }, 50);   //put cursor at the end of fakeinput
            return;
        }

        var obj = {
            "altKey": false,
            "charCode": 0,
            "code": "",
            "ctrlKey": false,
            "key": charAdded,
            "keyCode": 97,
            "location": 0,
            "metaKey": false,
            "repeat": false,
            "shiftKey": false,
            "string": charAdded,
            "time": 1528881386861,
            "type": "keydown",
            "which": charAdded.charCodeAt(0)
        }
        // window.onkeypress(obj, true);
        // this.onKeyPress(obj, true)
        var c = new Command();
        c.setContent(ClientToServerCommand.KEYBOARDEVENT, [charAdded.charCodeAt(0), charAdded, '0', SystemTools.translateModifiers(event)]);
        c.send();

        this.fakeMobileInput.value = "^";
        this.fakeMobileInput.focus();
        this.fakeMobileInput.setSelectionRange(this.fakeMobileInput.value.length, this.fakeMobileInput.value.length);

    }

    /* Actions */
    // removeWindowIdParam() {
    //     if (this.getParameterByName("windowId")) {
    //         window.history.pushState({}, "", "/")
    //     } else {
    //         this.onHashChange()//trick to make remote browse to initial
    //     }
    // }

    startPing() {
        if (this.pingInterval !== undefined) {
            console.log(this.pingInterval)
            console.log("its not null??")
            return;
        }
        this.pingInterval = setInterval(function () {
            var c = new Command();
            c.setContent(ClientToServerCommand.PING, []);
            c.send();
        }, 50000)
    }

    setDevicePixelRatio() {
        return
        // if(this.isMobile) return;
        // var container = document.getElementById("video-container");
        // container.style.zoom = 1/this.currentRatio;
        // let c = new Command();
        // c.setContent(ClientToServerCommand.DEVICEPIXELRATIO, [window.devicePixelRatio]);
        // c.send();
    }

    initConnection() {

        if (Config.ENVIRONMENT === 'dev') {
           // let wsUrl = `${Config.BROKER_HOST_WS}?winId=${this.windowId}`;
            let wsUrl = `ws://${window.location.hostname}:9001`;
            // socketHandler.init(wsUrl)
            // socketHandler.init(`ws://localhost:9001?winId=${this.windowId}`);

            let windowId = this.getParameterByName("windowId")
            if (windowId) {
                wsUrl += `?winId=${windowId}`;
                window.history.pushState({}, "", "/")
            } else {
                socketHandler.addBrowserReadyHandler(this.onHashChange) // trick to make backend browse
            }

            socketHandler.init(wsUrl);
            virtualBrowser.userId = "dummy";
            virtualBrowser.downloadsEnabled = true;

        } else {
            // if (this.browsersecret) {
            //     let wsUrl = `${Config.BROKER_HOST_WS}/${this.userId}/browser?winId=${this.windowId}`;
            //     socketHandler.init(wsUrl)
            //     virtualBrowser.browserId = this.browserId;
            //     virtualBrowser.downloadsEnabled = this.downloadsEnabled;
            // } else {
            this.getContainer().then(container => {
                let wsUrl = `${Config.BROKER_HOST_WS}/${container.userId}/browser`;
                let windowId = this.getParameterByName("windowId")
                if (windowId) {
                    wsUrl += `?winId=${windowId}`
                    window.history.pushState({}, "", "/")
                } else {
                    socketHandler.addBrowserReadyHandler(this.onHashChange) // trick to make backend browse
                }
                socketHandler.init(wsUrl)
                // virtualBrowser.browserSecret = container.secret;
                virtualBrowser.downloadsEnabled = container.downloadsEnabled;
                virtualBrowser.userId = container.userId
            });
            // }
        }
    }


    getContainer() {
        return new Promise((resolve, reject) => {
            const userId = initUser();
            const data = { userId }
            var xhr = new XMLHttpRequest();
            xhr.open('POST', `//${Config.BROKER_HOST}/container`, true);
            xhr.setRequestHeader('Content-Type', 'application/json');
            xhr.onreadystatechange = () => {
                if (xhr.readyState == 4) {
                    console.log(xhr.response)
                    resolve(JSON.parse(xhr.response))
                }
            };

            xhr.send(JSON.stringify(data));
        })
    }


}

export default PhysicalBrowser;
