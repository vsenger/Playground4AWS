
void setup() {
  pinMode(12, INPUT_PULLUP);
  buttonNormalState = digitalRead(12);

  pinMode(13, OUTPUT);


  Wire.begin();
  Serial.begin(115200);

  // Try to initialize!
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    //while (1) {
    //  delay(10);
    //}
  }
  Serial.println("MPU6050 Found!");

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  Serial.print("Accelerometer range set to: ");
  switch (mpu.getAccelerometerRange()) {
    case MPU6050_RANGE_2_G:
      Serial.println("+-2G");
      break;
    case MPU6050_RANGE_4_G:
      Serial.println("+-4G");
      break;
    case MPU6050_RANGE_8_G:
      Serial.println("+-8G");
      break;
    case MPU6050_RANGE_16_G:
      Serial.println("+-16G");
      break;
  }
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  Serial.print("Gyro range set to: ");
  switch (mpu.getGyroRange()) {
    case MPU6050_RANGE_250_DEG:
      Serial.println("+- 250 deg/s");
      break;
    case MPU6050_RANGE_500_DEG:
      Serial.println("+- 500 deg/s");
      break;
    case MPU6050_RANGE_1000_DEG:
      Serial.println("+- 1000 deg/s");
      break;
    case MPU6050_RANGE_2000_DEG:
      Serial.println("+- 2000 deg/s");
      break;
  }

  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  Serial.print("Filter bandwidth set to: ");
  switch (mpu.getFilterBandwidth()) {
    case MPU6050_BAND_260_HZ:
      Serial.println("260 Hz");
      break;
    case MPU6050_BAND_184_HZ:
      Serial.println("184 Hz");
      break;
    case MPU6050_BAND_94_HZ:
      Serial.println("94 Hz");
      break;
    case MPU6050_BAND_44_HZ:
      Serial.println("44 Hz");
      break;
    case MPU6050_BAND_21_HZ:
      Serial.println("21 Hz");
      break;
    case MPU6050_BAND_10_HZ:
      Serial.println("10 Hz");
      break;
    case MPU6050_BAND_5_HZ:
      Serial.println("5 Hz");
      break;
  }
  Serial.println("");
  delay(100);

}

void loop()
{
  Serial.println("\n**** Sensor HTU21D ****");
  Serial.print("Humidity(%RH): ");
  Serial.println(SHT2x.GetHumidity());
  Serial.print(" Temperature(C): ");
  Serial.println(SHT2x.GetTemperature());
  Serial.print(" Dewpoint(C): ");
  Serial.println(SHT2x.GetDewPoint());
  Serial.println("***********************");

  Serial.print("\n**** Sensor MPU6050 ****");
  /* Get new sensor events with the readings */
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  /* Print out the values */
  Serial.print("Acceleration X: ");
  Serial.print(a.acceleration.x);
  Serial.print(", Y: ");
  Serial.print(a.acceleration.y);
  Serial.print(", Z: ");
  Serial.print(a.acceleration.z);
  Serial.println(" m/s^2");

  Serial.print("Rotation X: ");
  Serial.print(g.gyro.x);
  Serial.print(", Y: ");
  Serial.print(g.gyro.y);
  Serial.print(", Z: ");
  Serial.print(g.gyro.z);
  Serial.println(" rad/s");

  Serial.print("Temperature: ");
  Serial.print(temp.temperature);
  Serial.println(" degC");

  Serial.print("***********************");


  if (digitalRead(12) != buttonNormalState) {
    Serial.println("\nBotão PRESSIONADO");
  } else {
    Serial.println("\nBotão LIBERADO");
  }
  //delay(500);

  //teste do rele
  digitalWrite(13, HIGH);
  delay(1000);
  digitalWrite(13, LOW);
}
#include <Wire.h>
#include <Sodaq_SHT2x.h>

#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>

#include <WiFiClientSecure.h>
#include <MQTTClient.h>

#include "secrets.h"

Adafruit_MPU6050 mpu;

int buttonNormalState ;

#include <ArduinoJson.h>
#define hostName "aws-amazing-device"

const char* ssid = "surfing-iot";
const char* password = "iotiotiot";

// The MQTT topics that this device should publish/subscribe
#define AWS_IOT_PUBLISH_SENSORS   "playground/sensors"
#define AWS_IOT_PUBLISH_BUTTON   "playground/button"
#define AWS_IOT_SUBSCRIBE_TOPIC "playground/lamp"

WiFiClientSecure net = WiFiClientSecure();
MQTTClient client = MQTTClient(256);

#define SECRET
#define THINGNAME "aws-device"

#define TABLE_NUMBER 10

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
  hdc1080.begin(0x40);
  Serial.print("Manufacturer ID=0x");
  Serial.println(hdc1080.readManufacturerId(), HEX); // 0x5449 ID of Texas Instruments
  Serial.print("Device ID=0x");
  Serial.println(hdc1080.readDeviceId(), HEX); // 0x1050 ID of the device
  attachInterrupt(IO_BUTTON, buttonISR, FALLING);
  connectWIFI();
  connectAWS();
}

double temperature, humidity;
unsigned long lastSensorsSending=0;
int volatile lastButtonTimeStamp=0, sendMessage=0;

void loop() {
  client.loop();
  temperature = hdc1080.readTemperature();
  humidity = hdc1080.readHumidity();  
  /*Serial.print("T=");
  Serial.print(hdc1080.readTemperature());
  Serial.print("C, RH=");
  Serial.print(hdc1080.readHumidity());
  Serial.println("%");
  Serial.print("Button=");
  Serial.println(digitalRead(23) ? "on" : "off");*/
  //sensors();
  
  if(millis() - lastSensorsSending>1000) {
    temperature = hdc1080.readTemperature();
    humidity = hdc1080.readHumidity();  
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
    doc["table_number"] = TABLE_NUMBER;
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
