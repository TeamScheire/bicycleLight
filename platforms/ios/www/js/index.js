ons.ready(function () {
    // Cordova APIs are ready
    console.log('ready');
    console.log(window.device);
});



var app = {
    // Application Constructor
    initialize: function () {
        console.log('app initialize');
        ons.ready(function () {
            // Cordova APIs are ready
            debug.log('ready', 'success');
            console.log('ready');
            //console.log(window.device);
            app.onDeviceReady();
        });
    },

    onDeviceReady: function () {
        app.receivedEvent('deviceready');
        app.bindEvents();
        //mqtt.initialize();
        //gps.getLocation();
        //bluetooth.initialize();
    },

    // Update DOM on a Received Event
    receivedEvent: function (id) {
        console.log('Received Event: ' + id);
    },

    bindEvents: function () {
        setTimeout(function() {
            debug.log('ready', 'success');
        }, 1000);

        setTimeout(function () {
            //mqtt.addMessage(bluetooth.connectedDevice.id, 'app,1');
        }, 3000);

        document.addEventListener("pause", this.onDevicePause, false);
        document.addEventListener("resume", this.onDeviceResume, false);
        document.addEventListener("menubutton", this.onMenuKeyDown, false);

        $('body').on('click', '#btnLogscreen', function (e) {
            debug.log('btnLogscreen clicked');
            document.getElementById('appSplitter').right.toggle();
        });
        
        /*
                $(document).on('click', '#refreshDeviceList', function (e) {
                    bluetooth.refreshDeviceList(false);
                });
                $('#deviceList').on('click', 'li', function (e) {
                    bluetooth.connectDevice($(this).attr("data-device-id"), $(this).attr("data-device-name"));
                });
                $(document).on('click', '#disconnectDevice', function (e) {
                    bluetooth.disconnectDevice(e);
                });
                */
    },

    onError: function (error) {
        debug.log(JSON.stringify(error), 'error');
    }
};

app.initialize();