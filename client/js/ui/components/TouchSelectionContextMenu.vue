<template>
<section class="touchselectioncontextmenu">
    <div id="touchselectioncontextmenu">
        <ul id="touchselectioncontextmenu-options" @click="ContextMenuEvents.hideTouchSelectionContextMenu">
            <li id="MOBILECANCUT" class="touchselectioncontextmenu-option" @click="ContextMenuEvents.cut">Cut</li>
            <li id="MOBILECANCOPY" class="touchselectioncontextmenu-option" @click="ContextMenuEvents.copy">Copy</li>
            <li id="MOBILECANPASTE" class="touchselectioncontextmenu-option" @click="ContextMenuEvents.paste">Paste</li>
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
  #touchselectioncontextmenu {
    background: white;
    z-index: 2;
    box-shadow: 0 4px 5px 3px rgba(0, 0, 0, 0.2);
    position: fixed;
    display: none !important;
  }
  
  
  #touchselectioncontextmenu-options {
    display: flex;
    list-style: none;
    z-index: 10000;
    height: 100%;
  }
  
  .touchselectioncontextmenu-option {
    flex: 1;
    display: none;
    font-family: "Ubuntu", sans-serif;
    font-weight: 400;
    z-index: 1;
    font-size: 14px;
    cursor: pointer;
    border-left: 1px solid lightgray;
    text-align: center;
    flex-direction: column;
    justify-content: center;
  }
   .touchselectioncontextmenu-option:first-of-type {
     border-left:none !important;
   }
  
  .touchselectioncontextmenu-option:hover {
    background: rgba(0, 0, 0, 0.2);
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