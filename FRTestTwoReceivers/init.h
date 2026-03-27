#ifndef INIT_H
#define INIT_H

// Pins
const byte PINPPM1 = 15;      // Primary Receiver input
const byte PINPPM2 = 4;      // Secondary Receiver input pint

// other constants
const byte NUMBEROFCHANNELS1 = 4;  
const byte NUMBEROFCHANNELS2 = 4;  
const int LOOPTIMEMS = 100;  // Loop time for logging

// Create all objects
Timer myTimer(LOOPTIMEMS);  // Timer object for the clock
RGBLED myLED;                           // Create a RGB led object. pinnummbers are defined in the library FRRGBLED.h.

//FRPrimaryReceiver* FRPrimaryReceiver::_instance = nullptr;
FRPrimaryReceiver MyReceiverPrimary(PINPPM1, NUMBEROFCHANNELS1);  // Create a PPM receiver object with given pin and number of channels

PPMReceiverSensor MyReceiverSecondary(PINPPM2, NUMBEROFCHANNELS2);  // Create a PPM receiver object with given pin and number of channels

int channelsValues1[NUMBEROFCHANNELS1];
int channelsValues2[NUMBEROFCHANNELS2];

#endif //INIT_H