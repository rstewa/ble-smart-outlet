/*
 *  This sketch demonstrates how to scan WiFi networks.
 *  The API is based on the Arduino WiFi Shield library, but has significant changes as newer WiFi functions are supported.
 *  E.g. the return value of `encryptionType()` different because more modern encryption is supported.
 */
#include "WiFi.h"

const bool DEBUG = true;
const char* ssid = "studentdebt";
const char* password = "nutx0b9zf8ihw7ik";

bool found_ssid = false;
bool connected = false;

void initWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println();
  Serial.println("Successfully connected to 'studentdebt'");
  Serial.println(WiFi.localIP());
  connected = true;
}

void setup() {
  Serial.begin(115200);

  // Set WiFi to station mode and disconnect from an AP if it was previously connected.
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  Serial.println("Setup done");
}

void loop() {
  if (!found_ssid) {
    Serial.println("Scan start");

    // WiFi.scanNetworks will return the number of networks found.
    int n = WiFi.scanNetworks();
    Serial.println("Scan done");

    if (n == 0) {
      Serial.println("no networks found");
    } else {
      Serial.print(n);
      Serial.println(" networks found");
      Serial.println("Nr | SSID                             | RSSI | CH | Encryption");
      for (int i = 0; i < n; ++i) {
        if (WiFi.SSID(i) == ssid) {
          found_ssid = true;
          printf("found studentdebt ssid\n");
        }

        // Print SSID and RSSI for each network found
        Serial.printf("%2d", i + 1);
        Serial.print(" | ");
        Serial.printf("%-32.32s", WiFi.SSID(i).c_str());
        Serial.print(" | ");
        Serial.printf("%4ld", WiFi.RSSI(i));
        Serial.print(" | ");
        Serial.printf("%2ld", WiFi.channel(i));
        Serial.print(" | ");
        switch (WiFi.encryptionType(i)) {
          case WIFI_AUTH_OPEN: Serial.print("open"); break;
          case WIFI_AUTH_WEP: Serial.print("WEP"); break;
          case WIFI_AUTH_WPA_PSK: Serial.print("WPA"); break;
          case WIFI_AUTH_WPA2_PSK: Serial.print("WPA2"); break;
          case WIFI_AUTH_WPA_WPA2_PSK: Serial.print("WPA+WPA2"); break;
          case WIFI_AUTH_WPA2_ENTERPRISE: Serial.print("WPA2-EAP"); break;
          case WIFI_AUTH_WPA3_PSK: Serial.print("WPA3"); break;
          case WIFI_AUTH_WPA2_WPA3_PSK: Serial.print("WPA2+WPA3"); break;
          case WIFI_AUTH_WAPI_PSK: Serial.print("WAPI"); break;
          default: Serial.print("unknown");
        }
        Serial.println();
        delay(10);
      }

      Serial.println("");
    }

    // Delete the scan result to free memory for code below.
    WiFi.scanDelete();

    // Wait a bit before scanning again.
    delay(5000);
  }
  else {
    if (!connected) {
      initWiFi();
    }
  }
}