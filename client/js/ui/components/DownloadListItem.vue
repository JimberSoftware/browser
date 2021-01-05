
<template>
  <div :class="`download ${dense? 'dense':'border '}`">
    <div class="icon">
      <i class="fas fa-file-alt"></i>
    </div>
    <div class="info">
      <div class="download-item-filename">{{(download.fileName)}}</div>
      <div class="download-item-status">{{download.state}}</div>
    </div>

    <div class="control-buttons-wrapper">
      <a
        v-if="isReady && downloadsEnabled"
        v-bind:href="downloadPath"
        v-bind:download="download.fileName"
        class="mr"
      >
        <i class="fas fa-download download-button"></i>
      </a>
      <a v-if="isPaused && !isCanceled" @click="resume" class="mr">
        <i class="fas fa-play"></i>
      </a>
      <a v-if="isDownloading" @click="pause" class="mr">
        <i class="fas fa-pause"></i>
      </a>
      <a v-if="!isReady && !isCanceled" @click="cancel" class="mr">
        <i class="fas fa-stop"></i>
      </a>
    </div>

    <a class="close">
      <i class="fas fa-times"></i>
    </a>

    <div class="progress-bg">
      <div
        v-if="!isReady"
        class="progress-fg"
        :style="`width: ${Math.round(download.bytesReceived / download.maxSize *100)}%`"
      ></div>
    </div>
  </div>
</template>

<script>
module.exports = new Promise(async (resolve, reject) => {
  const { downloadHandler } = await import("/js/state/DownloadHandler.js");
  const { virtualBrowser } = await import("/js/state/VirtualBrowser.js");
  const Config = await import("/config.js");

  resolve({
    name: "DownloadListItem",
    components: {},
    props: ["download", "dense"],
    data() {
      return {
        downloadPath: `//${Config.default.BROKER_HOST}/downloads/${virtualBrowser.userId}/${this.download.path}/${this.download.fileName}`,
        downloadsEnabled: virtualBrowser.downloadsEnabled
      };
    },
    computed: {
      isReady() {
        return this.download.state == "finished";
      },
      isCanceled() {
        return this.download.state == "canceled";
      },
      isDownloading() {
        return this.download.state == "downloading";
      },
      isPaused() {
        return this.download.state == "paused";
      }
    },
    mounted() {},
    methods: {
      pause() {
        downloadHandler.doPause(this.download.id);
      },
      cancel() {
        downloadHandler.doCancel(this.download.id);
      },
      resume() {
        downloadHandler.doResume(this.download.id);
      },
      copyPath() {
        navigator.clipboard.writeText(this.download.path).then(
          function() {
            this.$root.$emit("showAlert", `File id copied to clipboard`);
          },
          function(err) {
            this.$root.$emit("showAlert", `Could not copy file id`);
            console.error("Async: Could not copy text: ", err);
          }
        );
      }
    }
  });
});
</script>
<style scoped>
.download {
  display: flex;
  flex-direction: row;
  color: var(--main-txt-color);
  background: white;
  margin: 20px;
  padding: 10px;
  position: relative;
  border-radius: 2px;
  overflow: hidden;
  align-items: center;
  font-size: 12px;
}

.download:hover {
  background: rgba(255, 255, 255, 0.15);
  cursor: pointer;
}
.icon {
  /* display: flex;
  flex-grow: 1; */
  margin-left: 10px;
  font-size: 1.5em;
}
.icon p {
  margin: 0;
}
.info {
  width:100%;
  max-width: 100%;
  border-left: 1px solid rgba(255, 255, 255, 0.1);
  padding: 0 1em;
  box-sizing: border-box;  
}
.info .download-item-filename {
  font-weight: 500;
  text-overflow: ellipsis;
  white-space: nowrap;
  overflow: hidden;
  width:60%;

}
.info .download-item-status {
  font-weight: 300;
}
.mr {
  margin-right: 1em;
}
.center {
  text-align: center;
  width: 100%;
  padding-right: 16px;
}
.close {
  position: absolute;
  right: 0px;
  top: -2px;
  background: var(--main-bg-color);
  z-index: 3;
  padding: 0.25em 0.5em;
  border-radius: 0 0 0 50%;
  width: 0.5em;
  text-align: center;
}
.close:hover {
  background: rgba(255, 255, 255, 0.1);
}
.progress-bg,
.progress-fg {
  position: absolute;
  background: var(--accent-txt-color);
  position: absolute;
  width: 100%;
  height: 2px;
  left: 0;
  bottom: 0;
  z-index: 2;
}
.progress-bg {
  background: transparent;
}
.download-button {
  color: black;
}
.control-buttons-wrapper {
  position: absolute;
  right: 10px;
}

.download.dense {
  display: flex;
  margin: 3px;
  padding: 5px;
  background: var(--main-bg-color);
  border-right: 1px solid #c9cacb;
  border-radius: 2;
  width: 234px;
  height: 40px;
}

.download.dense .center {
  padding-right: 0;
}

.download.dense .close {
  right: 5px;
}
</style>