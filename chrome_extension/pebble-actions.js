/*
This file loads any interactions provided by the user and manages their local controls when they've chosen them
*/
console.log("Extension loaded.");

Firebase.INTERNAL.forceWebSockets();
var fb = new Firebase('https://buzzkill2.firebaseIO.com');

var updateFireWithCurrentSite = function() {
  var buzzsites = ["facebook", "twitter", "buzzfeed", "reddit", "tumblr", "9gag", "myspace", "cracked", "xkcd"];
  var is_on_buzzsite = false;
  var website_on = "";
 
  for(var i = 0; i < buzzsites.length; i++) {
    var current_site = window.location.href;
    console.log('here ' + current_site);
    if(current_site.indexOf(buzzsites[i]) > -1){
      is_on_buzzsite = true;
      website_on = buzzsites[i] + " is on";
      fb.set({'site': website_on});
      console.log(website_on);
      break;
    }
    else {
      fb.set({'site': "good to go"});
      console.log("GTG");
    }
  }
}

updateFireWithCurrentSite();