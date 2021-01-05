import SocketHandler from './SocketHandler.js'


export class MicrophoneHandler {
    constructor() {
        this.stream;
        this.mediaRecorder;
    }

    startStreaming(stream) {
        this.stream = stream
        let options = {
            mimeType: 'video/webm;codecs=vp8',
            // videoBitsPerSecond: 218,
        }
        let mediaRecorder = new MediaRecorder(this.stream, options);
        mediaRecorder.ondataavailable = (e) => {
            let blob = new Blob(["mic", e.data])
            this.webCamBytesSent += blob.size;
            SocketHandler.sendMessage(blob)
        }
        console.log("Sending microphone media")
        mediaRecorder.start(30); // fiddle with this maybe
    }

    stopStreaming() {
        this.mediaRecorder.stop()
        const tracks = stream.getTracks();
        tracks.forEach(function (track) {
            track.stop();
        });
    }


}

export default MicrophoneHandler;