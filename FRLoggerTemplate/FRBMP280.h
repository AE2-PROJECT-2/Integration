// Wrapper for a BMP280 sensor. It uses the FRSensor class, such that the Logger class can log the sensor.
// This file works as is. Use this as example for other sensors
//
// 2026-02-27, Jos Meuleman, Inholland Aeronautical & Precision Engineering, The Netherlands

#ifndef FRBMP280_h
#define FRBMP280_h

#include "FRSensor.h" //Needed to make this class part of the FRSensor class
#include <FRGeneric.h> //Needed for converting data to strings
#include <Adafruit_BMP280.h> //The library for the BMP280
#include <Wire.h> //Library for I2C sensors

class FRBMP280 : public FRSensor {
public:
  // Constructor
  FRBMP280() {
    _myBMP = new Adafruit_BMP280();
  }
  
  // Destructor
  ~FRBMP280() {
    delete _myBMP;
  }

  // Initialisation of the sensor
  bool Init(TwoWire& myWire) {
    if (!_myBMP->begin(BMP280_ADDRESS_ALT)) {
      return false;
    }
    return true;
  }

  long GetPressure() {
    return _myBMP->readPressure();
  }
  float GetAltitude() {
    return _myBMP->readAltitude(_offsetPressure);
  }
  void AutoOffset() {
    _offsetPressure = _myBMP->readPressure() / 100;
  }
  void SetOffsetPressurehPa(float inPressure) {
    _offsetPressure = inPressure;
  }

  // This function will return the names of the data that will be logged. The logger will call this function
  String HeaderString() override {
    String tempString;
    tempString.concat("Pressure [Pa]; ");
    tempString.concat("Approx altitude [m]; ");
    tempString.concat("Temperature [degC]; ");
    return tempString;
  }

  // This function will the data that will be logged. The logger will call this function
  String SensorString() override {
    String tempString;
    tempString.concat(createFloatString(GetPressure(), 0));
    tempString.concat(createFloatString(GetAltitude(), 2));  // Adjust for local pressure!!
    tempString.concat(createFloatString(_myBMP->readTemperature(), 1));
    return tempString;
  }

private:
  Adafruit_BMP280* _myBMP;
  float _offsetPressure = 1013.25; //Some default value that is not too crazy
};

#endif