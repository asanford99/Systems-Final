#include <WiFi.h>
#include <PubSubClient.h>

#define PIN_LED 2

// WiFi
const char *ssid = "Psalm23"; // Enter your WiFi name
const char *password = "maga2016";  // Enter WiFi password

// MQTT Broker
const char *mqtt_broker = "192.168.50.117";
const char *topic = "esp32/test";
const int mqtt_port = 1883;

//Program Variales

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
 pinMode(PIN_LED, OUTPUT);
  
 // Set software serial baud to 115200;
 Serial.begin(115200);
 // connecting to a WiFi network
 WiFi.begin(ssid, password);
 while (WiFi.status() != WL_CONNECTED) {
     delay(500);
     Serial.println("Connecting to WiFi..");
 }
 Serial.println("Connected to the WiFi network");
 //connecting to a mqtt broker
 client.setServer(mqtt_broker, mqtt_port);
 client.setCallback(callback);
 while (!client.connected()) {
     String client_id = "esp32-client-";
     client_id += String(WiFi.macAddress());
     Serial.printf("The client %s connects to the public mqtt broker\n", client_id.c_str());
     if (client.connect(client_id.c_str())) {
         Serial.println("Public emqx mqtt broker connected");
     } else {
         Serial.print("failed with state ");
         Serial.print(client.state());
         delay(2000);
     }
 }
 // publish and subscribe
 client.publish(topic, "Hi EMQX I'm ESP32 ^^");
 client.subscribe(topic);
}

void publishStatus (int status){
  if (status == 1){
    client.publish(topic, "Turning light on.");
  }
  if (status == 2){
    client.publish(topic, "Turning light off.");
  }
}

void callback(char *topic, byte *payload, unsigned int length) {
 Serial.print("Message arrived in topic: ");
 Serial.println(topic);
 Serial.print("Message:");
 for (int i = 0; i < length; i++) {
     Serial.print((char) payload[i]);
 }
 if ((char) payload[0] == '1'){
   publishStatus(1);
   digitalWrite(PIN_LED, HIGH);
 }
 if ((char) payload[0] == '2'){
   publishStatus(2);
   digitalWrite(PIN_LED, LOW);
 }
 Serial.println();
 Serial.println("-----------------------");
}

void loop() {
 client.loop();
}
