import {socketHandler} from '../state/SocketHandler.js';


export class CameraHandler {
    constructor() {
        this.stream;
        this.mediaRecorder;

        // Otherwise the backend does not know it has a webcam 
        // Todo ...
        // window.socketHandler.addSocketOpenHandler(this.hack.bind(this))

    }
    hack() {
        try {
            navigator.mediaDevices.getUserMedia({ video: true }).then((stream) => {
                console.log("video stream initiated")
                this.startStreaming(stream)
            });

        } catch (error) {

        }
    }

    startStreaming(stream) {
        this.stream = stream
        let options = {
            mimeType: 'video/webm;codecs=vp8',
            // videoBitsPerSecond: 218,
        }
        let mediaRecorder = new MediaRecorder(this.stream, options);
        mediaRecorder.ondataavailable = (e) => {
            let blob = new Blob(["cam", e.data])
            this.webCamBytesSent += blob.size;
            socketHandler.sendMessage(blob)
        }
        console.log("Sending camera media")
        mediaRecorder.start(60);
    }

    stopStreaming() {
        this.mediaRecorder.stop()
        const tracks = stream.getTracks();
        tracks.forEach(function (track) {
            track.stop();
        });
    }


}

export default CameraHandler;