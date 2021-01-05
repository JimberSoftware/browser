import Extensions from './../Extensions.js'
import { socketHandler } from './../state/SocketHandler.js'
// import { Cookie, CookieJar } from './Cookie.js'
import changeCursor from './CursorChange.js'
import { Command } from '../models/Command.js'
import { ServerToClientCommand, ClientToServerCommand } from '../Proto.js'
import { physicalBrowser } from "../state/PhysicalBrowser.js"
import SystemTools from '../SystemTools.js';
import ContextMenu from './ContextMenu.js';
import ContextMenuEvents from './ContextMenuEvents.js';
import { uploadHandler } from '../state/UploadHandler.js'
import { permissionHandler } from "../state/PermissionHandler.js"

Extensions();

/* Virtual browser will provide all sorts of functions to make your physical browser behave like your virtual
one and sync states between them */

class VirtualBrowser {

    constructor() {
        socketHandler.addAsciiMessageHandler(this.commandParser.bind(this));
        socketHandler.addBrowserReadyHandler(this.onStart.bind(this))
        this.clipboardData = "";
        this.selectionData = "";
        this.contextMenu = {};
        this.currentFavicon = null;
        this.currentUrl = "";
        this.inputVisible = false;

        this.toOpenPopup = "";
        this.toOpenPopupRect = {};
        this.popupWindow = undefined;

        // this.cookieJar = new CookieJar();
        this.browserId = null;
        this.userId = null;
        this.downloadsEnabled = null;
        // this.browserSecret = null;
        this.handlers = {};
        this.handlers[ServerToClientCommand.URL] = this.onChangeUrl.bind(this);
        this.handlers[ServerToClientCommand.TITLE] = this.onChangeTitle.bind(this);
        // this.handlers[ServerToClientCommand.ADDCOOKIE] = this.onAddCookie.bind(this);
        // this.handlers[ServerToClientCommand.REMOVECOOKIE] = this.onDeleteCookie.bind(this);
        this.handlers[ServerToClientCommand.CLIPBOARD] = this.onClipboardChange.bind(this);
        this.handlers[ServerToClientCommand.SELECTION] = this.onSelectionChange.bind(this);
        this.handlers[ServerToClientCommand.CURSOR] = this.onChangeCursor.bind(this);
        this.handlers[ServerToClientCommand.OPENTAB] = this.onOpenTab.bind(this);
        this.handlers[ServerToClientCommand.ICON] = this.onChangeIcon.bind(this);
        this.handlers[ServerToClientCommand.SHOWINPUT] = this.onShowInput.bind(this);
        this.handlers[ServerToClientCommand.HIDEINPUT] = this.onHideInput.bind(this);
        this.handlers[ServerToClientCommand.SHOWCONTEXTMENU] = this.onShowContextMenu.bind(this);
        this.handlers[ServerToClientCommand.PDF] = this.onPdf.bind(this);
        this.handlers[ServerToClientCommand.ALERT] = this.onAlert.bind(this);
        this.handlers[ServerToClientCommand.REQUESTPERMISSION] = this.onRequestPermission.bind(this);
        this.handlers[ServerToClientCommand.NOTIFICATION] = this.onNotification.bind(this);
        this.handlers[ServerToClientCommand.FILEUPLOAD] = this.onFileUpload.bind(this);
        this.handlers[ServerToClientCommand.FILEUPLOADFINISHED] = this.onFileUploadFinished.bind(this);
        this.handlers[ServerToClientCommand.SHOWSELECTIONMENU] = this.onShowSelectionMenu.bind(this);
        this.handlers[ServerToClientCommand.HIDESELECTIONMENU] = this.onHideSelectionMenu.bind(this);
        this.handlers[ServerToClientCommand.SETTOUCHHANDLEBOUNDS] = this.onSetTouchHandleBounds.bind(this);
        this.handlers[ServerToClientCommand.SETTOUCHHANDLEVISIBILITY] = this.onSetTouchHandleVisibility.bind(this);
        this.handlers[ServerToClientCommand.SETTOUCHHANDLEOPACITY] = this.onSetTouchHandleOpacity.bind(this);
        this.handlers[ServerToClientCommand.POPUPOPENED] = this.onPopupOpened.bind(this);
        this.handlers[ServerToClientCommand.POPUPCLOSED] = this.onPopupClosed.bind(this);
        this.handlers[ServerToClientCommand.POPUPRESIZED] = this.onPopupResized.bind(this);
        this.handlers[ServerToClientCommand.OPENSERVICE] = this.onOpenService.bind(this);
        this.handlers[ServerToClientCommand.PAGELOADINGSTARTED] = this.onPageLoadStarted.bind(this);
        this.handlers[ServerToClientCommand.PAGELOADINGFINISHED] = this.onPageLoadFinished.bind(this);
        this.handlers[ServerToClientCommand.WINDOWOPENED] = this.onWindowOpened.bind(this);
        this.handlers[ServerToClientCommand.PONG] = this.onPong.bind(this);
    }
    onWindowOpened(windowId) {
        window.open(`//${window.location.host}/?windowId=${windowId}`, '_blank');
    }
    onPageLoadStarted() {
        document.getElementById("page-load-spinner").style.visibility = "visible"
    }
    onPageLoadFinished() {
        document.getElementById("page-load-spinner").style.visibility = "hidden"
    }
    onPopupClosed() {
        if (this.popupWindow) { this.popupWindow.close() };
        this.toOpenPopup = "";
        this.popupWindow = undefined;
    }

    onPopupResized(width, height) {
        physicalBrowser.resizedByBackend = true;
        window.resizeTo(width, height);
    }

    onPopupOpened(winId, x, y, width, height) {
        this.toOpenPopup = winId;
        this.toOpenPopupRect = { x, y, width, height };

        let popup = window.open(`//${window.location.host}/?windowId=${winId}`, "", `width=${width},height=${height},left=${x},top${y}`);
        popup.onbeforeunload = () => { this.unload(winId); };
        // window.open("https://google.com", "", "width=500,height=500,top=300,left=3000");
    }

    onSetTouchHandleVisibility(orientation, visible) {
        var element;
        if (orientation == 0) {
            element = document.getElementById("leftselectionhandle");
        }
        if (orientation == 1) {
            element = document.getElementById("centerselectionhandle");
        }
        if (orientation == 2) {
            element = document.getElementById("rightselectionhandle");
        }
        if (element) element.style.display = (visible == 1) ? 'block' : 'none';
    }

    onSetTouchHandleOpacity(orientation, opacity) {
        //maybe later?
    }

    onShowSelectionMenu(cut, copy, paste, selectionX, selectionY, selectionWidth, selectionHeight) {
        cut = parseInt(cut);
        copy = parseInt(copy);
        paste = parseInt(paste);

        var buttonCount = cut + copy + paste;
        if (buttonCount == 0) return;

        selectionX = parseInt(selectionX);
        selectionY = parseInt(selectionY);
        selectionWidth = parseInt(selectionWidth);
        selectionHeight = parseInt(selectionHeight);

        var cutEl = document.getElementById("MOBILECANCUT");
        var copyEl = document.getElementById("MOBILECANCOPY");
        var pasteEl = document.getElementById("MOBILECANPASTE");
        cutEl.style.display = cut == 1 ? 'flex' : 'none';
        copyEl.style.display = copy == 1 ? 'flex' : 'none';
        pasteEl.style.display = paste == 1 ? 'flex' : 'none';

        var spacingBetweenButtons = 2;

        var minButtonWidth = 63;
        var minButtonHeight = 38;

        var width = (spacingBetweenButtons * (buttonCount + 1)) + (minButtonWidth * buttonCount);
        var height = minButtonHeight + spacingBetweenButtons;
        var x = ((selectionX * 2) + selectionWidth - width) / 2;
        var y = selectionY - height - 2;


        var menu = document.getElementById("touchselectioncontextmenu");
        menu.setAttribute('style', 'display:block !important');
        menu.style.width = `${width}px`;
        menu.style.height = `${height}px`;

        if (y < 0) {
            y = y + (height * 2.5);
        }
        if (x < 0) {
            x = 0;
        }

        menu.style.left = `${x}px`;
        menu.style.top = `${y}px`;
    }

    onHideSelectionMenu() {
        ContextMenuEvents.hideTouchSelectionContextMenu();
    }

    onSetTouchHandleBounds(orientation, x, y, width, height) {
        var element;
        if (orientation == 0) {
            element = document.getElementById("leftselectionhandle");
        }
        if (orientation == 1) {
            element = document.getElementById("centerselectionhandle");
        }
        if (orientation == 2) {
            element = document.getElementById("rightselectionhandle");
        }

        element.style.left = `${x}px`;
        element.style.top = `${y}px`;
        element.style.width = `${width}px`;
        element.style.height = `${height}px`;
    }

    onNotification(title, origin, body, iconBase64) {
        var icon = new Image();
        let src = 'data:image/png;base64,' + iconBase64;
        if (origin.startsWith('http')) {
            origin = origin.split('://')[1]
        }

        body = origin + "\n\n" + body
        var options = {
            body: body,
            icon: src,
        }
        new Notification(title, options)

    }

    onFileUpload(id) {
        upload
    }

    onRequestPermission(url, permission) {
        permissionHandler.onPermissionRequested(url, permission);
    }

    // onAddCookie(cookiestr) {
    //     let cookie = Cookie.fromCookieString(atob(cookiestr));
    //     this.cookieJar.addCookie(cookie);
    // }
    /* events */
    onChangeCursor(cursor) {
        changeCursor(cursor);
    }
    onChangeUrl(url) {
        if (this.currentUrl === url) return;

        this.currentUrl = url;
        if (!_aliasBrowsing) {
            history.pushState({}, undefined, "#" + url);
        }
    }
    onChangeTitle(title) {
        window.document.title = title;
    }
    onChangeIcon(icon) {
        // this.currentFavicon = icon;
        var head = document.head || document.getElementsByTagName('head')[0];
        var link = document.createElement('link'),
            oldLink = document.getElementById('dynamic-favicon');
        link.id = 'dynamic-favicon';
        link.rel = 'shortcut icon';
        link.href = 'data:image/png;base64,' + icon;
        if (oldLink) {
            head.removeChild(oldLink);
        }
        head.appendChild(link);
    }

    onPdf(pdfb64) {
        var linkSource = `data:application/pdf;base64,${pdfb64}`;
        var downloadLink = document.createElement("a");
        var fileName = "page.pdf";

        downloadLink.href = linkSource;
        downloadLink.download = fileName;
        downloadLink.click();
    }

    onOpenTab(url) {
        // window.open(`http://${window.location.host}/#${url}`, '_blank')
    }
    onConfirmDialog(text) {

    }
    // onDeleteCookie(cookiestr) {
    //     let cookie = Cookie.fromCookieString(atob(cookiestr));
    //     this.cookieJar.delCookie(cookie);
    // }
    onClipboardChange(type, data) {
        this.clipboardData = atob(data)
        if (type === "text") {
            navigator.clipboard.writeText(this.clipboardData)
            return
        }
        if (type === "image") {
            const blob = this.b64toBlob(data, "image/png")
            navigator.clipboard.write([
                new ClipboardItem({
                    [blob.type]: blob
                })
            ]);
            return
        }

    }
    onSelectionChange(data) {
        this.selectionData = atob(data)
    }
    onStart() {
        // let cookies = this.cookieJar.all();

        // var c = new Command();
        // c.setContent(ClientToServerCommand.ADDCOOKIES, [btoa(JSON.stringify(cookies))]);
        // c.send();
    }
    onShowInput() {
        this.inputVisible = true;
        physicalBrowser.fakeMobileInput.focus();
    }
    onHideInput() {
        this.inputVisible = false;
        physicalBrowser.fakeMobileInput.blur();
    }
    onShowContextMenu(x, y, editFlags, editable, back, forward, linkUrl, mediaUrl, mediaType) {
        //Set text in clipboard in backend:
        physicalBrowser.sendClipboard();
        //End

        x = parseInt(x);
        y = parseInt(y);

        this.contextMenu = new ContextMenu(x, y, editFlags, back, forward, linkUrl, mediaUrl, mediaType);

        if (linkUrl) {
            this.contextMenu.createLinkMenu();
        }
        if (mediaUrl) {
            if (mediaType == 1) {
                this.contextMenu.createImageMenu();
            } else if (mediaType == 2) {
                this.contextMenu.createMediaMenu();
            }
        }

        // if (!physicalBrowser.isMobile) {
        if (editable != 0) {
            this.contextMenu.createEditableMenu();
        }

        if (this.contextMenu.empty) {
            this.contextMenu.createPageMenu();
        }
        // }

        if (!this.contextMenu.empty) {
            var menu = document.getElementById("contextmenu");
            menu.setAttribute('style', 'display:block !important');

            let container = document.getElementById('video-container');
            let containerWidth = window.isSafari() ? container.offsetWidth / window.devicePixelRatio : container.offsetWidth;
            let containerHeight = window.isSafari() ? container.offsetHeight / window.devicePixelRatio : container.offsetHeight;
            let menuWidth = window.isSafari() ? menu.offsetWidth / window.devicePixelRatio : menu.offsetWidth;
            let menuHeight = window.isSafari() ? menu.offsetHeight / window.devicePixelRatio : menu.offsetHeight;

            let overflowX = (x + menuWidth) - containerWidth;
            let overflowY = (y + menuHeight) - containerHeight;

            if (overflowX > 0) x = x - overflowX;
            if (overflowY > 0) y = y - overflowY;
            menu.style.left = `${x}px`;
            menu.style.top = `${y}px`;
        }
    }

    onAlert(msg) {
        alert(atob(msg));
    }

    onFileUpload(multiple, mimeTypes) {
        uploadHandler.requestUpload(multiple, mimeTypes)
    }
    onFileUploadFinished() {
        uploadHandler.uploadIsDone();
    }
    onOpenService(url) {
        window.open(url, '_target')
    }

    onPong() {
        return;
    }


    /* Actions */
    changeQuality(quality) {
        var c = new Command();
        c.setContent(ClientToServerCommand.QUALITY, [quality]);
        c.send();
    }
    browse(url){
        var c = new Command();
        c.setContent(ClientToServerCommand.CHANGEURL, [url]);
        c.send();
    }
    reload() {
        var c = new Command();
        c.setContent(ClientToServerCommand.RELOAD, []);
        c.send();
    }
    backward() {
        var c = new Command();
        c.setContent(ClientToServerCommand.BACK, []);
        c.send();
    }
    forward() {
        var c = new Command();
        c.setContent(ClientToServerCommand.FORWARD, []);
        c.send();
    }
    // end

    getClipboardData() {
        return this.clipboardData;
    }

    b64toBlob(b64Data, contentType, sliceSize) {
        contentType = contentType || '';
        sliceSize = sliceSize || 512;

        var byteCharacters = atob(b64Data);
        var byteArrays = [];

        for (var offset = 0; offset < byteCharacters.length; offset += sliceSize) {
            var slice = byteCharacters.slice(offset, offset + sliceSize);
            var byteNumbers = new Array(slice.length);
            for (var i = 0; i < slice.length; i++) {
                byteNumbers[i] = slice.charCodeAt(i);
            }
            var byteArray = new Uint8Array(byteNumbers);
            byteArrays.push(byteArray);
        }

        var blob = new Blob(byteArrays, { type: contentType });
        return blob;
    }
    /* Command parser */
    commandParser(cmd) {
        let type = cmd[0];
        if (this.handlers[type] === undefined) {
            console.warn("No handler for this command ", type);
            return;
        }
        this.handlers[type](...cmd.slice(1));

    }
}

export default VirtualBrowser;

