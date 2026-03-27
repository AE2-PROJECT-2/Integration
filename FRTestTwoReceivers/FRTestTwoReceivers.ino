// Test script to see if the EPS can handle two controllers
// Required hardware:
// - 1x Flight Recorder Board v2 with ESP32
// - 2x Turnigy TGY IA6B Receiver programmed in PPM mode
// - 2x Tunigy TGY-i6S Controller programmed in PPM mode
// - 1x OLED connected to board
// Connections:
// - TGY2 channel PPM, pin 1 - FR Board, J3-GND
// - TGY2 channel PPM, pin 2 - FR Board, J3-+5V
// - TGY2 channel PPM, pin 3 - FR Board, J3-D4
// - TGY1 channel PPM, pin 1 - FR Board, J2-GND
// - TGY1 channel PPM, pin 3 - FR Board, J2-D15
// Required libraries:
// - FRLibBasics (download from https://github.com/josmeuleman/FRLibBasics, unzipped in ../Documents/Arduino/libraries/ )
//
// 2024-05-06, Jos Meuleman, Inholland Aeronautical & Precision Engineering, The Netherlands

// Libraries from FRLibBasics
#include <FRTimer.h>
#include <FRPPMReceiverSensor.h>
#include <FRRGBLED.h>

// Other libraries

// include extra project files. Note that the order matters!
#include "FRPrimaryReceiver.h"
#include "init.h"

//---------------------------------------------------------------------------------------------------------
// SETUP
// This block of code is only run once at the beginning
//---------------------------------------------------------------------------------------------------------
void setup() {
  // Start the serial communication for all I2C sensors
  
  Serial.begin(9600);
  delay(1000);  // safe to wait a few milliseconds after serial.begin before writing to serial port
  Serial.println("Setup ");

  MyReceiverPrimary.Init();
  MyReceiverSecondary.Init();

  myTimer.Start();
  Serial.println("End of Setup");
}

//---------------------------------------------------------------------------------------------------------
// LOOP
// This block of code is looped infinitely
//---------------------------------------------------------------------------------------------------------
void loop() {
  //-------------------------------------------------------------------------------------------------------
  // Read the values from the receivers
  //-------------------------------------------------------------------------------------------------------
  // The primary receiver is now a sensor object, so you can read the SensorString
  Serial.print(MyReceiverPrimary.SensorString());

  // The secondary receiver is not a sensor object, so read the channels individually
  for (byte i = 0; i < NUMBEROFCHANNELS2; i++) {
    channelsValues2[i] = MyReceiverSecondary.ReadChannel(i);
    Serial.print(channelsValues2[i]);
    Serial.print("; ");
  }
  // End with a new line
  Serial.println();


  //-------------------------------------------------------------------------------------------------------
  // End of the loop
  //-------------------------------------------------------------------------------------------------------
  // At the end of the loop, WaitUntilEnd runs until the time until looptime has passed
  if (myTimer.WaitUntilEnd()) {
    Serial.println("Overrun!");  // if there are delays in the loop, you will get overruns i.e. the loop took longer than the looptime
  }
  //
}
