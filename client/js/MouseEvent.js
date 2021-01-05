"use strict";
import SystemTools from './SystemTools.js';

class MouseEvent {

    constructor(event) {
        this.which = event.which;
        this.type = event.type;
        // this.clientX = event.offsetX*window.devicePixelRatio;
        this.clientX = event.offsetX;
        // this.clientY = event.offsetY*window.devicePixelRatio;
        this.clientY = event.offsetY;
        this.modifiers = SystemTools.translateModifiers(event);
    }
    
};

export default MouseEvent;