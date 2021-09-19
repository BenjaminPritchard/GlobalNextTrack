// ==UserScript==
// @name         GlobalNextTrack
// @namespace    http://tampermonkey.net/
// @version      0.1
// @description  Connect to local Websocket host, and listen for commands to advance to next song...
// @author       Benjamin Pritchard
// @match        https://cpl-nml3-naxosmusiclibrary-com.ezproxy2.cpl.org/* https://music.youtube.com/*
// @match        https://music.youtube.com/*
// @GitHub       https://github.com/BenjaminPritchard/GlobalNextTrack
// @grant        none
// ==/UserScript==

(function () {
  "use strict";

  // connect to local python websocket host
  let socket = new WebSocket("ws://localhost:9899");

  socket.onopen = function (e) {
    // do nothing for now
  };

  socket.onmessage = function (event) {
    // logic for naxos music library
    if (
      document.location.href.startsWith(
        "https://cpl-nml3-naxosmusiclibrary-com"
      )
    ) {
      if (event.data == "next")
        document.getElementsByClassName("u-play-btn next")[0].click();
      if (event.data == "prev")
        document.getElementsByClassName("u-play-btn prev")[0].click();
    }
    // logic for youtube music
    if (document.location.href.startsWith("https://music.youtube.com/")) {
      if (event.data == "next")
        document.getElementsByClassName("next-button")[0].click();
      if (event.data == "prev")
        document.getElementsByClassName("previous-button")[0].click();
    }
  };

  socket.onclose = function (event) {
    console.log("connection to python websocket server lost!");
  };

  // this probably happens when the server is not running
  socket.onerror = function (error) {
    console.log(`[error] ${error.message}`);
  };
})();
