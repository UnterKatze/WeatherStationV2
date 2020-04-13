// ************************************* I N C L U D E S ********************************************* //
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

// ************************************** D E F I N E S ********************************************** //
#define SEALEVELPRESSURE_HPA (1013.25)
#define sensor_altitude (321.0)

// ********************************** D E C L A R A T I O N S **************************************** //
Adafruit_BME280 bme; // I2C

int delayTime = 3000;

// ************************************ F U N C T I O N S ******************************************** //

void printValues() {
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
    Serial.println(" %\n");
}

// **************************************** S E T U P ************************************************ //
void setup() {
    Serial.begin(9600);
    while(!Serial);    // time to get serial running
    Serial.println(F("BME280 test"));

    unsigned status;

    status = bme.begin();  
    
    if (!status) {
        Serial.println("Could not find a valid BME280 sensor, check wiring, address, sensor ID!");
        while (1) {};
    }
}

// **************************************** L O O P ************************************************** //
void loop() { 
    printValues();
    delay(delayTime);
}
