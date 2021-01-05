<template>
  <section class="feature-permission">
    <!-- <div v-if="currentPermissionRequest != undefined" id="feature-permission"> -->
    <div v-if="permissionHandler.showPermissionRequest" id="feature-permission">
      <p>
        {{currentPermissionRequest.url}}
        wants to
      </p>
      <p v-if="currentPermissionRequest.permission == 0">
        <i class="fas fa-bell" style="color:#5f6063;"></i>
        Show notifications
      </p>
      <p v-if="currentPermissionRequest.permission == 1">
        <i class="fas fa-map-marker-alt" style="color:#5f6063;"></i>
        Know your location
        <br />Sharing your location is currently not supported
        <br />
      </p>
      <p v-if="currentPermissionRequest.permission == 2">
        <i class="fas fa-microphone" style="color:#5f6063;"></i>
        Use your microphone
      </p>
      <p v-if="currentPermissionRequest.permission == 3">
        <i class="fas fa-camera" style="color:#5f6063;"></i>
        Use your webcam
      </p>
      <p v-if="currentPermissionRequest.permission == 4">
        <i class="fas fa-camera" style="color:#5f6063;"></i>
        Use your webcam
        <br />
        <i class="fas fa-microphone" style="color:#5f6063;"></i>
        Use your microphone
      </p>
      <input type="button" value="Block" @click="denyPermission" />
      <input type="button" value="Allow" @click="grantPermission" />

      <a class="close-btn" @click="denyPermission">
        <i class="fas fa-times"></i>
      </a>
    </div>
  </section>
</template>
<script>
module.exports = new Promise(async (resolve, reject) => {
  const { permissionHandler } = await import("/js/state/PermissionHandler.js");
  resolve({
    name: "FeaturePermission",
    components: {},
    props: [],
    data() {
      return {
        permissionHandler: permissionHandler,
        permissions: permissionHandler.permissions
      };
    },
    computed: {
      currentPermissionRequest() {
        var filtered = this.permissions.find(permission => {
          return permission.state == "requested";
        });
        return filtered;
      }
    },
    mounted() {},
    methods: {
      denyPermission() {
        console.log("denyPermission");
        this.permissionHandler.denyPermission (
          this.currentPermissionRequest.url,
          this.currentPermissionRequest.permission
        );
      },
      grantPermission() {
        console.log("grantPermission");
        this.permissionHandler.grantPermission(
          this.currentPermissionRequest.url,
          this.currentPermissionRequest.permission
        );
      }
    },
    watch: {}
  });
});
</script>
<style>
#feature-permission {
  z-index: 2;
  background-color: white;
  position: absolute;
  left: 150px;
  font-family: Sans-Serif;
  width: 350px;
  height: 130px;
  box-shadow: 0px 0px 10px gray;
  border-radius: 2px;
}

#feature-permission > p {
  font-family: "Baloo Thambi 2", cursive;
  font-weight: 400;
  padding-left: 20px;
  padding-top: 10px;
}

#feature-permission > input {
  font-weight: bold;
  color: #5d8fca;
  background: white;
  box-shadow: 0px 0px 0px transparent;
  border: 1px solid lightgray;
  text-shadow: 0px 0px 0px transparent;
  border-radius: 3px;
  width: 70px;
  height: 30px;
  float: right;
  margin-right: 20px;
  margin-left: -10px;
}

.close-btn {
  position: absolute;
  right: 10px;
  top: 10px;
}

.close-btn:hover {
  background: rgba(255, 255, 255, 0.1);
}
</style>