var mqttclient = {
    settings: {
        host: 'production.dyamand.tengu.io',
        port: '8883',
        protocolId: 'TCP'
    },
    defaultSettings: {
        host: 'becme.idlab.uantwerpen.be',
        port: '9001',
        protocolId: 'MQTT'
    },
    client: false,
    isConnecting: false,
    connected: false,
    messageQueue: [],
    messages: [],
    messagesLength: 20,
    initialize: function () {
        debug.log('Initialising mqtt ...');
        mqttclient.loadSettings();
        mqttclient.client = false;
        mqttclient.isConnecting = false;
        mqttclient.connect();
        debug.log('Mqtt Initialised', 'success');
    },
    connect: function () {
        if (!mqttclient.isConnecting) {
            debug.log("Mqtt client is connecting...");
            console.log(mqttclient.settings);
            mqttclient.isConnecting = true;

            try {
                mqttclient.settings.clientId = 'becme-' + parseInt(Math.random() * 100, 10);
                mqttclient.client = mqtt.connect(mqttclient.settings);

                mqttclient.client.on('connect', function () {
                    mqttclient.connected = true;
                    mqttclient.isConnecting = false;
                    debug.log('mqtt connected');
                    mqttclient.toggleConnectionButtons();
                    mqttclient.sendMessageQueue();
                })
            } catch (exception) {
                debug.log('Mqtt client connecting failed', 'error');
                mqtt.connected = false;
                mqtt.isConnecting = false;
                console.log(exception);
            }
        }
    },
    disconnect: function () {
        debug.log("Mqtt client is disconnecting...");
        try {
            mqttclient.client.end();
            mqttclient.connected = false;
            mqttclient.toggleConnectionButtons();
            debug.log('Mqtt client disconnected', 'success');
        } catch (exception) {
            debug.log('Mqtt client disconnecting failed', 'error');
            console.log(exception);
        }
    },
    addMessage: function (data) {
        var userId = (app.user !== undefined) ? app.user.userId : 'nouser';
        var deviceId = (bluetooth.connectedDevice.id !== undefined) ? bluetooth.connectedDevice.id : 'nodevice';

        gps.getLocation(function () {
            var payload = {
                timestamp: moment().unix(),
                geoloc: gps.coords,
                entityId: userId, //'blf.' + userId,
                deviceId: deviceId,
                payload: data
            };

            mqttclient.messageQueue.push(payload);
            console.log('-----message added-----');
            console.log(payload);

            if (mqttclient.connected) {
                mqttclient.sendMessageQueue();
            } else {
                mqttclient.connect();
            }
        });
    },
    sendMessageQueue: function () {
        if (mqttclient.connected) {
            $.each(mqttclient.messageQueue, function (index, payload) {
                if (mqttclient.sendMessage(payload)) {
                    mqttclient.messageQueue.splice(index, 1);
                }
            });
        }
        mqttclient.refreshSentMessageList();
    },
    sendMessage(payload) {
        try {
            var topic = 'becme'; // payload.payload.entityId

            console.log("----message to send---");
            console.log('topic: ' + topic);
            console.log(payload);

            mqttclient.client.publish(topic, JSON.stringify(payload));
            debug.log('Mqtt Message sent', 'success');

            mqttclient.messages.push({
                data: payload.payload,
                timestamp: moment().format(),
                status: true
            });
            if (mqttclient.messages.length > mqttclient.messagesLength) {
                mqttclient.messages.shift();
            }
            return true;
        } catch (exception) {
            debug.log('Mqtt sendMessage failed', 'error');
            console.log(exception);
            return false;
        }
    },
    loadSettings: function () {
        mqttclient.settings = storage.getItem('mqttSettings', mqttclient.defaultSettings);
        return mqttclient.settings;
    },
    validateSettings: function (newSettings) {
        if ((newSettings.protocolId != 'TCP') && (newSettings.protocolId != 'MQTT')) {
            newSettings.protocolId = 'MQTT';
        }
        return newSettings;
    },
    saveSettings: function (newSettings) {
        storage.setItem('mqttSettings', newSettings);
    },
    toggleConnectionButtons: function () {
        console.log('current mqtt connection status: ' + mqttclient.connected);

        if (mqttclient.connected) {
            $('.showOnMqttDisconnected').hide();
            $('.showOnMqttConnected').show();
        } else {
            $('.showOnMqttDisconnected').show();
            $('.showOnMqttConnected').hide();
        }
    },
    refreshSentMessageList: function () {
        $('#sentMessages').empty();

        if (mqttclient.messages.length > mqttclient.messagesLength) {
            mqttclient.messages.shift();
        }

        $.each(mqttclient.messages, function (index, data) {
            var messageLine = '<ons-list-item>' +
                '<span class="list-item__title">' + data.data + '</span>' +
                '<span class="list-item__subtitle" style="color: ' + ((data.status) ? 'green' : 'red') + ';">' + data.timestamp + '</span>' +
                '</ons-list-item>';

            $('#sentMessages').prepend(messageLine);
        });

    }
};