// ************************************* I N C L U D E S ********************************************* //
#include <ESP8266WiFi.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <time.h>

// ************************************** D E F I N E S ********************************************** //
#define SEALEVELPRESSURE_HPA (1013.25)
#define sensor_altitude (321.0)

// ********************************** D E C L A R A T I O N S **************************************** //
Adafruit_BME280 bme; // I2C

int delayTime = 1000;
int hour, minute, second;
int timer_100ms = 0;

int mainTicker = 20;
bool conn_timeout = false;

int lastPicture = 6;

bool nightmode = false;
bool timeupdate = false;

double temperature = 0.0;
int temp = 0;
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
      if (hour == 24) {
        hour = 0;
      }
    }

    struct tm *ti;
    time_t rawtime = epoch_time;
    ti = localtime(&rawtime);
    uint8_t month = ti->tm_mon + 1;
    String monthStr = month < 10 ? "0" + String(month) : String(month);
    switch (monthStr.toInt())
    {
    case 1:
      if ((hour >= 17) || (hour <= 7))
      {
        nightmode = true;
      }
      else
      {
        nightmode = false;
      }
      break;

    case 2:
      if ((hour >= 18) || (hour <= 6))
      {
        nightmode = true;
      }
      else
      {
        nightmode = false;
      }
      break;

    case 3:
      if ((hour >= 19) || (hour <= 5))
      {
        nightmode = true;
      }
      else
      {
        nightmode = false;
      }
      break;

    case 4:
      if ((hour >= 21) || (hour <= 5))
      {
        nightmode = true;
      }
      else
      {
        nightmode = false;
      }
      break;

    case 5:
      if ((hour >= 21) || (hour <= 4))
      {
        nightmode = true;
      }
      else
      {
        nightmode = false;
      }
      break;

    case 6:
      if ((hour >= 22) || (hour <= 3))
      {
        nightmode = true;
      }
      else
      {
        nightmode = false;
      }
      break;

    case 7:
      if ((hour >= 22) || (hour <= 4))
      {
        nightmode = true;
      }
      else
      {
        nightmode = false;
      }
      break;

    case 8:
      if ((hour >= 21) || (hour <= 4))
      {
        nightmode = true;
      }
      else
      {
        nightmode = false;
      }
      break;

    case 9:
      if ((hour >= 20) || (hour <= 5))
      {
        nightmode = true;
      }
      else
      {
        nightmode = false;
      }
      break;

    case 10:
      if ((hour >= 19) || (hour <= 6))
      {
        nightmode = true;
      }
      else
      {
        nightmode = false;
      }
      break;

    case 11:
      if ((hour >= 17) || (hour <= 6))
      {
        nightmode = true;
      }
      else
      {
        nightmode = false;
      }
      break;

    case 12:
      if ((hour >= 16) || (hour <= 7))
      {
        nightmode = true;
      }
      else
      {
        nightmode = false;
      }
      break;

    default:
      break;
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

void setCorrectTextboxBackgroundPicture(int picture)
{
  if (lastPicture != picture)
  {
    data = "n6.picc=";
    data = data + String(picture);
    printToDisplay();

    data = "n7.picc=";
    data = data + String(picture);
    printToDisplay();

    data = "n1.picc=";
    data = data + String(picture);
    printToDisplay();

    data = "n8.picc=";
    data = data + String(picture);
    printToDisplay();

    data = "t4.picc=";
    data = data + String(picture);
    printToDisplay();

    data = "t1.picc=";
    data = data + String(picture);
    printToDisplay();

    data = "n3.picc=";
    data = data + String(picture);
    printToDisplay();

    data = "t2.picc=";
    data = data + String(picture);
    printToDisplay();

    data = "n4.picc=";
    data = data + String(picture);
    printToDisplay();

    data = "t3.picc=";
    data = data + String(picture);
    printToDisplay();

    lastPicture = picture;
  }
}

void setDisplayPicture()
{
  if (nightmode && (WiFi.status() == WL_CONNECTED))
  {
    data = "page1.pic=7";
    printToDisplay(); // display night picture with wifi on
    setCorrectTextboxBackgroundPicture(7);
  }
  else
  {
    if (nightmode && (WiFi.status() != WL_CONNECTED))
    {
      data = "page1.pic=6";
      printToDisplay(); // display night picture with wifi off
      setCorrectTextboxBackgroundPicture(6);
      return;
    }
    if ((WiFi.status() == WL_CONNECTED) && (temperature < 0.0))
    {
      data = "page1.pic=9";
      printToDisplay(); // display cold picture with wifi on
      setCorrectTextboxBackgroundPicture(9);
      return;
    }
    if ((WiFi.status() != WL_CONNECTED) && (temperature < 0.0))
    {
      data = "page1.pic=8";
      printToDisplay(); // display cold picture with wifi off
      setCorrectTextboxBackgroundPicture(8);
      return;
    }

    if ((WiFi.status() == WL_CONNECTED) && (temperature < 10.0))
    {
      data = "page1.pic=5";
      printToDisplay(); // display coldish picture with wifi on
      setCorrectTextboxBackgroundPicture(5);
      return;
    }
    if ((WiFi.status() != WL_CONNECTED) && (temperature < 10.0))
    {
      data = "page1.pic=4";
      printToDisplay(); // display coldish picture with wifi off
      setCorrectTextboxBackgroundPicture(4);
      return;
    }

    if ((WiFi.status() == WL_CONNECTED) && (temperature < 20.0))
    {
      data = "page1.pic=1";
      printToDisplay(); // display warmer picture with wifi on
      setCorrectTextboxBackgroundPicture(1);
      return;
    }
    if ((WiFi.status() != WL_CONNECTED) && (temperature < 20.0))
    {
      data = "page1.pic=0";
      printToDisplay(); // display warmer picture with wifi off
      setCorrectTextboxBackgroundPicture(0);
      return;
    }

    if ((WiFi.status() == WL_CONNECTED) && (temperature < 30.0))
    {
      data = "page1.pic=3";
      printToDisplay(); // display warm picture with wifi on
      setCorrectTextboxBackgroundPicture(3);
      return;
    }
    if ((WiFi.status() != WL_CONNECTED) && (temperature < 30.0))
    {
      data = "page1.pic=2";
      printToDisplay(); // display warm picture with wifi off
      setCorrectTextboxBackgroundPicture(2);
      return;
    }

    if ((WiFi.status() == WL_CONNECTED))
    {
      data = "page1.pic=11";
      printToDisplay(); // display hot picture with wifi on
      setCorrectTextboxBackgroundPicture(11);
      return;
    }
    else
    {
      data = "page1.pic=10";
      printToDisplay(); // display hot picture with wifi off
      setCorrectTextboxBackgroundPicture(10);
      return;
    }
  }
}

void setDisplayTextColor()
{
  if (temperature < 13.0)
  {
    data = "t4.pco=63488"; // red temp
    printToDisplay();
    data = "t1.pco=63488"; // red degrees
    printToDisplay();
  }
  else
  {
    if (temperature < 18.0)
    {
      data = "t4.pco=65504"; // yellow temp
      printToDisplay();
      data = "t1.pco=65504"; // yellow degrees
      printToDisplay();
    }
    else
    {
      data = "t4.pco=7648"; // green temp
      printToDisplay();
      data = "t1.pco=7648"; // green degrees
      printToDisplay();
    }
  }
}

void printDataToDisplay()
{
  int calcHour = 0;
  int calcMinute = 0;

  if (hour < 10)
  {
    data = "n6.val=0";
    printToDisplay();
    calcHour = hour;
  }
  else
  {
    if (hour < 20)
    {
      data = "n6.val=1";
      printToDisplay();
      calcHour = hour - 10;
    }
    else
    {
      data = "n6.val=2";
      printToDisplay();
      calcHour = hour - 20;
    }
  }

  data = "n7.val=";
  data = data + calcHour;
  printToDisplay();

  if (minute < 10)
  {
    data = "n1.val=0";
    printToDisplay();
    calcMinute = minute;
  }
  else
  {
    if (minute < 20)
    {
      data = "n1.val=1";
      printToDisplay();
      calcMinute = minute - 10;
    }
    else
    {
      if (minute < 30)
      {
        data = "n1.val=2";
        printToDisplay();
        calcMinute = minute - 20;
      }
      else
      {
        if (minute < 40)
        {
          data = "n1.val=3";
          printToDisplay();
          calcMinute = minute - 30;
        }
        else
        {
          if (minute < 50)
          {
            data = "n1.val=4";
            printToDisplay();
            calcMinute = minute - 40;
          }
          else
          {
            data = "n1.val=5";
            printToDisplay();
            calcMinute = minute - 50;
          }
        }
      }
    }
  }

  temp = round(temperature);

  data = "t4.txt=\"";
  data = data + String(temp);
  data = data + "\"";
  printToDisplay();

  data = "n3.val=";
  data = data + String(pressure, 0);
  printToDisplay();

  if (humidity >= 99.0)
  {
    humidity = 99.0;
  }
  data = "n4.val=";
  data = data + String(humidity, 0);
  printToDisplay();
}

// **************************************** S E T U P ************************************************ //
void setup()
{
  Serial.begin(9600);
  while (!Serial)
  {
    delay(10); // time to get serial running
  }

  data = "page 0";
  printToDisplay();
  printToDisplay();

  data = "t0.pco=65535";
  printToDisplay(); // Controller Starting
  delay(10);

  data = "t3.pco=7648";
  printToDisplay(); // positive response
  delay(10);

  data = "t1.pco=65535";
  printToDisplay(); // Starting Serial
  delay(10);

  data = "t4.pco=7648";
  printToDisplay(); // positive response
  delay(10);

  WiFi.begin(ssid, password);
  while ((WiFi.status() != WL_CONNECTED) && !conn_timeout)
  {
    data = "t2.pco=65535";
    printToDisplay(); // Connecting to Internet
    delay(100);
    timer_100ms++;
    if (timer_100ms >= 100)
    {
      data = "t8.pco=63488";
      printToDisplay(); // negative response
      conn_timeout = true;
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

  delay(2500); // delay for reading boot info
  data = "page 1";
  printToDisplay(); // switch to page 1
  data = "t0.txt=\" \"";
  printToDisplay(); // bug with -
}

// **************************************** L O O P ************************************************** //
void loop()
{
  timeupdate = timeClient.update();
  readOnlineTime();
  readSensorData();
  if (mainTicker >= 20)
  {
    setDisplayPicture();
    setDisplayTextColor();
    printDataToDisplay();
    mainTicker = 0;
  }

  delay(delayTime);
  mainTicker++;
}
