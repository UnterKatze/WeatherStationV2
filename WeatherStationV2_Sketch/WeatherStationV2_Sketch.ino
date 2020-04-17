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

int delayTime = 1000;
int mainCounter = 0;
bool timeupdate = false;

unsigned long set2020_1 = 1585443600; // 29.03.2020 02:00 UTC+1
unsigned long set2020_2 = 1603587600; // 25.10.2020 03:00 UTC+2

unsigned long set2021_1 = 1616893200; // 28.03.2021 02:00 UTC+1
unsigned long set2021_2 = 1635642000; // 31.10.2021 03:00 UTC+2

const char *ssid     = "Kwik-E-Mart";
const char *password = "9589089603281286";

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", 3600, 60000);

unsigned status;

// ************************************ F U N C T I O N S ******************************************** //

void printSensorData() {
  if (status) {
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

void printTime() {
  int hour, minute, second;
  unsigned long epoch_time;
  if ((WiFi.status() == WL_CONNECTED)) {
    hour = timeClient.getHours();
    minute = timeClient.getMinutes();
    second = timeClient.getSeconds();
    epoch_time = timeClient.getEpochTime();
    if ((epoch_time >= set2020_1) && (epoch_time <= set2020_2)) {
      hour++;
    } else {
    }
    Serial.println(epoch_time);
    Serial.print(hour);
    Serial.print(":");
    Serial.print(minute);
    Serial.print(":");
    Serial.print(second);
    Serial.print("\n");
  }
}

// **************************************** S E T U P ************************************************ //
void setup() {
  Serial.begin(9600);
  while (!Serial) {
    delay(1);                                                             // time to get serial running
  }
  Serial.println("\nSerial working");

  WiFi.begin(ssid, password);
  Serial.print("Connecting to Internet ");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(100);
    int timer_100ms = 0;
    timer_100ms++;
    if (timer_100ms >= 100) {
      Serial.println("\nNo Internet Connection");
      break;
    }
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nInternet Connection Established");
    timeClient.begin();
  }

  status = bme.begin();
  delay(100);
  if (!status) {
    Serial.println("Could not find a valid BME280 sensor");
  }
}

// **************************************** L O O P ************************************************** //
void loop() {
  timeupdate = timeClient.update();

  printSensorData();

  printTime();

  Serial.print("\n");
  mainCounter++;
  delay(delayTime);
}
