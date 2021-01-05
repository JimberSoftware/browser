<template>
  <section class="browser-bar">
    <button class="btn-backward" @click="backward">
      <img src="img/jimber/back.svg" />
    </button>
    <button class="btn-forward" @click="forward">
      <img src="img/jimber/forward.svg" />
    </button>
    <button @click="reload">
      <img src="img/jimber/refresh.svg" />
    </button>
    <!-- <span>SECURE HTTPS</span> -->
    <input
      @keyup.enter="browse"
      type="text"
      v-model="currentUrl"
      placeholder="Type a url"
    />
    <img class="logo" src="img/jimber/jimber_logo.svg" />
    <button @click="more" class="btn-more">
      <img src="img/jimber/more_vertical.svg" />
    </button>
  </section>
</template>
<script>
module.exports = new Promise(async (resolve, reject) => {
  const virtualBrowser = (await import("./../../state/VirtualBrowser.js"))
    .virtualBrowser;
  const physicalBrowser = (await import("./../../state/PhysicalBrowser.js"))
    .physicalBrowser;

  resolve({
    name: "ContextMenu",
    components: {},
    props: [],
    data() {
      return {
        virtualBrowser: virtualBrowser,
        currentUrl: virtualBrowser.currentUrl,
      };
    },
    computed: {
      virtualBrowserUrl() {
        return virtualBrowser.currentUrl;
      },
    },
    mounted() {},
    methods: {
      browse(e) {
        virtualBrowser.browse(this.currentUrl);
      },
      forward() {
        virtualBrowser.forward();
      },
      backward() {
        virtualBrowser.backward();
      },
      reload() {
        virtualBrowser.reload();
      },
      more() {
        this.$store.state.menuActive = true;
      },
    },
    watch: {
      virtualBrowserUrl(val) {
        this.currentUrl = val;
      },
    },
  });
});
</script>
<style scoped>
.browser-bar {
  padding: 10px 5px 10px 5px;
  justify-content: center;
  align-items: center;
  display: flex;

  background: gainsboro;
  display: flex;
  /* padding-right: 5px; */
}

.btn-backward {
  fill: red;
}

.browser-bar {
  /* padding: 1em; */
}

.browser-bar > * {
  vertical-align: middle;
}

img {
  display: inline-block;
  width: 20px;
  color: red;
}

img.logo {
  height: 22px;
  width: auto;
  margin-left: 20px;
}

@media (max-width: 800px) {
  img.logo {
    display: none;
  }
}

button {
  display: inline-block;
  padding: 0;
  margin: 1px 10px 0 0;
  border: none;
  background: none;
  display: inline-block;
  width: 25px;
  height: 25px;

  justify-content: center;
  align-items: center;
  display: flex;
}
button:hover {
  border-radius: 50%;
  background-color: rgba(0, 0, 0, 0.1);
}

/* input[type="text"] {
  display: inline-block;
  width: 40%;
  border-color: #a0a0a0;
  border-width: 1px;
  background-color: white;
  /* margin-top: -1px;
  height: 25px;
  color: #666;
  padding: 5px 15px !important;
} */

.btn-forward {
  color: rgba(0, 0, 0, 0.54);
}
input {
  flex: 1;
  border-radius: 30px;
  color: white;
  display: inline-block;
  width: 40%;
  /* border-color: none; */
  /* border-width: 1px; */
  /* margin-top: -1px; */
  border: none;
  height: 25px;
  color: #000;
  font-size: 1rem;
  padding: 5px 15px;
  box-shadow: 0 0 2px 0px #000;

  overflow: hidden;
  text-overflow: ellipsis;
}

:focus,
:active {
  outline: none;
}

.btn-more {
  margin-left: 20px;
}
</style>