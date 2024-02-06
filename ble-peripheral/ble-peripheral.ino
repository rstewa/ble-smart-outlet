/*
 * Author: Ryan Stewart
 * Date: 2/4/24
 */

/*
  BLE_Peripheral.ino

  This program uses the ArduinoBLE library to set-up an Arduino Nano 33 BLE
  as a peripheral device and specifies a service and a characteristic. Depending
  of the value of the specified characteristic, an on-board LED gets on.

  The circuit:
  - Arduino Nano 33 BLE.

  This example code is in the public domain.
*/

#include <ArduinoBLE.h>

enum {
  STATE_OFF = 0,
  STATE_ON = 1,
};

const char *deviceServiceUuid = "19b10000-e8f2-537e-4f6c-d104768a1214";
const char *deviceServiceCharacteristicUuid =
    "19b10001-e8f2-537e-4f6c-d104768a1214";

int state = -1;

int pinOut = 9;

BLEService smartOutletService(deviceServiceUuid);
BLEByteCharacteristic smartOutletCharacteristic(deviceServiceCharacteristicUuid,
                                                BLERead | BLEWrite);

void setup() {
  Serial.begin(9600);
  while (!Serial)
    ;

  pinMode(pinOut, OUTPUT);

  pinMode(LEDR, OUTPUT);
  pinMode(LEDG, OUTPUT);

  digitalWrite(LEDR, LOW);
  digitalWrite(LEDG, HIGH);

  if (!BLE.begin()) {
    Serial.println("- Starting Bluetooth® Low Energy module failed!");
    while (1)
      ;
  }

  BLE.setDeviceName("ble-smart-outlet");
  BLE.setLocalName("ble-smart-outlet");
  BLE.setAdvertisedService(smartOutletService);
  smartOutletService.addCharacteristic(smartOutletCharacteristic);
  BLE.addService(smartOutletService);
  smartOutletCharacteristic.writeValue(0);
  BLE.advertise();

  Serial.print("Peripheral device MAC: ");
  Serial.println(BLE.address());

  Serial.println("Nano 33 BLE (Peripheral Device)");
  Serial.println(" ");
}

void loop() {
  BLEDevice central = BLE.central();
  // Serial.println("- Discovering central device...");
  // delay(500);

  if (central) {
    Serial.println("* Connected to central device!");
    Serial.print("* Device MAC address: ");
    Serial.println(central.address());
    Serial.println(" ");

    while (central.connected()) {
      if (smartOutletCharacteristic.written()) {
        Serial.println("* state characteristic was written to");
        state = smartOutletCharacteristic.value();
        writeState(state);
      }
    }

    Serial.println("* Disconnected to central device!");
  }
}

void writeState(int state) {
  Serial.println("- Characteristic <state> has changed!");
  Serial.print("State: ");
  Serial.println(state);

  switch (state) {
    case STATE_ON:
      Serial.println("* Actual value: ON (green LED on)");
      Serial.println(" ");
      digitalWrite(LEDR, HIGH);
      digitalWrite(LEDG, LOW);
      digitalWrite(pinOut, HIGH);
      break;
    case STATE_OFF:
      Serial.println("* Actual value: OFF (red LED on)");
      Serial.println(" ");
      digitalWrite(LEDR, LOW);
      digitalWrite(LEDG, HIGH);
      digitalWrite(pinOut, LOW);
      break;
    default:
      digitalWrite(LEDR, LOW);
      digitalWrite(LEDG, HIGH);
      digitalWrite(pinOut, LOW);
      break;
  }
}