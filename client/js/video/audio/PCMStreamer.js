import SocketHandler from '../../handlers/SocketHandler.js'
import PCMPlayer from './pcm-player.js'
class PCMStreamer {

    constructor() {
        this.player = new PCMPlayer({
            encoding: '16bitInt',
            channels: 1,
            sampleRate: 44100,
            flushingTime: 3000
        });
    }
   
    start() {
        SocketHandler.addBinaryMessageHandler(function (msg) {
            // msg.data.text().then((text) => {
            //     console.log("ma text lukt wel")
            // })
            msg.data.arrayBuffer().then((arrayBuffer) => {
                let view = new Uint8Array(arrayBuffer)
                //[a,u,d,i,o]
                if (view[0] === 97 && view[1] === 117) {
                    let bufferstuff = arrayBuffer.slice(5);
                    // console.log("feeding")
                    let array = new Uint8Array(bufferstuff);

                    this.player.feed(array)
                    // } catch (error) {
                    //     console.log(error)
                    // }
                }
            });
        }.bind(this));

    }
};



export default PCMStreamer;