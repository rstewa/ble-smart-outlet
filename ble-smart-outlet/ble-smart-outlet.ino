/* Author: Ryan Stewart
 * Date: 2/7/24
*/

#include <BLE2902.h> // this include is needed to force the compiler to find the ESP32 BLE library instead of the ArduinoBLE library
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

#define SERVICE_NAME "BLE Smart Outlet"
#define SERVICE_UUID "bfa7820b-1d87-49ed-8908-82b0e347ca56"
#define CHARACTERISTIC_UUID "9ffa81aa-bb6e-4254-8f3a-94b0154eea50"

const uint8_t STATE_OFF = 0;
const uint8_t STATE_ON = 1;
const int relayPin = 7;

uint8_t state = STATE_OFF;
int deviceConnected = false;

class MyServerCallbacks : public BLEServerCallbacks
{
  void onConnect(BLEServer *pServer)
  {
    deviceConnected = true;
    Serial.println("Connected to a central device.");
  };

  void onDisconnect(BLEServer *pServer)
  {
    deviceConnected = false;
    Serial.println("Disconnected from a central device.");
    delay(500);                  // give the bluetooth stack the chance to get things ready
    pServer->startAdvertising(); // restart advertising
    Serial.println("start advertising");
  }
};

class StateCharacteristicCallback : public BLECharacteristicCallbacks
{
    void onWrite(BLECharacteristic *pCharacteristic)
    {
        std::string tmp = pCharacteristic->getValue();
        const uint8_t *value = reinterpret_cast<const uint8_t*>(tmp.c_str());
        
        if (memcmp(value, &STATE_OFF, 1) == 0)
        {
            state = STATE_OFF;
            digitalWrite(relayPin, LOW);
            Serial.println("Turning off");
        }
        else if (memcmp(value, &STATE_ON, 1) == 0)
        {
            state = STATE_ON;
            digitalWrite(relayPin, HIGH);
            Serial.println("Turning on");
        }
        else
        {
            Serial.println("Invalid value");
        }
    }
};

void setup()
{
    Serial.begin(115200);

    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(relayPin, OUTPUT);

    digitalWrite(LED_BUILTIN, LOW);
    digitalWrite(relayPin, LOW);

    // create BLE server

    BLEDevice::init(SERVICE_NAME);
    BLEServer *pServer = BLEDevice::createServer();

    pServer->setCallbacks(new MyServerCallbacks());

    BLEService *pService = pServer->createService(SERVICE_UUID);

    BLECharacteristic *stateCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);

    stateCharacteristic->setCallbacks(new StateCharacteristicCallback());

    stateCharacteristic->setValue(&state, 1);
    pService->start();

    BLEAdvertising *pAdvertising = pServer->getAdvertising();
    pAdvertising->start();
}

void loop()
{
    if (deviceConnected)
    {
        digitalWrite(LED_BUILTIN, HIGH);
    }
    else
    {
        digitalWrite(LED_BUILTIN, LOW);
    }
}