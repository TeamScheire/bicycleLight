var gps = {
    coords: {
        lat: 0,
        lng: 0,
        acc: 0
    },
    initialize: function () {
        /*
        backgroundGeolocation.configure(gps.onBackgroundInitSuccess, gps.onBackgroundInitFailure, {
            desiredAccuracy: 10,
            stationaryRadius: 20,
            distanceFilter: 30,
            interval: 10000
        });
       
        backgroundGeolocation.start();
        */

        gps.bgGeoLocation = window.BackgroundGeolocation;
        gps.bgGeoLocation.on('location', gps.onLocation, gps.onLocationfailure);

        // Fired whenever state changes from moving->stationary or vice-versa.
        gps.bgGeoLocation.on('motionchange', function (isMoving) {
            console.log('- onMotionChange: ', isMoving);
        });

        gps.bgGeoLocation.ready({
            desiredAccuracy: 0,
            distanceFilter: 5,
            stationaryRadius: 10
        }, function (state) {
            console.log('- = - = - BackgroundGeolocation is configured and ready to use');
            //console.log(state);
            //if (!state.enabled) {
                gps.bgGeoLocation.start(function () {
                    console.log('- = - = - BackgroundGeolocation tracking started');
                });
            //}
        });
        //gps.bgGeoLocation.start();
    },
    onLocation: function (location) {
        gps.coords = location.coords;
        gps.coords.timestamp = location.timestamp;
        gps.coords.battery = location.battery;
        gps.showLocation();
        console.log('- Location: ', JSON.stringify(location));
    },
    onLocationfailure: function (errorCode) {
        console.warn('- BackgroundGeoLocation error: ', errorCode);
    },
    /*onBackgroundInitSuccess: function (location) {
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
    },*/
    getLocation: function (onSuccessCallback) {
        if (backgroundGeolocation !== 'undefined') {
            backgroundGeolocation.getLocations(function (locations) {
                console.log('---location---');
                console.log(locations[locations.length - 1]);
                gps.coords = locations[locations.length - 1];
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