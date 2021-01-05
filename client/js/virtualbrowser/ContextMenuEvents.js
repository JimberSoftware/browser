import { Command } from '../models/Command.js'
import { ClientToServerCommand } from '../Proto.js'
import { virtualBrowser } from './../state/VirtualBrowser.js';

function hideContextMenu() {
    var menu = document.getElementById("contextmenu");
    menu.setAttribute('style', 'display:none !important');
}

function hideTouchSelectionContextMenu() {
    var menu = document.getElementById("touchselectioncontextmenu");
    menu.setAttribute('style', 'display:none !important');
}

function copy() {
    document.execCommand("copy");
    var c = new Command();
    c.setContent(ClientToServerCommand.COPYEVENT, []);
    c.send();
}

function cut() {
    copy();
    var c = new Command();
    c.setContent(ClientToServerCommand.KEYBOARDEVENT, ["Backspace", "Backspace", 2, 0]);
    c.send();
    c.setContent(ClientToServerCommand.KEYBOARDEVENT, ["Backspace", "Backspace", 1, 0]);
    c.send();
}

function paste() {
    //Clipboard is set whenever a context menu is opened
    var c = new Command();
    c.setContent(ClientToServerCommand.KEYBOARDEVENT, ["KeyV", "v", 2, 67108864]);
    c.send();
    c.setContent(ClientToServerCommand.KEYBOARDEVENT, ["KeyV", "v", 1, 67108864]);
    c.send();
}

function selectall() {
    var c = new Command();
    c.setContent(ClientToServerCommand.KEYBOARDEVENT, ["KeyA", "a", 2, 67108864]);
    c.send();
    c.setContent(ClientToServerCommand.KEYBOARDEVENT, ["KeyA", "a", 1, 67108864]);
    c.send();
}

function undo() {
    var c = new Command();
    c.setContent(ClientToServerCommand.KEYBOARDEVENT, ["KeyZ", "z", 2, 67108864]);
    c.send();
    c.setContent(ClientToServerCommand.KEYBOARDEVENT, ["KeyZ", "z", 1, 67108864]);
    c.send();
}

function redo() {
    var c = new Command();
    c.setContent(ClientToServerCommand.KEYBOARDEVENT, ["KeyY", "y", 2, 67108864]);
    c.send();
    c.setContent(ClientToServerCommand.KEYBOARDEVENT, ["KeyY", "y", 1, 67108864]);
    c.send();

    c.send();
}

function reload() {
    var c = new Command();
    c.setContent(ClientToServerCommand.RELOAD, []);
    c.send();
}

function back() {
    var c = new Command();
    c.setContent(ClientToServerCommand.BACK, []);
    c.send();
}

function forward() {
    var c = new Command();
    c.setContent(ClientToServerCommand.FORWARD, []);
    c.send();
}

function print() {
    var c = new Command();
    c.setContent(ClientToServerCommand.PRINT, []);
    c.send();
}

//todo send to the backend and open a new tab there
function openlink() {
    window.open(`//${window.location.host}/#${virtualBrowser.contextMenu.linkUrl}`, '_blank')
}
async function copylink() {
    await navigator.clipboard.writeText(`${window.location.host}/#${virtualBrowser.contextMenu.linkUrl}`);
}
function openimage() {
    window.open(`//${window.location.host}/#${virtualBrowser.contextMenu.mediaUrl}`, '_blank')
}
async function copyimage() {    
    var c = new Command();
    c.setContent(ClientToServerCommand.COPYIMAGE, []);
    c.send();
}

async function copyimageLink() {
    await navigator.clipboard.writeText(`//${window.location.host}/#${virtualBrowser.contextMenu.mediaUrl}`);
}
function openvideo() {
    window.open(`//${window.location.host}/#${virtualBrowser.contextMenu.mediaUrl}`, '_blank')
}
async function copyvideo() {
    await navigator.clipboard.writeText(`//${window.location.host}/#${virtualBrowser.contextMenu.mediaUrl}`);
}


export default {
    hideContextMenu,
    hideTouchSelectionContextMenu,
    copy,
    cut,
    paste,
    selectall,
    undo,
    redo,
    reload,
    back,
    forward,
    print,
    openlink,
    copylink,
    openimage,
    copyimage,
    copyimageLink,
    openvideo,
    copyvideo
}