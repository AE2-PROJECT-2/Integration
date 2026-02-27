// Generic FRSensor for the Logger
// 
// 2024-03-07, Jos Meuleman & Tim van Cuylenborg, Inholland Aeronautical & Precision Engineering, The Netherlands


#ifndef FRSensor_h
#define FRSensor_h

#include "WString.h"

//base class for all sensor, neccessary to interface with the FRLogger
class FRSensor {
public:
  virtual String HeaderString() = 0;
  virtual String SensorString() = 0;
  virtual ~FRSensor() = default;
};

#endif