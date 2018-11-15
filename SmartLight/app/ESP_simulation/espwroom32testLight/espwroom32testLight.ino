/*
 * Code for a esp-wroom-32 to test and simulate the smart bicycle light
 * 
 * Connect a button on pin 19 to test the input of a notification.
 * 
 */
// using esp32 libraries from https://github.com/nkolban/ESP32_BLE_Arduino

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <BLEAdvertisedDevice.h>

#define buttonPin 19
uint8_t buttonStatus;

#define DEVICE_NAME "beCME"

BLEServer *pServer = NULL;
BLECharacteristic *pTxCharacteristic;
BLECharacteristic *pRxCharacteristic;
BLEAdvertising *pAdvertising;

//BLEDescriptor *pDescriptor;
bool deviceConnected = false;
bool deviceNotifying = true;
uint8_t messageId = 0;

#define SERVICE_UUID           "6E400001-B5A3-F393-E0A9-E50E24DCCA9E" // UART service UUID
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

// timing
unsigned long lastHeartbeat;
unsigned long heartbeatInterval = 5000;

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
    }
};

class MyCharacteristicCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string rxValue = pCharacteristic->getValue();

      if (rxValue.length() > 0) {
        Serial.println("*********");
        Serial.print("Received Value: ");
        for (int i = 0; i < rxValue.length(); i++) {
          Serial.print(rxValue[i]);
        }
        
        Serial.println();
        Serial.println("*********");
      }
    }
};

bool initBLE() {
  BLEDevice::init(DEVICE_NAME);

  // Create the BLE Server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create a BLE Characteristic
  pTxCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID_TX, 
                      BLECharacteristic::PROPERTY_NOTIFY
                    );

  pTxCharacteristic->addDescriptor(new BLE2902());

  pRxCharacteristic = pService->createCharacteristic(
                                         CHARACTERISTIC_UUID_RX,
                                         BLECharacteristic::PROPERTY_WRITE
                                       );

  pRxCharacteristic->setCallbacks(new MyCharacteristicCallbacks());
  //pDescriptor->setCallbacks(new MyDescriptorCallbacks());

  // Start the service
  pService->start();

  // Start advertising
  pAdvertising = pServer->getAdvertising();
  pAdvertising->addServiceUUID(BLEUUID(SERVICE_UUID));
  BLEAdvertisementData oScanResponseData = BLEAdvertisementData();
  pAdvertising->setScanResponseData(oScanResponseData);
  pAdvertising->start();

  return true;
}

void sendLightNotification(char* message) {
  if (deviceConnected && deviceNotifying) {
    Serial.printf("*** Sent Value: %d ***\n", messageId);
    pTxCharacteristic->setValue(message);
    pTxCharacteristic->notify();
    messageId++;
  }
}

void checkLightPush() {
  uint8_t newStatus = digitalRead(buttonPin);
  
  if ((buttonStatus == 0) && (newStatus == 1)) {
    buttonStatus = 1;
    Serial.println("pressed");
    sendLightNotification("testbutton,1");
  } else if (newStatus == 0) {
    buttonStatus = 0;
  }  
}

void setup() {
  Serial.begin(115200);
  pinMode(buttonPin, INPUT);

  if (!initBLE()) {
    Serial.println("Bluetooth init failed");
  }

  lastHeartbeat = millis();
  
  Serial.println("The device started, now you can pair it with bluetooth!");
}

void loop() {
  checkLightPush();

  if ((lastHeartbeat + heartbeatInterval) < millis()) {
    sendLightNotification("testheartbeat,1");
    lastHeartbeat = millis();
  }
  
  delay(20);
}
