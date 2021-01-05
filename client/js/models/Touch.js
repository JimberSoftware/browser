class Touch {
    constructor(event) {
        this.identifier = event.identifier;
        this.clientX = event.clientX;
        this.clientY = event.clientY;
        this.force = event.force;
        this.pageX = event.pageX;
        this.pageY = event.pageY;
        this.radiusX = event.radiusX;
        this.radiusY = event.radiusY;
        this.rotatingAngle = event.rotatingAngle;
        this.screenX = event.screenX;
        this.screenY = event.screenY;
    }
}

export default Touch;