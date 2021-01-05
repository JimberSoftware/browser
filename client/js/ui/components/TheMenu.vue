
<template>
  <section class="the-menu">
    <div id="themenu">
      <a
        class="toggleMenu"
        @mousedown="onMouseDownMenu"
        @click="onMenuClick"
        @touchmove="onTouchMoveMenu"
        @touchstart="onTouchStartMenu"
        v-if="!this.$store.state.showBrowserBar"
      >
        <span v-if="this.$store.state.menuActive">
          <i class="fas fa-times"></i>
        </span>
        <span v-else>
          <i class="fas fa-bars"></i>
        </span>
      </a>
      <div :class="menuClass" id="jmenu">
        <ul>
          <li>
            <TheConfig />
          </li>
          <li @click="toggleDownloadList">
            <a>Downloads</a>
          </li>
          <li>
            <a>Print page</a>
          </li>
          <li v-if="logoClickCounter >= 5">
            <TheCIInfo />
          </li>
        </ul>
        <img
          id="jimberlogo"
          src="/img/jimber/jimber_logo.svg"
          alt="Jimber Logo"
          @click="logoClicked"
        />
        <button class="btn-close" @click="onMenuClick">
          <img src="img/jimber/close.svg" />
        </button>
      </div>
    </div>
  </section>
</template>
<script>
module.exports = new Promise(async (resolve, reject) => {
  const ContextMenuEvents = (
    await import("./../../virtualbrowser/ContextMenuEvents.js")
  ).default;

  const Config = (await import("./../../../config.js")).default;

  const { physicalBrowser } = await import("/js/state/PhysicalBrowser.js");
  resolve({
    name: "TheMenu",
    components: {
      theconfig: httpVueLoader("js/ui/components/TheConfig.vue"),
      theciinfo: httpVueLoader("js/ui/components/TheCIInfo.vue"),
    },
    props: [],
    data() {
      return {
        menuActive: this.$store.state.menuActive,
        logoClickCounter: 0,
        draggingMenu: {
          drag: false,
          clickPosX: 0,
          clickPosY: 0,
        },
        shouldOpenMenu: true,
        ContextMenuEvents: ContextMenuEvents,
      };
    },
    computed: {
      menuClass() {
        return `menu ${this.$store.state.menuActive ? "active" : "hidden"}`;
      },
      browserBarEnabled() {
        return Config.SHOW_BROWSER_BAR;
        // return false;
      },
    },
    mounted() {
      this.$root.$on("touch", this.onTouch);
      this.$root.$on("click", this.onClick);
      this.$root.$on("mouseup", this.onMouseUp);
      this.$root.$on("mousemove", this.onMouseMove);
      var menu = document.getElementById("jmenu");
      menu.addEventListener("wheel", (e) => {
        e.preventDefault();
      });
    },
    methods: {
      onDragMove(x, y, mousePosOnMenuX, mousePosOnMenuY) {
        if (this.draggingMenu.drag) {
          this.shouldOpenMenu = false;
          let element = document.getElementsByClassName("toggleMenu")[0];
          element.classList.toggle("dragMenu", true);
          let container = document.getElementById("video-container");
          let width = window.isSafari()
            ? container.offsetWidth / window.devicePixelRatio
            : container.offsetWidth;
          let height = window.isSafari()
            ? container.offsetHeight / window.devicePixelRatio
            : container.offsetHeight;
          element.style.right = `${width - x}px`;
          element.style.left = `${x - mousePosOnMenuX}px`;
          element.style.top = `${y - mousePosOnMenuY}px`;
        }
      },
      onDragEnd(x, y) {
        if (this.draggingMenu.drag) {
          let element = document.getElementsByClassName("toggleMenu")[0];
          element.style.transition = "all 0.4s";
          let container = document.getElementById("video-container");
          let width = window.isSafari()
            ? container.offsetWidth / window.devicePixelRatio
            : container.offsetWidth;
          let height = window.isSafari()
            ? container.offsetHeight / window.devicePixelRatio
            : container.offsetHeight;

          if (y < element.offsetHeight) {
            element.style.top = `0px`;
          }

          if (y + element.offsetHeight > height) {
            element.style.top = `${height - element.offsetHeight}px`;
          }

          if (x < width / 2) {
            element.classList.toggle("left", true);
            element.style.left = `0px`;
            element.style.right = ``;
          } else {
            element.classList.toggle("left", false);
            element.style.right = `0px`;
            element.style.left = ``;
          }

          element.classList.toggle("dragMenu", false);
          this.draggingMenu.drag = false;
        }
      },
      onMouseMove(event) {
        this.onDragMove(
          parseInt(event.clientX),
          parseInt(event.clientY),
          parseInt(this.draggingMenu.mousePosX),
          parseInt(this.draggingMenu.mousePosY)
        );
      },
      onMouseUp(event) {
        this.onDragEnd(event.clientX, event.clientY);
      },
      onClick(event) {
        ContextMenuEvents.hideContextMenu();
      },
      onTouch(event) {
        if (event.type == "touchend") {
          this.onDragEnd(
            event.changedTouches[0].clientX,
            event.changedTouches[0].clientY
          );
        }
      },
      logoClicked() {
        this.logoClickCounter++;
      },
      isMobileOrTablet() {
        return window.isMobileOrTablet();
      },
      onTouchStartMenu(e) {
        let element = document.getElementsByClassName("toggleMenu")[0];
        element.style.transition = "all 0s";
      },
      onTouchMoveMenu(e) {
        this.draggingMenu.drag = true;
        this.onDragMove(
          parseInt(event.touches[0].clientX),
          parseInt(event.touches[0].clientY),
          0,
          0
        );
      },
      onMouseDownMenu(event) {
        let element = document.getElementsByClassName("toggleMenu")[0];
        element.style.transition = "all 0s";
        this.draggingMenu.drag = true;
        this.draggingMenu.mousePosX = event.layerX;
        this.draggingMenu.mousePosY = event.layerY;
      },
      onMenuClick(event) {
        if (!this.shouldOpenMenu) {
          this.shouldOpenMenu = true;
          return;
        }
        this.$store.state.menuActive = !this.$store.state.menuActive;
      },
      toggleDownloadList() {
        this.$store.state.showDownloadList = !this.$store.state
          .showDownloadList;
        this.$store.state.showBottomDownloadList = false;
        this.$store.state.menuActive = false;
      },
      toggleStats() {
        this.$store.state.showStats = !this.$store.state.showStats;
        this.$store.state.menuActive = false;
        setTimeout(() => {
          // ooooh dirty boi
          physicalBrowser.onResize();
        }, 100);
      },
      toggleBrowserBar() {
        this.$store.state.showBrowserBar = !this.$store.state.showBrowserBar;
        this.$store.state.menuActive = false;
        setTimeout(() => {
          // ooooh dirty boi
          physicalBrowser.onResize();
        }, 100);
      },
    },
    watch: {},
  });
});
</script>
<style scoped>
#themenu {
  width: 100%;
  height: 100%;
}
#jimberlogo {
  object-fit: cover;
  max-width: 1000px;
  width: 20%;
  position: fixed;
  bottom: 20px;
}

@media (max-height: 600px) {
  #jimberlogo {
    display: none;
  }
}

@media (max-height: 400px) {
  #jmenu {
    font-size: 0.8em;
  }
  ul {
    overflow: scroll;
  }
}

.menu {
  z-index: 2;
  position: fixed;
  left: 0;
  top: 0;
  width: 100%;
  height: 100%;
  background: var(--main-bg-color);
  visibility: hidden;
  display: -webkit-box;
  display: -moz-box;
  display: -ms-flexbox;
  display: -webkit-flex;
  display: flex;
  align-items: center;
  justify-content: center;
  opacity: 0;
  transition: all 100ms ease-in-out;
  flex-direction: column;
  text-align: center;
}

.left {
  border-radius: 0 20% 20% 0 !important;
}

.dragMenu {
  border-radius: 50% 50% 50% 50% !important;
}

.menu.active {
  visibility: visible;
  opacity: 1;
}

.menu ul {
  padding-left: 0;
}

.menu ul li {
  color: var(--main-txt-color);
  text-align: center;
  list-style-type: none;
  font-size: 2em;
  cursor: pointer;
  margin-bottom: 20px;
  display: flex;
  align-items: center;
  justify-content: center;
}

.toggleMenu {
  position: fixed;
  right: -2px;
  top: -2px;
  background: var(--main-bg-color);
  z-index: 3;
  padding: 0.5em 0.75em;
  border-radius: 20% 0 0 20%;
  width: 1em;
  text-align: center;
  border: 1px solid rgba(0, 0, 0, 0.12);
}

.btn-close {
  border: none;
  width: 30px;
  height: 30px;
  justify-content: center;
  align-items: center;
  display: flex;
  position: fixed;
  top: 15px;
  right: 12px;
}
.btn-close:hover {
  border-radius: 50%;
  background-color: rgba(0, 0, 0, 0.1);
}
</style>
