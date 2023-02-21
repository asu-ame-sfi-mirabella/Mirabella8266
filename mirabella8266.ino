#include "DHT.h"
#include <Arduino.h>
#include <Wire.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include "Adafruit_SHT31.h"
#include <BH1750.h>
#define DHTTYPE DHT11
#define dht_dpin D4
BH1750 lightMeter;
Adafruit_SHT31 sht31 = Adafruit_SHT31();

const char* ssid = "ASU-M";
const char* password = "12345678";

String macAddress;

String serverName = "http://44.242.66.1:8089/sendData";

unsigned long lastTime = 0;
// Timer set to 10 minutes (600000)
//unsigned long timerDelay = 600000;
// Set timer to 5 seconds (5000)
unsigned long timerDelay = 7000;


DHT dht(dht_dpin, DHTTYPE); 
float t;
float h;

void setup() {
  Serial.begin(115200); 

  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
 
  Serial.println("Timer set to 5 seconds (timerDelay variable), it will take 5 seconds before publishing the first reading.");
  dht.begin();
  Wire.begin(D2,D1);
  lightMeter.begin();
  Serial.println(F("BH1750 Test begin"));
  if (! sht31.begin(0x44))
{
Serial.println("Couldn't find SHT31");
while (1) delay(1);
}
macAddress = WiFi.macAddress();
Serial.println(macAddress);
  // put your setup code here, to run once:

}

void loop() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  float lux = lightMeter.readLightLevel();
  float t_ground = sht31.readTemperature();
  float h_ground = sht31.readHumidity();
  if (! isnan(t_ground))
{
Serial.print("ground_Temp *C = "); Serial.println(t_ground);
}
else
{
Serial.println("Failed to read temperature");
}
 
if (! isnan(h_ground))
{
Serial.print("ground_Hum. % = "); Serial.println(h_ground);
}
else
{
Serial.println("Failed to read humidity");
}
  Serial.print("Light: ");
  Serial.print(lux);
  Serial.println(" lx"); 
  Serial.println("Humidity and temperature\n\n");
  Serial.print("Current humidity = ");
  Serial.print(h);
  Serial.print("%  ");
  Serial.print("temperature = ");
  Serial.print(t);
  delay(2000);

  if ((millis() - lastTime) > timerDelay) {
    //Check WiFi connection status
    if(WiFi.status()== WL_CONNECTED){
      WiFiClient client;
      HTTPClient http;

      String serverPath = String(serverName) + "?mac="+ String(macAddress) +"&t="+ String(t)+"&h="+ String(h)+"&t_ground="+ String(t_ground)+"&h_ground="+String(h_ground)+"&lux="+String(lux);
      
      // Your Domain name with URL path or IP address with path
      http.begin(client, serverPath.c_str());
  
      // If you need Node-RED/server authentication, insert user and password below
      //http.setAuthorization("REPLACE_WITH_SERVER_USERNAME", "REPLACE_WITH_SERVER_PASSWORD");
        
      // Send HTTP GET request
      int httpResponseCode = http.GET();
      
      if (httpResponseCode>0) {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        String payload = http.getString();
        Serial.println(payload);
      }
      else {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
      }
      // Free resources
      http.end();
    }
    else {
      Serial.println("WiFi Disconnected");
    }

}
}
