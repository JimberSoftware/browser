import SocketHandler from '../../handlers/SocketHandler.js'

class AudioStreamer {

    constructor() {

        this.videoEl = document.createElement('video');
        this.videoEl.style.position = 'absolute';
        this.videoEl.style.width = '250';
        this.videoEl.style.height = '40';
        this.videoEl.style.left = '0';
        this.videoEl.style.top = '500';
        this.videoEl.controls = true;
        this.bytes = null;
        this.firstTime = true;
        this.buffersToBeProcessed = [];
        this.sourceBuffers = [];
        this.mediaSource = new MediaSource();
        this.videoEl.src = window.URL.createObjectURL(this.mediaSource);
        this.playing = false;
        this.sources = [];
        this.lock = false;
        this.mediaSource.addEventListener('sourceopen', () => {
            // var mimeCodec = 'audio/mp3; codecs="avc1.42E01E, mp4a.40.2"';
            this.addSourceBuffer('audio/mpeg')
            // for (var source of this.sources) {

            //     console.log('Late adding source buffer');
            //     this._addSourceBuffer(source.mimeType);

            //     for (var bufferString in source.buffer) {
            //         console.log("Appending buffered buffer")
            //         var bufferId = this.sources.indexOf(source);
            //         this.appendBuffer(bufferId, bufferString);
            //     }
            //     source.buffer = [];
            // }
            // this.sourceBuffer = this.mediaSource.addSourceBuffer('video/webm; codecs="opus,vp9"');
            // // this.sourceBuffer.mode = 'segments';
            // this.sourceBuffer.onabort = function (e) { console.log(e); };
            // this.sourceBuffer.onerror = function (e) { console.log(e); };
        });

        document.body.appendChild(this.videoEl);
    }
    appendBuffer(bufferId, buffer) {
        // console.log("jawadde")
        // console.log(this.sourceBuffers[bufferId]);
        // return;
        // if (this.mediaSource.readyState !== 'open') {
        //     console.log("Pre buffer ", bufferId);
        //     this.sourceBuffers.buffer.push(buffer);
        //     return;
        // }
        // console.log('appendbuffer_start')
        var sourceBuffer = this.sourceBuffers[bufferId];
        // sourceBuffer.buffersToBeProcessed.push(buffer);

        if (sourceBuffer.buffersToBeProcessed.length) {
            sourceBuffer.buffersToBeProcessed.push(buffer);
        } else if (sourceBuffer.updating) {
            sourceBuffer.buffersToBeProcessed.push(buffer);
        } else {
            sourceBuffer.appendBuffer(buffer);
        }
        //     console.log(this.lock);
        //     // if(this.lock === true) {
        //     //     sourceBuffer.buffersToBeProcessed.push(buffer);
        //     //     return;
        //     // }
        //     this.lock = true;
        //     console.log("sourceBuffer.appendBuffer")
        //     sourceBuffer.appendBuffer(new Uint8Array(buffer));
        //     console.log("sourceBuffer.done")
        //     this.lock = false;
        // }
        // console.log('appendbuffer_end')

    }
    addSourceBuffer(mimeType) {
        var sourceBuffer = this.mediaSource.addSourceBuffer(mimeType);

        sourceBuffer.addEventListener('updateend', () => {
            if (this.firstTime){
                this.firstTime = false;
                this.videoEl.play();
            }
            // console.log('updateend sbuffer', sourceBuffer.buffersToBeProcessed.length);
            if (sourceBuffer.buffersToBeProcessed.length) {
                // console.log("updateend, going to append")
                sourceBuffer.appendBuffer(sourceBuffer.buffersToBeProcessed.shift());
            }
        }, false);

        // console.log(this.mediaSource)
        sourceBuffer.buffersToBeProcessed = [];
        this.sourceBuffers.push(sourceBuffer);
    }

    start() {
        SocketHandler.addBinaryMessageHandler(function (msg) {
            // msg.data.text().then((text) => {
            //     console.log("ma text lukt wel")
            // })
            msg.data.arrayBuffer().then((arrayBuffer) => {
                // console.log("made it a buffer")
                let view = new Uint8Array(arrayBuffer)
				//[a,u,d,i,o]
                if(view[0] === 97 && view[1] === 117) {
                    // view.slice(5)
                    this.appendBuffer(0, view.slice(5))
                }
            });
        }.bind(this));

        // var request = new XMLHttpRequest();
        // request.open('GET', 'http://localhost:8000/examples/audio/sound.mp3', true);
        // request.responseType = 'arraybuffer';
        // request.onload = function () {
        //     console.log("load?")
        //     var audioData = request.response;
        //     this.appendBuffer(0, audioData);
        // }.bind(this);
        // request.send()
    }
};



export default AudioStreamer;