#ifndef INIT_H
#define INIT_H

// Pins
const byte I2C_SDA = 21;    // The data pin for I2C communication
const byte I2C_SCL = 22;    // The clock pin for I2C communcation
const byte PINSWITCH = 35;  // The pin number for he button to start and stop logging

// other constants
const int LOGGERLOOPTIMEMS = 100;  // Loop time for logging
const int OLED_ADDRESS = 0x3C;     // I2C address of OLED

// Create all objects
Timer myLoggerTimer(LOGGERLOOPTIMEMS);  // Timer object for the clock
Logger myLogger;                        // Logger object for logging sensors to the SD
Button myButton(PINSWITCH, true);       // Create a button object with the given pin. True for an inverted button, false for a normal button
RGBLED myLED;                           // Create a RGB led object. pinnummbers are defined in the library FRRGBLED.h.

FRBMP280 myBMPSensor;

SSD1306AsciiWire myOLED;

#endif //INIT_H