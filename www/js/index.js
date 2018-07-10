var app = {
    defaultMqtt: {
        host: 'test.mosquitto.org',
        port: '8080',
        clientid: 'bicycleTestClient',
        topic: 'bell/1'
    },
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
            //mqtt.addMessage('app,1');
        }, 3000);

        document.addEventListener("pause", app.onDevicePause, false);
        document.addEventListener("resume", app.onDeviceResume, false);
        document.addEventListener("menubutton", app.onMenuKeyDown, false);

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

    onDevicePause: function () {
        debug.log('in pause');
        mqtt.addMessage('app,2');
    },
    onDeviceResume: function () {
        debug.log('out of pause');
        mqtt.addMessage('app,3');
    },
    onMenuKeyDown: function () {
        debug.log('menubuttonpressed');
        mqtt.addMessage('app,4');
    },
    onError: function (error) {
        debug.log(JSON.stringify(error), 'error');
    },

    loadUser: function () {
        return storage.getItem('user');
    },
    validateUser: function (newUser) {
        // XXX make api call to verify token
        newUser.userName = 'testuser01';
        return newUser;
    },
    saveUser: function (newUser) {
        storage.setItem('user', newUser);
    },

    loadSettings: function () {
        return storage.getItem('mqttSettings', app.defaultMqtt);
    },
    validateSettings: function (newSettings) {
        // XXX validate settings
        return true;
    },
    saveSettings: function (newSettings) {
        storage.setItem('mqttSettings', newSettings);
    }
};

app.initialize();