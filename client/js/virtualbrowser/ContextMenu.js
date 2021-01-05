import { } from "./ContextMenuEvents.js"

var ContextMenuFlags = {
    CANUNDO : 0x1,
    CANREDO : 0x2,
    CANCUT : 0x4,
    CANCOPY : 0x8,
    CANPASTE : 0x10,
    CANDELETE : 0x20,
    CANSELECTALL : 0x40,
    CANTRANSLATE : 0x80,
    CANEDITRICHLY : 0x100
}


var LinkContextMenuFlags = ["CANOPENLINK", "CANCOPYLINK", "CANPRINT"]
var ImageContextMenuFlags = ["CANOPENIMAGE", "CANCOPYIMAGE", "CANCOPYIMAGELINK", "CANPRINT"]
var EditableContextMenuFlags = ["CANUNDO","CANREDO","CANCUT","CANCOPY","CANPASTE", "CANSELECTALL", "CANPRINT"]
var PageContextMenuFlags = ["CANRELOAD", "CANBACK", "CANFORWARD", "CANCOPY", "CANPRINT"]
var MediaContextMenuFlags = ["CANOPENVIDEO", "CANCOPYVIDEO", "CANPRINT"]

class ContextMenu {
    constructor(x, y, editFlags, back, forward, linkUrl, mediaUrl) {
        this.x = x;
        this.y = y;
        this.editFlags = editFlags;
        this.back = back;
        this.forward = forward;
        this.linkUrl = linkUrl;
        this.mediaUrl = mediaUrl;
        this.empty = true;
        this.resetHTMLElements();
    }

    testFlag(flag) {
        return (this.editFlags & flag) == flag && (flag != 0 || this.editFlags == flag);
    }

    resetHTMLElements() {
        var arr = LinkContextMenuFlags.concat(EditableContextMenuFlags, PageContextMenuFlags, ImageContextMenuFlags, MediaContextMenuFlags);
        arr.forEach(flag => {
            var element = document.getElementById(flag);
            this.hideElement(element);
        });
    }

    createLinkMenu() {
        this.empty = false;
        LinkContextMenuFlags.forEach(flag => {
            var element = document.getElementById(flag);
            this.showElement(element);
        })
    }

    createMediaMenu() {
        this.empty = false;
        MediaContextMenuFlags.forEach(flag => {
            var element = document.getElementById(flag);
            this.showElement(element);
        })
    }

    createImageMenu() {
        this.empty = false;
        ImageContextMenuFlags.forEach(flag => {
            var element = document.getElementById(flag);
            this.showElement(element);
        })
    }

    createEditableMenu() {
        this.empty = false;
        EditableContextMenuFlags.forEach(flag => {
            var element = document.getElementById(flag);
            this.showElement(element);
            if(ContextMenuFlags[flag]) this.setElementStatus(element, this.testFlag(ContextMenuFlags[flag]));
        })
    }

    createPageMenu() {
        this.empty = false;
        PageContextMenuFlags.forEach(flag => {
            var element = document.getElementById(flag);
            this.showElement(element);
            if(ContextMenuFlags[flag]) this.setElementStatus(element, this.testFlag(ContextMenuFlags[flag]));
            if(flag == 'CANBACK') this.setElementStatus(element, !!parseInt(this.back))
            if(flag == 'CANFORWARD') this.setElementStatus(element, !!parseInt(this.forward))
        })
    }

    showElement(element) {
        element.setAttribute( 'style', 'display:block !important');
    }

    hideElement(element) {
        element.setAttribute( 'style', 'display:none !important');
    }

    setElementStatus(element, enabled) {
        element.classList.remove("disabled-option");
        if(!enabled) {
            element.classList.add("disabled-option");
        }
    }
}

export default ContextMenu;