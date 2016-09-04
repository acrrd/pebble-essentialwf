var Clay = require('pebble-clay');
var clayConfig = require('./config.json');
var clay = new Clay(clayConfig, null, { autoHandleEvents: false });

Pebble.addEventListener('showConfiguration', function(e) {
  Pebble.openURL(clay.generateUrl());
});

Pebble.addEventListener('webviewclosed', function(e) {
  if (e && !e.response) {
    return;
  }

  // Get the keys and values from each config item
  var settings = clay.getSettings(e.response, false);

  settings.DATE_FORMAT = parseInt(settings.DATE_FORMAT.value);
  settings.DAY_FORMAT = parseInt(settings.DAY_FORMAT.value);

  settings = Clay.prepareSettingsForAppMessage(settings);

  // Send settings values to watch side
  Pebble.sendAppMessage(settings, function(e) {
    console.log('Sent config data to Pebble');
  }, function(e) {
    console.log('Failed to send config data!');
    console.log(JSON.stringify(e));
  });
});



