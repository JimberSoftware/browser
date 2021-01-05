<template>
  <section :class="`downloadlist ${dense ? 'dense' : ''}`">
    <header v-if="!dense">
      <h1>
        <span>Downloads</span>
        <a class="downloads-close-btn" @click="closeDownloadList">
          <i class="fas fa-times"></i>
        </a>
      </h1>
    </header>
    <div class="downloads">
      <DownloadListItem
        class="downloads_item"
        v-for="download in reverseDownloads"
        :key="download.id"
        :download="download"
        :dense="dense"
      />
    </div>

    <a v-if="dense" class="show-all-btn" @click="openDownloadList">Show all</a>
    <a
      v-if="dense"
      class="dense-downloads-close-btn"
      @click="closeDenseDownloadList"
    >
      <i class="fas fa-times"></i>
    </a>
  </section>
</template>
<script>
module.exports = new Promise(async (resolve, reject) => {
  const { downloadHandler } = await import("/js/state/DownloadHandler.js");
  const { physicalBrowser } = await import("/js/state/PhysicalBrowser.js");

  resolve({
    name: "DownloadList",
    components: {
      downloadlistitem: "url:js/ui/components/DownloadListItem.vue",
    },
    props: ["dense"],
    data() {
      return {
        downloadHandler,
      };
    },
    computed: {
      downloads() {
        return downloadHandler.downloads;
      },
      reverseDownloads() {
        return this.downloads.slice().reverse();
      },
    },
    mounted() {
      setTimeout(() => {
        // ooooh dirty boi
        physicalBrowser.onResize();
      }, 100);
    },
    methods: {
      openDownloadList() {
        this.$store.state.showDownloadList = true;
        this.$store.state.showBottomDownloadList = false;
        setTimeout(() => {
          // ooooh dirty boi
          physicalBrowser.onResize();
        }, 100);
      },
      closeDownloadList() {
        this.$store.state.showDownloadList = false;
        setTimeout(() => {
          // ooooh dirty boi
          physicalBrowser.onResize();
        }, 100);
      },
      closeDenseDownloadList() {
        this.$store.state.showBottomDownloadList = false;
        setTimeout(() => {
          // ooooh dirty boi
          physicalBrowser.onResize();
        }, 100);
      },
    },
  });
});
</script>
<style scoped>
.downloadlist {
  min-width: 250px;
  width: 30vw;
}
.subtitle {
  padding-bottom: 0;
}

.downloadlist header {
  display: flex;
  justify-content: space-between;
  padding: 20px;
  background-color: var(--main-txt-color);
}

.downloadlist h1 {
  width: 100%;
  display: flex;
  justify-content: space-between;
  font-family: "Roboto", sans-serif;
  font-weight: 500;
  font-size: 1.5rem;
  color: white;
}

.downloadlist .downloads {
  height: 100%;
  width: 100%;
  overflow-y: scroll;
}

.downloadlist.dense {
  width: 100%;
  height: 60px;
  display: flex;
  background: #f2f2f2;
  align-items: center;
  justify-content: center;
  overflow: hidden;
}
.downloadlist.dense .downloads {
  position: static;
  display: flex;
  width: 100%;
  height: 100%;
  flex-wrap: wrap;
  overflow: hidden;
}
.downloadlist.dense .downloads_item {
  list-style-type: none;
}
.downloadlist.dense .show-all-btn {
  width: 100px;
  text-align: center;
  padding: 5px 2px 5px 2px;
  border-radius: 3px;
  border: 1px solid rgba(0, 0, 0, 0.12);
  color: #414141;
  transition: 0.3s;
}
.downloadlist.dense .show-all-btn:hover {
  background-color: #e0e0e0;
  border: 1px solid rgba(0, 0, 0, 0);
  transition: 0.3s;
}

.downloadlist.dense .dense-downloads-close-btn {
  margin-left: 12px;
  margin-right: 12px;
  padding: 5px 10px 5px 10px;
  color: #414141;
  border-radius: 3px;
  transition: 0.3s;
}
.downloadlist.dense .dense-downloads-close-btn:hover {
  background-color: #e0e0e0;
  transition: 0.3s;
}
</style>

