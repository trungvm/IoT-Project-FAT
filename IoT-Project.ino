#define BLYNK_TEMPLATE_ID "TMPL6DKuhxuzw"
#define BLYNK_TEMPLATE_NAME "PhungTrinh"
#define BLYNK_AUTH_TOKEN "3oNk9uP3zr0VLvrfRqyS8-V9Mqml_x6l"

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <SoftwareSerial.h>
#include <string.h>
#include "DHT.h"

// Blynk configuration
char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "Fatlab";
char pass[] = "12345678@!";

// DHT configuration
#define DHTTYPE DHT22   // DHT 22
#define DHTPIN 14
DHT dht(DHTPIN, DHTTYPE);

// CO2 Sensor configuration
SoftwareSerial mySerial(16, 17); // RX, TX
unsigned char hexdata[9] = {0xFF,0x01,0x86,0x00,0x00,0x00,0x00,0x00,0x79}; //Read the gas density command /Don't change the order

void setup()
{
  Serial.begin(115200);
  dht.begin();
  mySerial.begin(9600);

  Blynk.begin(auth, ssid, pass);
}

void loop()
{
  Blynk.run();
  
  // Read Temp
  float t = dht.readTemperature();
  // Read Humi
  float h = dht.readHumidity();
  // Check isRead ?
  if (isnan(h) || isnan(t)) {
    delay(500);
    Serial.println("Failed to read from DHT sensor!\n");
    return;
  }

  // Read CO2
  mySerial.write(hexdata, 9);
  delay(500);
  long hi, lo, CO2;
  for (int i = 0; i < 9; i++) {
    if (mySerial.available() > 0) {
      int ch = mySerial.read();
      if (i == 2) {
        hi = ch;
      } else if (i == 3) {
        lo = ch;
      } else if (i == 8) {
        CO2 = hi * 256 + lo;
        Serial.print("CO2 concentration: ");
        Serial.print(CO2);
        Serial.println("ppm");
        Blynk.virtualWrite(V2, CO2); // Send CO2 data to Virtual Pin V2 in Blynk app
      }
    }
  }

  // Send temperature, humidity, and CO2 data to Virtual Pins V0, V1, and V2 in Blynk app
  Blynk.virtualWrite(V0, t);
  Blynk.virtualWrite(V1, h);
  
  Serial.print("\n");
  Serial.print("Humidity: " + String(h) + "%");
  Serial.print("\t");
  Serial.print("Temperature:" + String(t) + " C");
  delay(2000);
}
