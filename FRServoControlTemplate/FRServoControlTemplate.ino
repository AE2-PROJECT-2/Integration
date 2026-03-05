// Template script for the servo control. In this script  and servo handler is implemented
// Required hardware:
// - 1x Flight Recorder Board with ESP32
// - 1x Turnigy TGY IA6B Receiver programmed in PPL mode
// - 1x Tunigy TGY-i6S Controller programmed in PPL mode
// - 2x Servo motors
// - 1x RGB LED 
// Connections:
// - TGY channel PPM, pin 1 - FR Board, PPL2 GND
// - TGY channel PPM, pin 2 - FR Board, PPL2, D4
// - TGY channel PPM, pin 3 - FR Board, J11 5V (or connected to other 5V source)
// Required libraries:
// - FRLibBasics (download from https://github.com/josmeuleman/FRLibBasics, unzipped in ../Documents/Arduino/libraries/ )
// - ESP32Servo.h
//
// 2024-03-21, Jos Meuleman, Inholland Aeronautical & Precision Engineering, The Netherlands

// Libraries from FRLibBasics
#include <FRTimer.h>
#include <FRButton.h>
#include <FRRGBLED.h>
#include <FRPPMReceiverSensor.h>
// Other libraries
#include <ESP32Servo.h>

// include extra project files. Note that the order matters!
#include "init.h"
#include "customfunctions.h"


//---------------------------------------------------------------------------------------------------------
// SETUP
// This block of code is only run once at the beginning
//---------------------------------------------------------------------------------------------------------
void setup() {
  myLed.SetColor(MAGENTA);
  Serial.begin(9600);  // Start the serial communciation
    
  myReceiver.Init();

  for (int i=0; i<NUMBEROFSERVOS; i++) {
    myServo[i].attach(PINSERVO[i]);
  }

  // Starting pos for servos. Make sure they are in the right positin
  servoTargetPos[SERVOLANDINGGEAR] = SERVOLANDINGGEARPOSEXTENDED;
  servoTargetPos[SERVOLANDINGHATCH] = SERVOLANDINGHATCHPOSOPEN;
  for (int i = 0; i < NUMBEROFSERVOS; i++) {
    servoActualPos[i] = servoTargetPos[i];
  }
  
  myServoTimer.Start();
  Serial.println("End of Setup");
  myLed.SetColor(GREEN);  
}

//---------------------------------------------------------------------------------------------------------
// LOOP
// This block of code is looped infinitely
//---------------------------------------------------------------------------------------------------------
void loop() {
  //-------------------------------------------------------------------------------------------------------
  // Read the myReceiver and store the values
  //-------------------------------------------------------------------------------------------------------
  for (byte i = 0; i < NUMBEROFCHANNELS; i++) {
    channelValues[i] = myReceiver.ReadChannel(i);
  }
  
  // for debugging, print some values. This debugging slows down your program
  // for (byte i = 0; i < NUMBEROFCHANNELS; i++) {
  //   Serial.print(channelValues[i]);
  //   Serial.print("; ");
  // }
  // Serial.println();

  //-------------------------------------------------------------------------------------------------------
  // Process the controller channels
  //-------------------------------------------------------------------------------------------------------
  // Read switch SWA (high/low) for logging
  loggerSwitchState = (channelValues[LOGGERSWITCHCHANNEL] < PPMTHRESHMID);
  
  // Compare the state with the previous state:
  startLogger = (loggerSwitchState < loggerSwitchStatePrev);  //if new state is LOW (0) and old state is HIGH (1)
  stopLogger = (loggerSwitchState > loggerSwitchStatePrev);   //if new state is HIGH (1) and old state is LOW (1)
  loggerSwitchStatePrev = loggerSwitchState; //update the previous state for the next loop

  if  (startLogger) {
    // insert code for starting logging
    myLed.SetColor(BLUE);
    Serial.println("Start logging");
  }
  if (stopLogger) {
    // insert code for stopping logging
    myLed.SetColor(GREEN);
    Serial.println("Stop logging ");
  }

  // Read switch SWB (high/mid/low) for landing gear. 
  HandleLandingGearSwitch(channelValues[LANDINGGEARCHANNEL]); //Update of landingGearAction and landingGearHatchAction

  //-------------------------------------------------------------------------------------------------------
  // Handle the motors
  //-------------------------------------------------------------------------------------------------------
  UpdateServos();
  // // for debugging, print some values. This debugging slows down your program
  // for (int i = 0; i < NUMBEROFSERVOS; i++) {
  //   Serial.print(servoActualPos[i]);
  //   Serial.print("; ");
  // }
  // Serial.println();

  
  //-------------------------------------------------------------------------------------------------------
  // End of the loop
  //-------------------------------------------------------------------------------------------------------
  // Kill the time until
  if (myServoTimer.WaitUntilEnd()) {
    Serial.println("Overrun!");
  }
}

