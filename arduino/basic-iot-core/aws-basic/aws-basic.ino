#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>

#include <WiFiClientSecure.h>
#include <MQTTClient.h>

#include "secrets.h"

#include <ArduinoJson.h>


#define hostName "awsiotdevice"

const char* ssid = "surfing-iot";
const char* password = "iotiotiot";

// The MQTT topics that this device should publish/subscribe
#define AWS_IOT_PUBLISH_SENSORS   "playground/sensors"
#define AWS_IOT_PUBLISH_BUTTON   "callwaiter/button"
#define AWS_IOT_SUBSCRIBE_TOPIC "control/lamp"

WiFiClientSecure net = WiFiClientSecure();
MQTTClient client = MQTTClient(256);

#define SECRET
#define THINGNAME "letsbuild01"

#define IO_BUTTON 23
#define IO_RELAY 27

// the setup function runs once when you press reset or power the board
void setup() {
  Serial.begin(115200);
  //button setup
  pinMode(IO_BUTTON, INPUT_PULLUP);   //enable user button pull-up
  //relay
  pinMode(IO_RELAY, OUTPUT);

  // Default settings: 
  //  - Heater off
  //  - 14 bit Temperature and Humidity Measurement Resolutions
  attachInterrupt(IO_BUTTON, buttonISR, FALLING);
  connectWIFI();
  connectAWS();
}

double temperature, humidity;
unsigned long lastSensorsSending=0;
int volatile lastButtonTimeStamp=0, sendMessage=0;

void loop() {
  client.loop();
  /*Serial.print("T=");
  Serial.print(hdc1080.readTemperature());
  Serial.print("C, RH=");
  Serial.print(hdc1080.readHumidity());
  Serial.println("%");
  Serial.print("Button=");
  Serial.println(digitalRead(23) ? "on" : "off");*/
  //sensors();
  
  if(millis() - lastSensorsSending>1000) {

    sensors();
    lastSensorsSending = millis();
  }
  if(sendMessage) {
    sendMQTTButtonMessage();
  }

}

void buttonISR() {
  if(millis() - lastButtonTimeStamp>400) {
    Serial.println("publishing...");
    sendMessage=1;
    lastButtonTimeStamp=millis();
  }
}

void sendMQTTButtonMessage() {
    StaticJsonDocument<200> doc;    
    doc["device_time"] = millis();
    doc["device_name"] = THINGNAME;
    doc["table_number"] = 10;
    char jsonBuffer[128];
    serializeJson(doc, jsonBuffer); // print to client
    client.publish(AWS_IOT_PUBLISH_BUTTON, jsonBuffer);
    sendMessage=0;
}

void sensors() {
  StaticJsonDocument<200> doc;
  doc["device_time"] = millis();
  doc["device_name"] = THINGNAME;
  doc["humidity"] = humidity;
  doc["temperature"] = temperature;
  char jsonBuffer[128];
  serializeJson(doc, jsonBuffer); // print to client
  client.publish(AWS_IOT_PUBLISH_SENSORS, jsonBuffer);
}

void connectWIFI() {
  Serial.println("Booting");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}
