var mqtt = {
    settings: {
        host: 'production.dyamand.tengu.io',
        port: '8883',
        clientid: 'blf.smartbicycle',
        topic: 'deviceid'
    },
    defaultSettings: {
        host: 'test.mosquitto.org',
        port: '8080',
        clientid: 'bicycleTestClient',
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
    messageQueue: [],
    messages: [],
    messagesLength: 20,
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
        mqtt.sendMessageQueue();
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

        if (mqtt.messages.length > mqtt.messagesLength) {
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
    addMessage: function (data) {
        gps.getLocation(function() {
            var userId = (app.user.userName !== undefined) ? app.user.userName : 'nouser';
            var deviceId = (bluetooth.connectedDevice.id !== undefined) ? bluetooth.connectedDevice.id : 'nodevice';
            
            var payload = {
                type: "idlab-iot-ingest",
                entityId: 'blf.' + userId,
                deviceId: deviceId,
                timestamp: moment().unix(),
                geoloc: gps.coords,
                payload: data
            };
            
            mqtt.messageQueue.push(payload);
            
            if (mqtt.connected) {
                mqtt.sendMessageQueue();
            } else {
                mqtt.connect();
            }
        });
    },
    sendMessage(payload) {
        try {
            console.log("----payload---");
            console.log(payload);
            message = JSON.stringify(payload);
            message = new Paho.MQTT.Message(message);
            message.destinationName = payload.deviceId;

            mqtt.client.send(message);
            debug.log('Mqtt Message sent', 'success');

            mqtt.messages.push({
                data: payload.payload,
                timestamp: moment().format(),
                status: true
            });
            if (mqtt.messages.length > mqtt.messagesLength) {
                mqtt.messages.shift();
            }
            return true;
        } catch (exception) {
            debug.log('Mqtt send Message failed', 'error');
            console.log(exception);
            return false;
        }
    },
    sendMessageQueue: function () {
        if (mqtt.connected) {
            $.each(mqtt.messageQueue, function (index, payload) {
                if (mqtt.sendMessage(payload)) {
                    mqtt.messageQueue.splice(index, 1);
                }
            });
        }
        mqtt.refreshSentMessageList();
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