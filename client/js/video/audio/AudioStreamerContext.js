import { socketHandler } from '../../state/SocketHandler.js'

class AudioStreamerContext {

    constructor() {
        // this.bytes = null;
        this.firstTime = true;
        // this.buffersToBeProcessed = [];
        // this.sourceBuffers = [];
        this.playingAudio = false;
        // this.lock = false;
        this.audioCtx = null;
        // this.rawAudioBuffers = [];
        this.wavAudioBuffers = new Array;
        // this.audioBufferSound;
        // this._addLatency = false;
        this.nextTime = 0;
        this.bytesReceived = 0;
        this.lastSoundPacket = Date.now();

    }

    createAudioContext() {
        if (!this.audioCtx) {
            console.log("Creating audio context...");
            this.audioCtx = new (window.AudioContext || window.webkitAudioContext)({ sampleRate: 44100 });
        }
    }

    bytesToSize(bytes) {
        const sizes = ['Bytes', 'KB', 'MB', 'GB', 'TB']
        if (bytes === 0) return 'n/a'
        const i = parseInt(Math.floor(Math.log(bytes) / Math.log(1024)), 10)
        if (i === 0) return `${bytes} ${sizes[i]})`
        return `${(bytes / (1024 ** i)).toFixed(1)} ${sizes[i]}`
    }

    scheduleBuffers() {
        while (this.wavAudioBuffers.length) {
            var buffer = this.wavAudioBuffers.shift();
            var source = this.audioCtx.createBufferSource();
            source.buffer = buffer;
            source.connect(this.audioCtx.destination);
            if (this.nextTime == 0 || this.addLatency) {
                this.nextTime = this.audioCtx.currentTime + 0.05;   /// add 50ms latency to work well across systems - tune this if you like
            }
            // console.log(this.nextTime)
            source.start(this.nextTime);
            this.nextTime += source.buffer.duration; // Make the next buffer wait the length of the last buffer before being played
        };
        // source.start(0);
        this.playingAudio = false;
    }
    start() {
        socketHandler.addBinaryMessageHandler(function (msg) {
            msg.data.arrayBuffer().then((arrayBuffer) => {
                let view = new Uint8Array(arrayBuffer)
                //[a,u,d,i,o]
                if (view[0] === 97 && view[1] === 117) {
                    let bufferstuff = arrayBuffer.slice(5);
                    this.bytesReceived += bufferstuff.byteLength;

                    this.addLatency = false
                    if ((Date.now() - this.lastSoundPacket) > 250) {
                        this.addLatency = true;
                    }
                    this.lastSoundPacket = Date.now();
                    this.audioCtx.decodeAudioData(bufferstuff, (audioBuffer) => {
                        this.wavAudioBuffers.push(audioBuffer)
                        if (!this.playingAudio) {
                            this.playingAudio = true;
                            this.scheduleBuffers();
                        }

                    })
                }
            });

        }.bind(this));
    }
};



export default AudioStreamerContext;