const DownloadState = {
    // created: 0,
    // finished: 1,
    // downloading: 2,
    // paused: 3,
    // cancelled: 4
}

class Download {
    constructor(id, fileName, path) {
        this.id = id;
        this.fileName = fileName;
        this.path = path;
        this.bytesReceived = 0;
        this.state = "created";
        this.date = new Date()
    }
    setState(state) {
        if(this.state === "canceled") return;
        this.state = state;
    }
}

export default Download;