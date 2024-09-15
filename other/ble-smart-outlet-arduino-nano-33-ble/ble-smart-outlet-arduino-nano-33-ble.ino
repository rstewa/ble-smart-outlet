/*
  Author: Ryan Stewart
  Created: 2/4/24
  Updated: 2/23/24
*/

#include <ArduinoBLE.h>

enum {
  STATE_OFF = 0,
  STATE_ON = 1,
};

const char *deviceServiceUuid = "628e460b-f5b0-4348-9e91-b9777c65bf11";
const char *deviceServiceCharacteristicUuid = "c1e4b52d-a02d-4dcd-9fa4-27dd90c52f03";

int state = -1;
int pinOut = 9;

BLEService smartOutletService(deviceServiceUuid);
BLEByteCharacteristic smartOutletCharacteristic(deviceServiceCharacteristicUuid,
                                                BLERead | BLEWrite | BLENotify);

void setup() {
  Serial.begin(9600);

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
  else {
    Serial.println("Successfully Started Bluetooth® Low Energy module!");
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