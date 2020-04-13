// ************************************* I N C L U D E S ********************************************* //
#include <ESP8266WiFi.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

// ************************************** D E F I N E S ********************************************** //
#define SEALEVELPRESSURE_HPA (1013.25)
#define sensor_altitude (321.0)

// ********************************** D E C L A R A T I O N S **************************************** //
Adafruit_BME280 bme; // I2C
bool sensor_online = false;

int delayTime = 3000;
long timer_100ms = 0;
unsigned long main_timer = 0;

const char *ssid     = "Mi 9 SE";
const char *password = "12345678";

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", 3600, 60000);

// ************************************ F U N C T I O N S ******************************************** //

void printSensorData() {
  if (sensor_online == true) {
    Serial.print("Temperature = ");
    Serial.print(bme.readTemperature(), 1);
    Serial.println(" °C");

    Serial.print("Normalized Pressure = ");
    Serial.print((bme.readPressure() / 100.0F) + (0.12F * sensor_altitude), 1);
    Serial.println(" hPa");

    Serial.print("Approx. Altitude = ");
    Serial.print(bme.readAltitude(SEALEVELPRESSURE_HPA), 1);
    Serial.println(" m");

    Serial.print("Humidity = ");
    Serial.print(bme.readHumidity(), 0);
    Serial.println(" %");
  }
}

unsigned status;

// **************************************** S E T U P ************************************************ //
void setup() {
  Serial.begin(9600);
  while (!Serial) {
    delay(1);                                                             // time to get serial running
  }
  Serial.println("Serial working");

  WiFi.begin(ssid, password);
  Serial.print("Connecting to Internet ");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(100);
    timer_100ms++;
    if (timer_100ms >= 100) {
      Serial.println("\nNo Internet Connection");
      break;
    }
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nInternet Connection Established");
  }

  status = bme.begin();
  delay(1);
  if (!status) {
    Serial.println("Could not find a valid BME280 sensor");
  } else {
    sensor_online = true;
  }

  if (WiFi.status() == WL_CONNECTED) {
    timeClient.begin();
  }
}

// **************************************** L O O P ************************************************** //
void loop() {
  printSensorData();
  if (WiFi.status() == WL_CONNECTED) {
    timeClient.update();
    Serial.println(timeClient.getFormattedTime());
  }
  Serial.print("\n");
  main_timer++;
  delay(delayTime);
}
