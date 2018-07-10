var app = {
    // Application Constructor
    initialize: function () {
        console.log('app initialize');
        ons.ready(function () {
            console.log('ready');
            //console.log(window.device);
            app.onDeviceReady();
        });
    },

    onDeviceReady: function () {
        app.bindEvents();
        //mqtt.initialize();
        //gps.getLocation();
        //bluetooth.initialize();
        debug.log('device ready', 'success');
    },

    bindEvents: function () {

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
    },

    loadUser: function() {

    },

    validateUser: function(newUser) {
        return newUser;
    },

    saveUser: function(newUser) {

    },

    loadSettings: function () {
        document.getElementById('settings-host').value = storage.getItem('host', 'test.mosquitto.org');
        document.getElementById('settings-port').value = storage.getItem('port', '8080');
        document.getElementById('settings-clientid').value = storage.getItem('clientid', 'bicycleTestClient');
        document.getElementById('settings-topic').value = storage.getItem('topic', 'bell/1');
    },

    validateSettings: function (newSettings) {
        // XXX validate settings
        return true;
    },
    
    saveSettings: function (newSettings) {
        storage.setItem('host', newSettings.host);
        storage.setItem('port', newSettings.port);
        storage.setItem('clientid', newSettings.clientid);
        storage.setItem('topic', newSettings.topic);
        debug.log('new settings: ' + JSON.stringify(newSettings), 'success');
    }
};

app.initialize();