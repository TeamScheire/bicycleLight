var gps = {
    coords: {},
    getLocation: function () {
        navigator.geolocation.getCurrentPosition(gps.onSuccess, app.onError);
    },
    onSuccess: function (position) {
        $('#geolocation').html('Latitude: ' + position.coords.latitude + '<br />' +
            'Longitude: ' + position.coords.longitude + '<br />' +
            'Altitude: ' + position.coords.altitude + '<br />' +
            'Accuracy: ' + position.coords.accuracy + '<br />' +
            'Altitude Accuracy: ' + position.coords.altitudeAccuracy + '<br />' +
            'Heading: ' + position.coords.heading + '<br />' +
            'Speed: ' + position.coords.speed + '<br />' +
            'Timestamp: ' + position.timestamp);

        gps.coords = position.coords;
    }
}