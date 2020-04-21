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
int hour, minute, second;
bool nightmode = false;
bool timeupdate = false;

double temperature = 0.0;
double pressure = 0.0;
double humidity = 0.0;

unsigned long set2020_1 = 1585443600; // 29.03.2020 02:00 UTC+1
unsigned long set2020_2 = 1603587600; // 25.10.2020 03:00 UTC+2

unsigned long set2021_1 = 1616893200; // 28.03.2021 02:00 UTC+1
unsigned long set2021_2 = 1635642000; // 31.10.2021 03:00 UTC+2

const char *ssid = "Kwik-E-Mart";
const char *password = "9589089603281286";

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", 3600, 60000);

unsigned status;

String data;

// ************************************ F U N C T I O N S ******************************************** //

void readSensorData()
{
  if (status)
  {
    temperature = bme.readTemperature();
    pressure = (bme.readPressure() / 100.0F) + (0.12F * sensor_altitude);
    humidity = bme.readHumidity();
  }
}

void readOnlineTime()
{
  unsigned long epoch_time;
  if ((WiFi.status() == WL_CONNECTED))
  {
    hour = timeClient.getHours();
    minute = timeClient.getMinutes();
    second = timeClient.getSeconds();
    epoch_time = timeClient.getEpochTime();
    if ((epoch_time >= set2020_1) && (epoch_time <= set2020_2))
    {
      hour++;
    }
    if ((hour >= 22) || (hour <= 5))
    {
      nightmode = true;
    } else
    {
      nightmode = false;
    }
  }
}

void printToDisplay()
{
  Serial.print(data);
  Serial.write(0xFF);
  Serial.write(0xFF);
  Serial.write(0xFF);
}

// **************************************** S E T U P ************************************************ //
void setup()
{
  Serial.begin(9600);
  while (!Serial)
  {
    delay(100); // time to get serial running
  }

  data = "page 0";
  printToDisplay();

  data = "t0.pco=65535";
  printToDisplay(); // Controller Starting
  delay(100);

  data = "t3.pco=7648";
  printToDisplay(); // positive response
  delay(100);

  data = "t1.pco=65535";
  printToDisplay(); // Starting Serial
  delay(100);

  data = "t4.pco=7648";
  printToDisplay(); // positive response
  delay(100);

  WiFi.begin(ssid, password);
  Serial.print("Connecting to Internet ");
  while (WiFi.status() != WL_CONNECTED)
  {
    data = "t2.pco=65535";
    printToDisplay(); // Connecting to Internet
    delay(100);
    int timer_100ms = 0;
    timer_100ms++;
    if (timer_100ms >= 100)
    {
      data = "t8.pco=63488";
      printToDisplay(); // negative response
      break;
    }
  }
  if (WiFi.status() == WL_CONNECTED)
  {
    data = "t7.pco=7648";
    printToDisplay(); // positive response
    timeClient.begin();
  }

  status = bme.begin();
  data = "t9.pco=65535";
  printToDisplay(); // Configuring Sensor

  if (!status)
  {
    data = "t11.pco=63488";
    printToDisplay(); // negative response
  }
  else
  {
    data = "t10.pco=7648";
    printToDisplay(); // positive response
  }

  delay(3000); // delay for reading boot info
  data = "page 1";
  printToDisplay(); // switch to page 1
}

// **************************************** L O O P ************************************************** //
void loop()
{
  timeupdate = timeClient.update();
  readOnlineTime();
  readSensorData();
  // set correct picture function
  if (nightmode) // put in own function + add background picture for text boxes + change color of text
  {
    data = "page1.pic=7";
    printToDisplay(); // display night picture with wifi on
  } else
  {
    if ((WiFi.status() == WL_CONNECTED) && (temperature < 0.0))
    {
      data = "page1.pic=9";
      printToDisplay(); // display cold picture with wifi on
      break;
    }
    if ((WiFi.status() != WL_CONNECTED) && (temperature < 0.0))
    {
      data = "page1.pic=8";
      printToDisplay(); // display cold picture with wifi off
      break;
    }


    if ((WiFi.status() == WL_CONNECTED) && (temperature < 10.0))
    {
      data = "page1.pic=5";
      printToDisplay(); // display coldish picture with wifi on
      break;
    }
    if ((WiFi.status() != WL_CONNECTED) && (temperature < 10.0))
    {
      data = "page1.pic=4";
      printToDisplay(); // display coldish picture with wifi off
      break;
    }


    if ((WiFi.status() == WL_CONNECTED) && (temperature < 20.0))
    {
      data = "page1.pic=1";
      printToDisplay(); // display warmer picture with wifi on
      break;
    }
    if ((WiFi.status() != WL_CONNECTED) && (temperature < 20.0))
    {
      data = "page1.pic=0";
      printToDisplay(); // display warmer picture with wifi off
      break;
    }


    if ((WiFi.status() == WL_CONNECTED) && (temperature < 30.0))
    {
      data = "page1.pic=3";
      printToDisplay(); // display warm picture with wifi on
      break;
    }
    if ((WiFi.status() != WL_CONNECTED) && (temperature < 30.0))
    {
      data = "page1.pic=2";
      printToDisplay(); // display warm picture with wifi off
      break;
    }


    if ((WiFi.status() == WL_CONNECTED))
    {
      data = "page1.pic=11";
      printToDisplay(); // display hot picture with wifi on
      break;
    } else
    {
      data = "page1.pic=10";
      printToDisplay(); // display hot picture with wifi off
      break;
    }
  }
  // print data to display (for time only minutes, maybe change refresh rate)
  Serial.print("\n");
  mainCounter++;
  delay(delayTime);
}
