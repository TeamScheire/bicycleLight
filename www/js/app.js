var app = {
    user: {},
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
        app.loadUser();
        mqtt.initialize();
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
        app.user = storage.getItem('user');
    },
    validateUser: function (newUser) {
        // TODO make api call to verify token
        newUser.userName = 'testuser01';
        return newUser;
    },
    saveUser: function (newUser) {
        storage.setItem('user', newUser);
    },
    checkIfUserLoggedIn() {
        app.loadUser();

        if (app.user) {
            $('.logged-in').show();
            $('.logged-out').hide();
            $('.username').html(app.user.userName);
        } else {
            $('.logged-out').show();
            $('.logged-in').hide();
        }
    }
};

app.initialize();