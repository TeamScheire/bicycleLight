var mqtt = {
    defaultSettings: {
        host: 'cot-dyamand-dev.tengu.io',
        port: '8883',
        clientid: 'maaktnietuit',
        topic: 'deviceid'
    },
    settings: {
        host: 'test.mosquitto.org',
        port: '8080',
        clientid: 'bicycleTestClient2S',
        topic: 'bell/1'
    },
    connected: false,
    connectionOptions: {
        timeout: 3,
        onFailure: function (message) {
            debug.log("mqtt connection failed: " + message.errorMessage, 'error');
            mqtt.connected = false;
        }
    },
    initialize: function () {
        debug.log('Initialising mqtt ...');
        mqtt.loadSettings();

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
        debug.log("Mqtt client is connecting...");
        try {
            mqtt.client.connect(mqtt.connectionOptions);
            debug.log('Mqtt client connected', 'success');
        } catch (exception) {
            debug.log('Mqtt client connecting failed', 'error');
            console.log(exception);
        }
    },
    onConnected: function () {
        mqtt.connected = true;
        mqtt.toggleConnectionButtons();
        console.log("mqtt connection succesfull");
    },
    onConnectionLost: function (responseObject) {
        mqtt.connected = false;
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
    sendMessage: function (data) {
        payload = {
            type: "idlab-iot-ingest", // TODO: add ble device id
            entityId: app.user.userName,
            timestamp: moment().unix(),
            geoloc: {           
                lat: 0,
                lng: 0
            },
            payload: data
        };

        message = JSON.stringify(payload);
        message = new Paho.MQTT.Message(message);
        message.destinationName = mqtt.settings.topic;

        try {
            mqtt.client.send(message);
            debug.log('Mqtt Message sent', 'success');
            return true;
        } catch (exception) {
            debug.log('Mqtt sendMessage failed', 'error');
            console.log(exception);
            return false;
        }
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