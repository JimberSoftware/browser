<template>
<section class="contextmenu">
    <div id="contextmenu">
        <ul id="contextmenu-options" @click="ContextMenuEvents.hideContextMenu">
                <li id="CANBACK" class="contextmenu-option disabled-option" @click="ContextMenuEvents.back">Back</li>
                <li id="CANFORWARD" class="contextmenu-option disabled-option" @click="ContextMenuEvents.forward">Forward</li>
                <li id="CANRELOAD" class="contextmenu-option " @click="ContextMenuEvents.reload">Reload</li>
                <li id="CANUNDO" class="contextmenu-option disabled-option" @click="ContextMenuEvents.undo">Undo</li>
                <li id="CANREDO" class="contextmenu-option disabled-option" @click="ContextMenuEvents.redo">Redo</li>
                <li id="CANCUT" class="contextmenu-option disabled-option" @click="ContextMenuEvents.cut">Cut</li>
                <li id="CANCOPY" class="contextmenu-option disabled-option" @click="ContextMenuEvents.copy">Copy</li>
                <li id="CANPASTE" class="contextmenu-option disabled-option" @click="ContextMenuEvents.paste">Paste</li>
                <li id="CANSELECTALL" class="contextmenu-option disabled-option" @click="ContextMenuEvents.selectall">Select All</li>
                <li id="CANOPENLINK" class="contextmenu-option" @click="ContextMenuEvents.openlink">Open link in new tab</li>
                <li id="CANCOPYLINK" class="contextmenu-option" @click="ContextMenuEvents.copylink">Copy link</li>
                <li id="CANOPENIMAGE" class="contextmenu-option" @click="ContextMenuEvents.openimage">Open image in new tab</li>
                <li id="CANCOPYIMAGE" class="contextmenu-option" @click="ContextMenuEvents.copyimage">Copy image</li>
                <li id="CANCOPYIMAGELINK" class="contextmenu-option" @click="ContextMenuEvents.copyimageLink">Copy image link</li>
                <li id="CANOPENVIDEO" class="contextmenu-option" @click="ContextMenuEvents.openvideo">Open media in new tab</li>
                <li id="CANCOPYVIDEO" class="contextmenu-option" @click="ContextMenuEvents.copyvideo">Copy media link</li>
                <!-- <hr> -->
                <li id="CANPRINT" class="contextmenu-option" @click="ContextMenuEvents.print">Print</li>
        </ul>
   </div>
</section>
</template>
<script>
module.exports = new Promise(async (resolve, reject) => {
const Command = (await import('../../models/Command.js')).Command;
const ServerToClientCommand = (await import('../../Proto.js')).ServerToClientCommand;
const ClientToServerCommand = (await import('../../Proto.js')).ClientToServerCommand;
const virtualBrowser = (await import('./../../state/VirtualBrowser.js')).virtualBrowser;
const ContextMenuEvents = (await import('./../../virtualbrowser/ContextMenuEvents.js')).default;

resolve({
    name: "ContextMenu",
    components: {},
    props: [],
    data() {
      return {
          ContextMenuEvents: ContextMenuEvents
      };
    },
    computed: {},
    mounted() {
    },
    methods: {
    },
    watch: {

    }
  });
});
</script>
<style>
  #contextmenu {
    background: white;
    width: 180px;
    z-index: 2;
    /* box-shadow: 0 4px 5px 3px rgba(0, 0, 0, 0.2); */
    box-shadow: 0 1px 6px rgba(60,64,67,.28);
    position: fixed;
    display: none !important;


    border: none;
    border-radius: 4px;
    min-width: 112px;
    max-width: 280px;
    color: #3C4043;
  }
  
  #contextmenu-options {
    list-style: none;
    padding: 5px 0;
    z-index: 10000;
  }
  
  .contextmenu-option {
    display: none;
    font-family: Roboto, Arial, sans-serif;
    font-weight: 400;
    z-index: 1;
    font-size: 16px;
    padding: 10px 10px 10px 10px;
    color: #3c4043;
    cursor: pointer;
  }
  
  .mobilecontextmenu-option {
    display: block;
    font-family: "Ubuntu", sans-serif;
    font-weight: 400;
    z-index: 1;
    font-size: 14px;
    padding: 10px 10px 10px 10px;
    cursor: pointer;
  }
  
  .contextmenu-option:hover {
    background: #e9e9e9
  }
  
  .disabled-option {
    background: rgba(0, 0, 0, 0) !important;
    cursor: default;
    color: lightgray;
  }
  
  hr {
    background-color: grey;
    border-left: 0px;
    border-top: 10x;
    border-right: 0px;
    border-bottom: 0px;
  }
</style>