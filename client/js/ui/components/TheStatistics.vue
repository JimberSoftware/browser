<template>
  <section class="statistics">
    <!-- [Resolution: 2048x1376], [20 fps], [Curr. Bandwidth Use: 0.00 Kbits/s], [40.19 Kbits/s Session Average],
    [Total:
    0.03 MiB], [14 NAL units], [13 frames in 00:00:06]-->
    <!-- {{ getFramesReceived.framesReceived }} -->
    <ul class="info-list">
      <li>[Frames received: {{ framesReceived }} fps: {{fps}}]</li>
      <li>[Session time: {{ formattedTime }}]</li>
      <li>[VideoBytes Received: {{ videoBytesReceived}}]</li>
      <li>[AudioBytes Received: {{ audioBytesReceived}}]</li>
      <li>[Resolution {{ videoCanvas.width }}x{{ videoCanvas.height }} ]</li>
      <li>[Webcam: {{ webCamBytesSent }} ]</li>
    </ul>
  </section>
</template>
<script>
module.exports = new Promise(async (resolve, reject) => {
  const { virtualBrowser } = await import("/js/state/VirtualBrowser.js");
  const { canvasStreamer } = await import("/js/state/CanvasStreamer.js");
  const { audioStreamerContext } = await import("/js/state/AudioStreamer.js");
  resolve({
    name: "TheStatistics",
    components: {},
    props: [],
    data() {
      return {
        videoStreamer: canvasStreamer,
        audioStreamer: audioStreamerContext,
        virtualBrowser: virtualBrowser,
        time: 0,
        fps: 0,
        frameSnapshot: 0,
        videoCanvas: window.player.canvas,
      };
    },
    computed: {
      framesReceived() {
        return this.videoStreamer.framesReceived;
      },
      videoBytesReceived() {
        return this.bytesToSize(this.videoStreamer.bytesReceived);
      },
      audioBytesReceived() {
        return this.bytesToSize(this.audioStreamer.bytesReceived);
      },
      webCamBytesSent() {
        return this.bytesToSize(this.virtualBrowser.webCamBytesSent)
      },
      formattedTime() {
        var sec_num = parseInt(this.time, 10);
        var hours = Math.floor(sec_num / 3600);
        var minutes = Math.floor((sec_num - hours * 3600) / 60);
        var seconds = sec_num - hours * 3600 - minutes * 60;

        if (hours < 10) {
          hours = "0" + hours;
        }
        if (minutes < 10) {
          minutes = "0" + minutes;
        }
        if (seconds < 10) {
          seconds = "0" + seconds;
        }
        return `${hours}h:${minutes}m:${seconds}s`;
      },

      width() {
        return this.videoContainerWidth
      },
    },
    mounted() {
      this.startTimer();
      this.videoContainer = document.getElementById('video-container')
    },
    methods: {
      startTimer() {
        this.interval = setInterval(this.incrementTime, 1000);
      },

      incrementTime() {
        this.fps = this.framesReceived - this.frameSnapshot;
        this.frameSnapshot = this.framesReceived;

        this.time = parseInt(this.time) + 1;
        var date = new Date(0);
        date.setSeconds(this.time); // specify value for SECONDS here
        var timeString = date.toISOString().substr(11, 8);
      },

      bytesToSize(bytes) {
        const sizes = ["Bytes", "KB", "MB", "GB", "TB"];
        if (bytes === 0) return "n/a";
        const i = parseInt(Math.floor(Math.log(bytes) / Math.log(1024)), 10);
        if (i === 0) return `${bytes} ${sizes[i]})`;
        return `${(bytes / 1024 ** i).toFixed(1)} ${sizes[i]}`;
      }
    },
    watch: {}
  });
});
</script>
<style scoped>
.statistics {
  display: flex;
  justify-content: center;
  box-shadow: 0 1px 6px 0 rgba(32, 33, 36, 0.28);
}
.info-list {
  margin-top: 5px;
  /* white-space: nowrap; */
  flex-wrap: wrap;
}
.info-list > li {
  display: inline-block;
}
</style>

