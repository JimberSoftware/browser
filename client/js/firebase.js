"use strict";
  
  // Initialize Firebase
  var config = {
    apiKey: "AIzaSyDVKLjBpZjQwEhZ-yY-2FFm_M6grw57cDg",
    authDomain: "brokerdemo-8acc3.firebaseapp.com",
    databaseURL: "https://brokerdemo-8acc3.firebaseio.com",
    projectId: "brokerdemo-8acc3",
    storageBucket: "brokerdemo-8acc3.appspot.com",
    messagingSenderId: "722124796660"
  };
  firebase.initializeApp(config);
  
  
  // Initialize Cloud Firestore through Firebase
var _db = firebase.firestore();

// Disable deprecated features
_db.settings({
    // timestampsInSnapshots: true
  });
  
