import { socketHandler } from "./../state/SocketHandler.js"
import { ClientToServerCommand } from '../Proto.js'
import Command from './../models/Command.js'
class UploadHandler {
    constructor() {
        this.upload = null;
    }

    requestUpload(multiple, mimeTypes) {
        this.upload = { multiple: parseInt(multiple), mimeTypes: mimeTypes, }
    }

    uploadFiles() {
        console.log("uploadFiles!")
        const files = document.getElementsByName("file")[0].files;
        let filesFinishedUploading = 0;
        const c = new Command();
        c.setContent(ClientToServerCommand.FILEUPLOADSTART, [files.length]);
        c.send();

        for (let index = 0; index < files.length; index++) {
            const file = files[index]
            const reader = new FileReader();
            reader.onload = function (e) {
                const encoder = new TextEncoder("utf-8");

                // f:1 byte, file.name.length:1 byte, filename:bytes, data:bytes
                const offset = 1 + 1 + file.name.length
                const buffer = new ArrayBuffer(offset + e.total)

                let bufferStuffer = new Uint8Array(buffer);

                bufferStuffer.set(encoder.encode('f'), 0);
                bufferStuffer.set([file.name.length], 1);
                bufferStuffer.set(encoder.encode(file.name), 2);
                bufferStuffer.set(new Uint8Array(e.target.result), offset);

                console.log("sending bufferstuffer")
                socketHandler.sendMessage(bufferStuffer)

                filesFinishedUploading++;
                if (filesFinishedUploading == files.length) {
                    // const c = new Command();
                    // c.setContent(ClientToServerCommand.FILEUPLOADFINISHED, []);
                    // console.log("sending finished")
                    // c.send();
                }
            };
            reader.readAsArrayBuffer(file);
        }

        // send finished
    }

    cancelUpload() {
        var c = new Command();
        c.setContent(ClientToServerCommand.FILEUPLOADCANCEL, []);
        c.send();
        this.upload = null
    }

    uploadIsDone() {
        this.upload = null;
    }
}

export default UploadHandler;