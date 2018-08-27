var gps = {
    coords: {
        lat: 0,
        lng: 0,
        acc: 0
    },
    initialize: function () {
        backgroundGeolocation.configure(gps.onBackgroundInitSuccess, gps.onBackgroundInitFailure, {
            desiredAccuracy: 10,
            stationaryRadius: 20,
            distanceFilter: 30,
            interval: 60000
        });

        backgroundGeolocation.start();
    },
    onBackgroundInitSuccess: function (location) {
        gps.coords = {
            lat: location.latitude,
            lng: location.longitude,
            acc: location.accuracy
        }
        debug.log(gps.coords);
    },
    onBackgroundInitFailure: function (error) {
        debug.log('background gps failure');
        console.log(error);
    },
    getLocation: function () {
        try {
            navigator.geolocation.getCurrentPosition(gps.onSuccess, app.onError);
        } catch (exception) {
            debug.log('gps fetch failed', 'error');
            console.log(exception);
        }
    },
    onSuccess: function (position) {
        console.log(position.coords);
        $('#geolocation').html('Latitude: ' + position.coords.latitude + '<br />' +
            'Longitude: ' + position.coords.longitude + '<br />' +
            'Altitude: ' + position.coords.altitude + '<br />' +
            'Accuracy: ' + position.coords.accuracy + '<br />' +
            'Altitude Accuracy: ' + position.coords.altitudeAccuracy + '<br />' +
            'Heading: ' + position.coords.heading + '<br />' +
            'Speed: ' + position.coords.speed + '<br />');

        gps.coords = {
            lat: position.coords.latitude,
            lng: position.coords.longitude,
            acc: position.coords.accuracy
        }
    }
}