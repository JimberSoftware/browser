<template>
  <section class="config">
    <a @click="showConfig = !showConfig">Settings</a>
    <div :class="`config-content ${showConfig ? 'active' : 'hidden'}`">
      <!-- <p>Do not render while out of focus</p> -->
      <label for="quality-slider">Quality</label>
      <div class="slider-wrapper">
        <span style="display: inline; width: 25%">Low</span>
        <input name="quality-slider" type="range" min="1" max="10" class="slider" v-model="quality"/>
        <span style="display: inline; width: 25%">High</span>
      </div>

      <label>Toggle browserbar</label>
      <label class="switch_browserbar">
        <input @change="toggleBrowserBar" type="checkbox" v-model="browserBar">
        <span class="slider_browserbar round"></span>
      </label>
    </div>
  </section>
</template>
<script>
module.exports = new Promise(async (resolve, reject) => {
  const {virtualBrowser} = await import("/js/state/VirtualBrowser.js");
  const { physicalBrowser } = await import("/js/state/PhysicalBrowser.js");
  resolve({
    name: "TheConfig",
    components: {},
    props: [],
    data() {
      return {
        showConfig: false,
        renderOutOfFocus: true,
        quality: 5,
        switch1: true,
        browserBar: this.$store.state.showBrowserBar
      };
    },
    computed: {},
    mounted() {
    },
    methods: {
      changeQuality() {
      },
      toggleBrowserBar() {
        this.$store.state.showBrowserBar = !this.$store.state.showBrowserBar;
        setTimeout(() => { // ooooh dirty boi
          physicalBrowser.onResize();
        }, 100);
      },
    },
    watch: {
      quality(val) {
        clearTimeout(this.qualityTimer);
        this.qualityTimer = setTimeout(() => {
          virtualBrowser.changeQuality(val);
        }, 300);
      },
    }
  });
});
</script>
<style scoped>
.config {
  position: relative;
  height: 100%;
  cursor: default;
}

.config-content {
  height: 0px;
  width: auto;
  transition: all 100ms ease-in-out;
  overflow: hidden;
  border: none;
  display: flex;
  flex-direction: column;
  margin: 0;
  padding: 0;
}
.slider-wrapper{
  margin-bottom: 20px;
}
.config-content.active {
  height: auto;
  margin-top: 0.5em;
  border: 1px solid rgba(0, 0, 0, 0.12);
  padding: 1em 2em;
}

.slider {
  display: inline;
  margin-top: 15px;
  width: 45%;
  -webkit-appearance: none;
  appearance: none;
  height: 4px;
  background: var(--accent-bg-color);
  outline: none;
  -webkit-transition: 0.2s;
  transition: opacity 0.2s;
  border-radius: 2px;
}

.inline {
  display: inline;
}

.slider::-webkit-slider-thumb:hover {
  background: var(--accent-txt-color);
}

.slider::-webkit-slider-thumb {
  position: relative;
  -webkit-appearance: none;
  appearance: none;
  border-radius: 50%;
  width: 10px;
  height: 10px;
  background: #8aa6d0;
  cursor: pointer;
}
.switch_browserbar {
  margin: 10px auto;
  position: relative;
  display: inline-block;
  width: 60px;
  height: 34px;
}

.switch_browserbar input {
  opacity: 0;
  width: 0;
  height: 0;
}

.slider_browserbar {
  position: absolute;
  cursor: pointer;
  top: 0;
  left: 0;
  right: 0;
  bottom: 0;
  background-color: #ccc;
  -webkit-transition: .4s;
  transition: .4s;
}

.slider_browserbar:before {
  position: absolute;
  content: "";
  height: 26px;
  width: 26px;
  left: 4px;
  bottom: 4px;
  background-color: white;
  -webkit-transition: .4s;
  transition: .4s;
}

input:checked + .slider_browserbar {
  background: #8aa6d0;
}

input:focus + .slider_browserbar {
  background: #8aa6d0;
}

input:checked + .slider_browserbar:before {
  -webkit-transform: translateX(26px);
  -ms-transform: translateX(26px);
  transform: translateX(26px);
}

/* Rounded sliders */
.slider_browserbar.round {
  border-radius: 34px;
}

.slider_browserbar.round:before {
  border-radius: 50%;
}
</style>

