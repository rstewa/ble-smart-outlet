#include <WiFi.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>

// Your WiFi credentials
const char* ssid = "";
const char* password = "";

// MQTT topics
const char* outTopic = "smart.outlet.state";
const char* inTopic = "smart.outlet.trigger";

// MQTT broker IP address
const char* mqtt_server = "";

// CA certificate, client certificate, and client key
const char* ca_cert = \
"-----BEGIN CERTIFICATE-----\n" \
"-----END CERTIFICATE-----\n";

const char* client_cert = \
"-----BEGIN CERTIFICATE-----\n" \
"-----END CERTIFICATE-----\n";

const char* client_key = \
"-----BEGIN PRIVATE KEY-----\n" \
"-----END PRIVATE KEY-----\n";

// Initialize the WiFi and MQTT client objects
WiFiClientSecure espClient;
PubSubClient client(espClient);

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("arduinoClient")) { // , "rstewa", "Lubricant7-Icing1-Anatomist6-Removal7-Slideshow1")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish(outTopic, "hello world");
      // ... and resubscribe
      client.subscribe(inTopic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  // convert payload to string
  char str[length + 1];
  memcpy(str, payload, length);
  str[length] = '\0';

  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("]: ");
  Serial.printf("'%s'\n", str);

  if (strcmp(str, "on") == 0) {
    Serial.println("the payload was 'on'");
  }
  else if (strcmp(str, "off") == 0) {
    Serial.println("the payload was 'off'");
  }
  else {
    Serial.println("the payload was invalid");
  }
}

void setup() {
  // Start the serial communication
  Serial.begin(115200);

  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Set the certificates
  espClient.setCACert(ca_cert);
  espClient.setCertificate(client_cert);
  espClient.setPrivateKey(client_key);

  // Set the MQTT server
  client.setServer(mqtt_server, 8883); // Use port 8883 for secure connection
  client.setCallback(callback);
}

void loop() {
  // Ensure the client is connected
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}