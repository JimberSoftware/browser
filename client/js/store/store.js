Vue.use(Vuex)

const state = {
  showStats: false,
  showDownloadList: false,
  showBottomDownloadList: false,
  showFileUpload: false,
  showBrowserBar: false,
  // shouldOpenMenu: true,
  menuActive: false
}

const store = new Vuex.Store({
  state,
  // getters,
  // actions,
  // mutations
})

export default store