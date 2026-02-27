// Template script for recording  sensors to an SD card for Flight Recorder PCB v2
// This script logs a few sensors and can be extended with more
// Required hardware:
// - 1x Flight Recorder Board v2 with ESP32
// - 1x Angular Sensor (AS5600)
// - 1x IMU Sensor (MPU9250)
// - 1x GPS (GY-GPSV3-neo or Adafruit )
// - 1x SD Card reader
// Connections:
// - components mounted on board
// - SD card reader mounted on board
// Required libraries:
// - FRLibBasics (download from https://github.com/josmeuleman/FRLibBasics, unzipped in ../Documents/Arduino/libraries/ )
// - AS5600 by Rob Tillaart (tested on 0.6.0)
// - MPU9280 Bolder Flight Systems (tested on 5.6.0), dependencies:
//    - Eigen Bolder Flight Systems (tested on 3.0.2)
//    - Unit Conversions Bolder Flight Systems (tested on 5.0.0)
// - TinyGPSPlus.h (tested)
//
// 2024-03-21, Jos Meuleman, Inholland Aeronautical & Precision Engineering, The Netherlands

// Libraries from FRLibBasics
#include <FRTimer.h>
#include <FRRGBLED.h>
#include <FRButton.h>

// Other libraries
#include <SSD1306Ascii.h>      //i2C OLED
#include <SSD1306AsciiWire.h>  //i2C OLED

// include extra files. Note that the order matters!
#include "FRLogger.h"
#include "FRBMP280.h"
#include "init.h"
#include "customfunctions.h"




//---------------------------------------------------------------------------------------------------------
// SETUP
// This block of code is only run once at the beginning
//---------------------------------------------------------------------------------------------------------
void setup() {
  // Open the serial port
  Serial.begin(9600);

  // Start the serial communication for all I2C sensors and display
  Wire.begin(I2C_SDA, I2C_SCL);
  Wire.setClock(400000);

  // init the OLED
  myOLED.begin(&Adafruit128x32, OLED_ADDRESS);
  myOLED.setFont(Adafruit5x7);  // use System5x7 for small letters
  myOLED.clear();               //clear display

  // Write messages to Serial and OLED, and control the RGB LED)
  Message("Setup", myLED, MAGENTA, Serial, myOLED);

  // Check if the button has been pressed in the following X seconds
  bool needOffsetCalculation = hasButtonBeenPressedDuringWait(4, myButton, Serial, myOLED);


  // Initialize each sensor object. Give an error on failure
  if (!myBMPSensor.Init(Wire)){
    Message("BMP Sensor not found", myLED, MAGENTA, Serial, myOLED);
  }
  else { // add it to the logger
    myLogger.AddSensor(&myBMPSensor);
  }

  // if (!myAngleOfAttackSensor.Init()){
  //   Error("Angle of Attack Sensor (AS5600) not found!");
  // }

  // if (!myIMUSensor.Init(Wire)){
  //   Error("IMU (MPU9250) not found!");
  // }

  // if (!myGPSSensor.Init(LAT0, LON0)) {
  //   Error("GPS not found!");
  // }

  if (!myLogger.CheckSD()) {
    Message("No SD card found!", myLED, RED, Serial, myOLED);
  }

  myOLED.begin(&Adafruit128x32, OLED_ADDRESS);
  myOLED.setFont(Adafruit5x7);
  myOLED.clear();  //clear display
  myOLED.print("Setup started");

  // Wait a little to make sure all sensors are up and running before reading them out
  delay(100);


  // Add the sensors to the logger
  // The "&" sign means that the sensor gets the address of the sensor object (pointer)
  // The logger now can read updates from the sensor
  // myLogger.AddSensor(&myAngleOfAttackSensor);
  // myLogger.AddSensor(&myIMUSensor);
  // myLogger.AddSensor(&myGPSSensor);

  // if the button was pressed at start, here the offset corrections can be made. In most cases it means: what you read now is zero
  // only for acceleration, be mindful that az = -9.81, assuming that your sensor is orientated as such
  if (needOffsetCalculation) {
    Serial.println("Sensor values before and after Calibration:");

  }

  // At the end of the setup, start the clock (for calculation loop durations)
  myLoggerTimer.Start();
  Message("End of Setup.", myLED, GREEN, Serial, myOLED);
  myOLED.clear();
}

//---------------------------------------------------------------------------------------------------------
// LOOP
// This block of code is looped infinitely
//---------------------------------------------------------------------------------------------------------
void loop() {

  //-------------------------------------------------------------------------------------------------------
  // Start or stop logger, depending on the button state
  //-------------------------------------------------------------------------------------------------------
  myButton.Update();              // Read the state of the button
  if (myButton.HasChangedUp()) {  //Check if the state has changed from low to high
    if (!myLogger.IsLogging()) {  // It wasn't logging yet, so start logging
      Serial.println("Start logging");
      if (!myLogger.StartLogger()) {
        Message("Something went wrong with the start of the log", myLED, RED, Serial, myOLED);
      }
      else {  // the actual start of the logging
        Message("Log started", myLED, BLUE, Serial, myOLED);
        Serial.print("File opened with the name: ");
        Serial.println(myLogger.GetLoggerFileName());
      }
    }
    else {  // Else we were logging, so now stop logging
      Message("Stop logging", myLED, GREEN, Serial, myOLED);
      if (!myLogger.StopLogger()) {
        Message("Something went wrong with the stopping of the log", myLED, RED, Serial, myOLED);
      }
    }
  }

  //-------------------------------------------------------------------------------------------------------
  // Write to the log file if needed
  //-------------------------------------------------------------------------------------------------------
  String myString = myLogger.UpdateSensors();  // Updates all connected sensors and generates a string of all sensor values;
  Serial.print(myString);                      // Writing to the Serial Monitor will sometimes take more than 100 ms. That will give overruns So print to screen only when you have a slow update rate.
  myLogger.WriteLogger();                      // This will only write to logger if myLogger.IsLogging is true;

  //-------------------------------------------------------------------------------------------------------
  // Update the OLED
  //-------------------------------------------------------------------------------------------------------
  // myOLED.setRow(1);
  // myOLED.print("# Sats found: ");
  // myOLED.print(myGPSSensor.GetSatellites());
  // myOLED.println("   ");
  // if (myGPSSensor.HasValidData()){
  //   myOLED.println("GPS fix    ");
  // }
  // else {
  //   myOLED.println("No GPS fix");
  // }


  //-------------------------------------------------------------------------------------------------------
  // End of the loop
  //-------------------------------------------------------------------------------------------------------
  // At the end of the loop, WaitUntilEnd runs until the time until looptime has passed
  if (myLoggerTimer.WaitUntilEnd()) {
    Serial.println("Overrun!");  // if there are delays in the loop, you will get overruns i.e. the loop took longer than the looptime
  }
  //
}
