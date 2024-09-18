#include <WiFi.h>
#include <PubSubClient.h>
#include <string>

const char* ssid = "";
const char* password = "";

// MQTT topics
const char* outTopic = "smart.outlet.state";
const char* inTopic = "smart.outlet.trigger";

// MQTT broker IP address
const char* mqtt_server = "";

// Initialize the WiFi and MQTT client objects
WiFiClient espClient;
PubSubClient client(espClient);

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

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("arduinoClient", "rstewa", "Lubricant7-Icing1-Anatomist6-Removal7-Slideshow1")) {
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

void setup() {
  Serial.begin(115200);

  // Connect to WiFi network
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print("Connecting to WiFi...");
  }

  Serial.println("connected");

  // Set the MQTT broker server IP address and port
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  // allow the hardware to sort itself out
  delay(1500);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}