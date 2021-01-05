import {socketHandler} from '../state/SocketHandler.js';
import {physicalBrowser} from '../state/PhysicalBrowser.js';


"use strict";

export class Command {
    constructor() {
        this.type = "";
        this.values = "";
    }

    send() {
        socketHandler.sendMessage(this.toString());
    }
    
    setContent(type, values) {
        this.type = type;
        this.values = values;
    }
    toString() {
        let temp = `${this.type}`;
        this.values.forEach(element => {
            temp += ';' + element;
        });
        return temp;
    }
};

// Command.prototype.toString = function () {
//     let temp = this._command.toString();
//     this._values.forEach(element => {
//         temp += ';' + element;
//     });
//     return temp;
// }

// export var CommandType = {
//     CHANGEURL: 0x01,
//     CLIPBOARD: 0x02,
//     ADDCOOKIE: 0x03,
//     TOUCHEVENT: 0x04,
//     SHOWINPUT: 0x05,
//     HIDEINPUT: 0x06,
//     SETCRF: 0x08,
//     PRINT: 0x09,
//     START: 0x10,
//     ADDCOOKIES: 0x11,
//     KEYBOARDEVENT: 0x6b,
//     PASTEEVENT: 0x70,
//     MOUSEEVENT: 0x6d,
//     RESIZEEVENT: 0x03,
//     MOUSEWHEELEVENT: 0x77,
//     CURSOR: 0x63,
//     FOCUSEVENT: 0x66,
// }

export default Command;



//Command.prototype.toet = "test";
