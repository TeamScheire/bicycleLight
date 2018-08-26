var mqtt = {
    settings: {
        host: 'cot-dyamand-dev.tengu.io',
        port: '8883',
        clientid: 'maaktnietuit',
        topic: 'deviceid'
    },
    defaultSettings: {
        host: 'test.mosquitto.org',
        port: '8080',
        clientid: 'bicycleTestClient2S',
        topic: 'bell'
    },
    isConnecting: false,
    connected: false,
    connectionOptions: {
        timeout: 3,
        onFailure: function (message) {
            debug.log("mqtt connection failed: " + message.errorMessage, 'error');
            mqtt.connected = false;
        }
    },
    messages: [],
    initialize: function () {
        debug.log('Initialising mqtt ...');
        mqtt.loadSettings();

        mqtt.client = false;

        try {
            mqtt.client = new Paho.MQTT.Client(mqtt.settings.host, parseInt(mqtt.settings.port), mqtt.settings.clientid);
            mqtt.client.onConnectionLost = mqtt.onConnectionLost;
            mqtt.client.onConnected = mqtt.onConnected;
            //mqtt.client.onMessageArrived = mqtt.onMessageArrived;
            debug.log('Mqtt Initialised', 'success');
            mqtt.connect();
        } catch (exception) {
            debug.log('Mqtt Initialising failed', 'error');
            console.log(exception);
        }
    },
    connect: function () {
        if (!mqtt.isConnecting) {
            debug.log("Mqtt client is connecting...");
            mqtt.isConnecting = true;
            try {
                mqtt.client.connect(mqtt.connectionOptions);
                debug.log('Mqtt client connected', 'success');
            } catch (exception) {
                debug.log('Mqtt client connecting failed', 'error');
                mqtt.connected = false;
                mqtt.isConnecting = false;
                console.log(exception);
            }
        }
    },
    onConnected: function () {
        mqtt.connected = true;
        mqtt.isConnecting = false;
        mqtt.toggleConnectionButtons();
        console.log("mqtt connection succesfull");
    },
    onConnectionLost: function (responseObject) {
        mqtt.connected = false;
        mqtt.isConnecting = false;
        mqtt.toggleConnectionButtons();
        console.log("mqtt connection lost: " + responseObject.errorMessage);
    },
    disconnect: function () {
        debug.log("Mqtt client is disconnecting...");
        try {
            mqtt.client.disconnect();
            debug.log('Mqtt client disconnected', 'success');
        } catch (exception) {
            debug.log('Mqtt client disconnecting failed', 'error');
            console.log(exception);
        }
    },
    toggleConnectionButtons: function () {
        console.log('current mqtt connection status: ' + mqtt.connected);

        if (mqtt.connected) {
            $('.showOnMqttDisconnected').hide();
            $('.showOnMqttConnected').show();
        } else {
            $('.showOnMqttDisconnected').show();
            $('.showOnMqttConnected').hide();
        }
    },
    refreshSentMessageList: function () {
        $('#sentMessages').empty();

        if (mqtt.messages.length > 20) {
            mqtt.messages.shift();
        }

        $.each(mqtt.messages, function (index, data) {
            var messageLine = '<ons-list-item>' +
                '<span class="list-item__title">' + data.data + '</span>' +
                '<span class="list-item__subtitle" style="color: ' + ((data.status) ? 'green' : 'red') + ';">' + data.timestamp + '</span>' +
                '</ons-list-item>';

            $('#sentMessages').prepend(messageLine);
        });

    },
    addMessage: function(deviceId, data) {
        mqtt.sendMessage(data);
    },
    sendMessage: function (data) {
        gps.getLocation();
        var status = false;
        var sent = false;

        var userid = (app.user.userName !== undefined) ? app.user.userName : 'no-user';

        payload = {
            type: "idlab-iot-ingest", // TODO: add ble device id
            entityId: userid,
            timestamp: moment().unix(),
            geoloc: gps.coords,
            payload: data
        };

        message = JSON.stringify(payload);
        message = new Paho.MQTT.Message(message);
        message.destinationName = (bluetooth.connectDevice.id !== undefined) ? bluetooth.connectDevice.id : mqtt.settings.topic;

        try {
            mqtt.client.send(message);
            debug.log('Mqtt Message sent', 'success');
            sent = true;
            status = true;
        } catch (exception) {
            debug.log('Mqtt sendMessage failed', 'error');
            console.log(exception);
            sent = false;
            status = false;
        }

        mqtt.messages.push({
            data: data,
            timestamp: moment().format(),
            sent: sent,
            status: status
        });

        mqtt.refreshSentMessageList();

        return status;
    },

    loadSettings: function () {
        mqtt.settings = storage.getItem('mqttSettings', mqtt.defaultSettings);
        return mqtt.settings;
    },
    validateSettings: function (newSettings) {
        // XXX validate settings
        return true;
    },
    saveSettings: function (newSettings) {
        storage.setItem('mqttSettings', newSettings);
    }
}