// Class for logging data to an SD card
//
// 2024-03-07, Jos Meuleman & Tim van Cuylenborg, Inholland Aeronautical & Precision Engineering, The Netherlands

#ifndef FRLogger_h
#define FRLogger_h

#include <SD.h>        // include the library for the SD card
#include <FS.h>        // include the library for writing files
#include "FRSensor.h"  //has access to the virtual functions of the sensor class
#include <vector>

class Logger {
public:

  // Constructor: initialize the logger object
  Logger() {
    _isLogging = false;
    _fileName = "";
    _loggerString.reserve(512);
    _headerString.clear();
    _headerString.concat("Time[ms]; ");
  }

  // Checks the presence of the SD card
  bool CheckSD() {
    return (SD.begin());
  }

  // Adds a sensor to the array of sensors
  void AddSensor(FRSensor* Sensor) {
    sensors.push_back(Sensor);
    _headerString.concat(Sensor->HeaderString());
  }

  String GetLoggerFileName() {
    return _fileName;
  }

  String GetHeaderString() {
    return _headerString;
  }

  String GetSensorString() {
    return _loggerString;
  }

  bool IsLogging() {
    return _isLogging;
  }


  bool StartLogger() {
    if (_isLogging) {  // Already logging
      return false;
    }
    _fileName = FindNewFileName();
    if (_fileName.length() == 0) {  // No proper filename found
      return false;
    }
    _file = SD.open(_fileName, FILE_WRITE);
    if (!_file) {  //Failed to open the file
      return false;
    }

    // Successfully opened a logfile. Write the header line
    _file.println(_headerString);
    _isLogging = true;
    return true;
  }

  bool StopLogger() {
    if (!_isLogging) {  // Logger was not running
      return false;
    }
    if (!_file) {  // File was not open
      return false;
    }
    _file.close();
    _isLogging = false;
    return true;
  }

  String UpdateSensors() {
    _loggerString = "";
    _loggerString.concat(String(millis()));  // Add the time
    _loggerString.concat("; ");
    for (int i = 0; i < sensors.size(); i++) {  // Loop through the sensors
      _loggerString.concat(sensors[i]->SensorString());
    }
    _loggerString.concat("\n");  // close with a new line
    return _loggerString;
  }

  void WriteLogger() {
    if (_isLogging) {
      _file.print(_loggerString);
    }
  }

  // void AddFloatToLogLine(float value, String format);
  // void AddStringToLogLine(float value, String format);


private:
  String FindNewFileName() {
    int fileNumber = 1;
    while (fileNumber <= 99999) {
      _fileName = "/LOG";
      String fileNumberString = String(fileNumber);
      for (int i = 0; i < 5 - fileNumberString.length(); i++) {
        _fileName.concat("0");
      }
      _fileName.concat(fileNumberString);
      _fileName.concat(".CSV");

      if (!SD.exists(_fileName)) {
        return _fileName;
      }
      fileNumber++;
    }
    return "";
  }

  std::vector<FRSensor*> sensors;  // vector of Sensor objects
  bool _isLogging;
  String _fileName;
  File _file;
  String _loggerString;
  String _headerString;
};


#endif