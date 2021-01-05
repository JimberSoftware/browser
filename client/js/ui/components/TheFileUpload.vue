<template>
  <div class="file-upload">
    <h1 v-if="!!multiple">Please choose your files</h1>
    <h1 v-else>Please choose your file</h1>

    <input type="file" name="file" :multiple="!!multiple" :accept="mimeTypes" />
    <div class="action-buttons">
      <button v-on:submit.prevent v-on:click="submitFiles">Submit</button>
      <button v-on:submit.prevent v-on:click="cancelUpload">Cancel</button>
    </div>
    <img v-if="submitted" src="img/ui/loading-png-gif.gif" class="loading-img" />
  </div>
</template>
<script>
module.exports = new Promise(async (resolve, reject) => {
  const { uploadHandler } = await import("/js/state/UploadHandler.js");
  resolve({
    name: "TheFileUpload",
    components: {},
    props: [],
    data() {
      return {
        uploadHandler,
        multiple: uploadHandler.upload.multiple,
        mimeTypes: uploadHandler.upload.mimeTypes,
        upload: uploadHandler.upload,
        submitted: false
      };
    },
    computed: {},
    mounted() {},
    methods: {
      cancelUpload() {
        this.uploadHandler.cancelUpload();
        this.$store.state.showFileUpload = false;
      },
      submitFiles() {
        this.submitted = true;
        this.uploadHandler.uploadFiles();
        // this.$store.state.showFileUpload = false;
      }
    },
    watch: {
    }
  });
});
</script>
<style scoped>
.file-upload {
  width: 300px;
  height: 200px;
  border-radius: 3px;
  display: flex;
  flex-direction: column;
  align-content: center;
  align-items: center;
  background-color: white;
  box-shadow: 0 4px 5px 3px rgba(0, 0, 0, 0.2);
  font-weight: 400;
}

.action-buttons {
  display: flex;
  width: 50%;
  justify-content: space-between;
  justify-items: center;
}

.loading-img {
  width: 10%;
  height: auto;
}
</style>