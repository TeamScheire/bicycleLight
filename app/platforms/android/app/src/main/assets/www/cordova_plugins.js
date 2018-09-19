cordova.define('cordova/plugin_list', function(require, exports, module) {
module.exports = [
  {
    "id": "cordova-plugin-background-fetch.BackgroundFetch",
    "file": "plugins/cordova-plugin-background-fetch/www/BackgroundFetch.js",
    "pluginId": "cordova-plugin-background-fetch",
    "clobbers": [
      "window.BackgroundFetch"
    ]
  },
  {
    "id": "cordova-background-geolocation-lt.BackgroundGeolocation",
    "file": "plugins/cordova-background-geolocation-lt/www/BackgroundGeolocation.js",
    "pluginId": "cordova-background-geolocation-lt",
    "clobbers": [
      "window.BackgroundGeolocation"
    ]
  },
  {
    "id": "cordova-background-geolocation-lt.API",
    "file": "plugins/cordova-background-geolocation-lt/www/API.js",
    "pluginId": "cordova-background-geolocation-lt"
  },
  {
    "id": "cordova-plugin-geolocation.geolocation",
    "file": "plugins/cordova-plugin-geolocation/www/android/geolocation.js",
    "pluginId": "cordova-plugin-geolocation",
    "clobbers": [
      "navigator.geolocation"
    ]
  },
  {
    "id": "cordova-plugin-geolocation.PositionError",
    "file": "plugins/cordova-plugin-geolocation/www/PositionError.js",
    "pluginId": "cordova-plugin-geolocation",
    "runs": true
  },
  {
    "id": "cordova-plugin-ble-central.ble",
    "file": "plugins/cordova-plugin-ble-central/www/ble.js",
    "pluginId": "cordova-plugin-ble-central",
    "clobbers": [
      "ble"
    ]
  }
];
module.exports.metadata = 
// TOP OF METADATA
{
  "cordova-plugin-background-fetch": "5.4.1",
  "cordova-background-geolocation-lt": "2.13.1",
  "cordova-plugin-cocoalumberjack": "0.0.4",
  "cordova-plugin-geolocation": "4.0.1",
  "cordova-plugin-whitelist": "1.3.3",
  "cordova-plugin-ble-central": "1.2.2"
};
// BOTTOM OF METADATA
});