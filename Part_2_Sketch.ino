#define PIN_LED 2
#define trigPin 33
#define echoPin 14
#define MAX_DISTANCE 700

#include <WiFi.h>
#include <PubSubClient.h>

int timeOut = MAX_DISTANCE * 60;
int soundVelocity = 340;

// WiFi
const char *ssid = "Psalm23"; // Enter your WiFi name
const char *password = "maga2016";  // Enter WiFi password

// MQTT Broker
const char *mqtt_broker = "192.168.50.117";
const char *topic_sub = "esp32/start";
const char *topic_pub = "esp32/position";
const int mqtt_port = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
 // Set software serial baud to 115200;
 Serial.begin(115200);

 pinMode(PIN_LED, OUTPUT);
 pinMode(trigPin, OUTPUT);
 pinMode(echoPin, INPUT);
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
 client.publish(topic_pub, "Hi EMQX I'm ESP32 ^^");
 client.subscribe(topic_sub);
 client.subscribe(topic_pub);
}

int getSonar() {
 unsigned long pingTime;
 float distance;
 // make trigPin output high level lasting for 10μs to triger HC_SR04？
 digitalWrite(PIN_LED, LOW);
 digitalWrite(trigPin, HIGH); 
 delayMicroseconds(10);
 digitalWrite(trigPin, LOW);
 // Wait HC-SR04 returning to the high level and measure out this waitting time
 pingTime = pulseIn(echoPin, HIGH, timeOut); 
 // calculate the distance according to the time
 distance = (float)pingTime * soundVelocity / 2 / 10000; 
 return (int)distance; // return the distance value
}

void measureDistance(){
  client.publish(topic_pub, String(getSonar()).c_str());
}

void callback(char *topic, byte *payload, unsigned int length) {
 Serial.print("Message arrived in topic: ");
 Serial.println(topic);
 Serial.print("Message:");
 for (int i = 0; i < length; i++) {
     Serial.print((char) payload[i]);
 }
 if ((char) payload[0] == '!'){
   digitalWrite(PIN_LED, HIGH);
   delay(250);
   measureDistance();
 }
 Serial.println();
 Serial.println("-----------------------");
}

void loop() {
 client.loop();
}
