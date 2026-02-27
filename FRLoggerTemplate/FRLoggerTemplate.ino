// Template script for recording  sensors to an SD card for Flight Recorder PCB v2
// This script logs a few sensors and can be extended with more
// Required hardware:
// - 1x Flight Recorder Board v2 with ESP32
// - 1x SD Card reader
// - 1x ESP32
// - 1x OLED display
// - sensors
// Required libraries:
// - FRLibBasics (download from https://github.com/josmeuleman/FRLibBasics, unzipped in ../Documents/Arduino/libraries/ )
// - Libraries that are needed per sensor
//
// 2026-02-27, Jos Meuleman, Inholland Aeronautical & Precision Engineering, The Netherlands

// Libraries from FRLibBasics
#include <FRTimer.h>
#include <FRRGBLED.h>
#include <FRButton.h>

// Other libraries
#include <SSD1306Ascii.h>      //i2C OLED
#include <SSD1306AsciiWire.h>  //i2C OLED

// include extra project files. Note that the order matters!
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
  // On success, add the sensors to the logger
  // The "&" sign means that the sensor gets the address of the sensor object (pointer)
  // The logger now can read updates from the sensor

  // Pressure sensor
  if (!myBMPSensor.Init(Wire)){
    Message("BMP Sensor not found", myLED, MAGENTA, Serial, myOLED);
  }
  else { // add it to the logger
    myLogger.AddSensor(&myBMPSensor);
  }

  // Do something similar for your other sensors


  // Check presence of the SD-card reader and SD-card
  if (!myLogger.CheckSD()) {
    Message("No SD card found!", myLED, RED, Serial, myOLED);
  }

  // Wait a little to make sure all sensors are up and running before reading them out
  delay(100);

  // if the button was pressed at start, here the offset corrections can be made. In most cases it means: what you read now is zero
  // only for acceleration, be mindful that az = -9.81, assuming that your sensor is orientated as such
  if (needOffsetCalculation) {
    myBMPSensor.AutoOffset();
    // Do your other sensors here too 
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
  myOLED.setRow(1);
  myOLED.print("alt. [m]: ");
  myOLED.print(myBMPSensor.GetAltitude());
  myOLED.println("   "); // This is a trick to overwrite some old data with some blanks. 
  

  //-------------------------------------------------------------------------------------------------------
  // End of the loop
  //-------------------------------------------------------------------------------------------------------
  // At the end of the loop, WaitUntilEnd runs until the time until looptime has passed
  if (myLoggerTimer.WaitUntilEnd()) {
    Serial.println("Overrun!");  // if there are delays in the loop, you will get overruns i.e. the loop took longer than the looptime
  }
  //
}
