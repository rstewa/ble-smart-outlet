/* 
 * Author: Ryan Stewart
 * Created: 2/23/24
 * Updated: n/a
*/

#include <BLE2902.h>  // this include is needed to force the compiler to find the ESP32 BLE library instead of the ArduinoBLE library
#include <BLEDevice.h>
#include <ezButton.h>
// #include "BLEScan.h"

// The remote service we wish to connect to.
static BLEUUID serviceUUID("628e460b-f5b0-4348-9e91-b9777c65bf11");
// The characteristic of the remote service we are interested in.
static BLEUUID charUUID("c1e4b52d-a02d-4dcd-9fa4-27dd90c52f03");

static boolean doConnect = false;
static boolean connected = false;
static boolean doScan = false;
static BLERemoteCharacteristic* pRemoteCharacteristic;
static BLEAdvertisedDevice* myDevice;

const uint8_t STATE_OFF = 0;
const uint8_t STATE_ON = 1;

bool current_toggle_status = LOW;  // start assuming switch switch is off
#define debounce 10                // debounce period  in milli seconds
#define LED LED_BUILTIN            // normally pin 13, but  this ensures it is set correctly by the compiler
int toggle_switch_pin = 4;         // GPIO4
int state = -1;

ezButton toggleSwitch(4);

//
// test the toggle  switch to see if its status has changed since last look.
// note that, although,  switch status is a returned value from the function,
// the current status of  the switch 'current_toggle_status' is always maintained
// and can be tested  outside of the function at any point/time.
//
bool read_toggle_switch() {
  static long int elapse_time = 0;
  static bool transition_started = false;
  int pin_value;
  pin_value = digitalRead(toggle_switch_pin);  // test current  state of toggle pin

  if (pin_value != current_toggle_status && !transition_started) {
    // switch change detected  so start debounce cycle
    transition_started = true;
    elapse_time = millis();  // set start of debounce timer
  } else {
    if (transition_started) {
      // we are in the switch transition cycle so check if debounce period has elapsed
      if (millis() - elapse_time >= debounce) {
        // debounce period elapse so assume switch has settled down after transition
        Serial.printf("current toggle switch status: %s\n", current_toggle_status == LOW ? "LOW" : "HIGH");
        current_toggle_status = !current_toggle_status;  // flip status
        transition_started = false;                      // cease transition cycle
      }
    }
  }
  return current_toggle_status;
}  // end of read_toggle_switch

static void notifyCallback(
  BLERemoteCharacteristic* pBLERemoteCharacteristic,
  uint8_t* pData,
  size_t length,
  bool isNotify) {
  Serial.print("Notify callback for characteristic ");

  Serial.print(pBLERemoteCharacteristic->getUUID().toString().c_str());

  std::string tmp = pRemoteCharacteristic->readValue();
  const uint8_t* value = reinterpret_cast<const uint8_t*>(tmp.c_str());

  if (memcmp(value, &STATE_OFF, 1) == 0) {
    state = STATE_OFF;
    Serial.println("Initial characteristic value was STATE_OFF");
  } else if (memcmp(value, &STATE_ON, 1) == 0) {
    state = STATE_ON;
    Serial.println("Initial characteristic value was STATE_ON");
  } else {
    Serial.println("Invalid value");
  }
}

class MyClientCallback : public BLEClientCallbacks {
  void onConnect(BLEClient* pclient) {
  }

  void onDisconnect(BLEClient* pclient) {
    connected = false;
    Serial.println("onDisconnect");
  }
};

bool connectToServer() {
  Serial.print("Forming a connection to ");
  Serial.println(myDevice->getAddress().toString().c_str());

  BLEClient* pClient = BLEDevice::createClient();
  Serial.println(" - Created client");

  pClient->setClientCallbacks(new MyClientCallback());

  // Connect to the remove BLE Server.
  pClient->connect(myDevice);  // if you pass BLEAdvertisedDevice instead of address, it will be recognized type of peer device address (public or private)
  Serial.println(" - Connected to server");
  pClient->setMTU(517);  //set client to request maximum MTU from server (default is 23 otherwise)

  // Obtain a reference to the service we are after in the remote BLE server.
  BLERemoteService* pRemoteService = pClient->getService(serviceUUID);
  if (pRemoteService == nullptr) {
    Serial.print("Failed to find our service UUID: ");
    Serial.println(serviceUUID.toString().c_str());
    pClient->disconnect();
    return false;
  }
  Serial.println(" - Found our service");

  // Obtain a reference to the characteristic in the service of the remote BLE server.
  pRemoteCharacteristic = pRemoteService->getCharacteristic(charUUID);
  if (pRemoteCharacteristic == nullptr) {
    Serial.print("Failed to find our characteristic UUID: ");
    Serial.println(charUUID.toString().c_str());
    pClient->disconnect();
    return false;
  }
  Serial.println(" - Found our characteristic");

  // Read the value of the characteristic.
  if (pRemoteCharacteristic->canRead()) {
    std::string tmp = pRemoteCharacteristic->readValue();
    const uint8_t* value = reinterpret_cast<const uint8_t*>(tmp.c_str());

    if (memcmp(value, &STATE_OFF, 1) == 0) {
      state = STATE_OFF;
      Serial.println("Initial characteristic value was STATE_OFF");
    } else if (memcmp(value, &STATE_ON, 1) == 0) {
      state = STATE_ON;
      Serial.println("Initial characteristic value was STATE_ON");
    } else {
      Serial.println("Invalid value");
    }
  }

  if (pRemoteCharacteristic->canNotify())
    pRemoteCharacteristic->registerForNotify(notifyCallback);

  connected = true;
  return true;
}
/**
 * Scan for BLE servers and find the first one that advertises the service we are looking for.
 */
class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
  /**
   * Called for each advertising BLE server.
   */
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    Serial.print("BLE Advertised Device found: ");
    Serial.println(advertisedDevice.toString().c_str());
    if (advertisedDevice.haveServiceUUID()) {
      Serial.printf("service uuid: %s\n", advertisedDevice.getServiceUUID().toString().c_str());
    }

    // We have found a device, let us now see if it contains the service we are looking for.
    if (advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(serviceUUID)) {

      BLEDevice::getScan()->stop();
      myDevice = new BLEAdvertisedDevice(advertisedDevice);
      doConnect = true;
      doScan = true;

    }  // Found our server
  }    // onResult
};     // MyAdvertisedDeviceCallbacks


void setup() {
  Serial.begin(115200);

  // pinMode(toggle_switch_pin, INPUT_PULLUP);
  toggleSwitch.setDebounceTime(50);  // set debounce time to 50 milliseconds

  Serial.println("Starting Arduino BLE Client application...");
  BLEDevice::init("Client: BLE Smart Outlet v2");

  // Retrieve a Scanner and set the callback we want to use to be informed when we
  // have detected a new device.  Specify that we want active scanning and start the
  // scan to run for 5 seconds.
  BLEScan* pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setInterval(1349);
  pBLEScan->setWindow(449);
  pBLEScan->setActiveScan(true);
  pBLEScan->start(10, false);
}  // End of setup.


// This is the Arduino main loop function.
void loop() {
  // read_toggle_switch();
  // int pin_value = digitalRead(toggle_switch_pin);
  toggleSwitch.loop();

  // If the flag "doConnect" is true then we have scanned for and found the desired
  // BLE Server with which we wish to connect.  Now we connect to it.  Once we are
  // connected we set the connected flag to be true.
  if (doConnect == true) {
    if (connectToServer()) {
      Serial.println("We are now connected to the BLE Server.");
    } else {
      Serial.println("We have failed to connect to the server; there is nothin more we will do.");
    }
    doConnect = false;
  }

  // If we are connected to a peer BLE Server, update the characteristic each time we are reached
  // with the current time since boot.
  if (connected) {
    if (toggleSwitch.isPressed())
      Serial.println("The switch: OFF -> ON");

    if (toggleSwitch.isReleased())
      Serial.println("The switch: ON -> OFF");
      
    int tmpState = toggleSwitch.getState();

    if (tmpState == LOW && state == STATE_OFF) {
      Serial.println("toggle status was low and state was off");
      uint8_t writeOnValue = 0x1;
      pRemoteCharacteristic->writeValue(writeOnValue, 1);
      state = STATE_ON;
    } else if (tmpState == HIGH && state == STATE_ON) {
      uint8_t writeOffValue = 0x0;
      pRemoteCharacteristic->writeValue(writeOffValue, 1);
      Serial.println("toggle status was high and state was on");
      state = STATE_OFF;
    }
  } else if (doScan) {
    BLEDevice::getScan()->start(0);  // this is just example to start scan after disconnect, most likely there is better way to do it in arduino
  }

  // delay(1000);  // Delay a second between loops.
}  // End of loop