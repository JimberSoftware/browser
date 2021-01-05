import Extensions from './Extensions.js'
import SocketHandler from './handlers/SocketHandler.js';
import Config from '../config.js'

import AudioStreamerContext from './video/audio/AudioStreamerContext.js'
import { physicalBrowser } from './state/PhysicalBrowser.js'
import { virtualBrowser } from './state/VirtualBrowser.js';
import { audioStreamerContext}  from './state/AudioStreamer.js';
import store from './store/store.js'
import { socketHandler } from './state/SocketHandler.js';

class Main {
    constructor() {
        Extensions();

        Vue.use(httpVueLoader)
        new Vue({
            components: {
                app: httpVueLoader('js/ui/components/App.vue'),
            },
            store,
            template: '<app />',
        }).$mount('#app')

        socketHandler.addSocketClosedHandler(physicalBrowser.initConnection.bind(physicalBrowser))
        physicalBrowser.initConnection();

        audioStreamerContext.start();
        window.onload = () => {

          
        }

    }
}

new Main();
