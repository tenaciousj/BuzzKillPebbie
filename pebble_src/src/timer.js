Pebble.addEventListener('ready',
  function(e) {
    console.log('JavaScript app ready and running!');
  }
);

Firebase.INTERNAL.forceWebSockets();
var fb = new Firebase('https://buzzkill2.firebaseio.com/');

fb.on('value', function(snapshot) {
  var data = snapshot.val();
  console.log(data['site']);
  
  Pebble.sendAppMessage({
    'site': data['site']
  });
});