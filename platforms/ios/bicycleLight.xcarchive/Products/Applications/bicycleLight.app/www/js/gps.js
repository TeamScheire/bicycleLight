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
            interval: 10000
        });
        backgroundGeolocation.start();
    },
    onBackgroundInitSuccess: function (location) {
        console.log('-----background location success:-----');
        console.log(location);
        gps.coords = {
            lat: location.latitude,
            lng: location.longitude,
            acc: location.accuracy
        }
    },
    onBackgroundInitFailure: function (error) {
        debug.log('background gps failure');
        console.log(error);
    },
    getLocation: function (onSuccessCallback) {
        if (backgroundGeolocation !== 'undefined') {
            backgroundGeolocation.getLocations(function (locations) {
                gps.coords = locations[locations.length - 1];
                gps.coords.lat = gps.coords.latitude;
                gps.coords.lng = gps.coords.longitude;
                gps.coords.acc = gps.coords.accuracy;
                gps.showLocation();

                if (typeof (onSuccessCallback) === 'function') {
                    onSuccessCallback();
                }
            }, function (error) {
                debug.log('background gps getLocations failure');
                console.log(error);
            });
        } else {
            if (typeof (onSuccessCallback) === 'function') {
                onSuccessCallback();
            }
        }
    },
    showLocation: function () {
        console.log('-----location showed:-----');
        console.log(gps.coords);
        $('#geolocation').html('Latitude: ' + gps.coords.latitude + '<br />' +
            'Longitude: ' + gps.coords.longitude + '<br />' +
            'Altitude: ' + gps.coords.altitude + '<br />' +
            'Accuracy: ' + gps.coords.accuracy + '<br />' +
            'Altitude Accuracy: ' + gps.coords.altitudeAccuracy + '<br />' +
            'Heading: ' + gps.coords.heading + '<br />' +
            'Speed: ' + gps.coords.speed + '<br />');
    }
}