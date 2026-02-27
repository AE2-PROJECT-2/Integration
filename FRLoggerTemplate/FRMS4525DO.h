// Template for wrapper for a ms4525do (differential pressure, pitot). It uses the FRSensor class, such that the Logger class can log the sensor.
// Note that this file is mostly empty. Fill in the necessary parts. Look at FRBMP280.h for reference
//
// 2026-02-27, Jos Meuleman, Inholland Aeronautical & Precision Engineering, The Netherlands

#ifndef FRMS4525DO_h
#define FRMS4525DO_h

#include <FRSensor.h>
#include <ms4525do.h>  // include the library for the ms4525do sensor
#include <Wire.h>

//creates class which inherents Sensor
class FRMS4525DO : public FRSensor {
public:
	// Constructor
	FRMS4525DO() {
		_myPitot = new bfs::Ms4525do();  // Allocate memory for the object
	}

	// Destructor
	virtual ~FRMS4525DO() {
		delete _myPitot;
	}

  // Initialisation of the sensor
	bool Init(TwoWire& myWire) {
		//_myPitot->Config(FILL IN STUFF HERE);
		// Start communication with the Pitot Sensor. If this fails, the program will stop here
		if (!_myPitot->Begin()) {
			return false;
		} else {
			return true;
		}
	}


	float GetPressure() {
		return _myPitot->pres_pa() - p0;
	}

	void AutoOffset() {
		// do your magic code here. Hint: use the P0
	}
	float GetSpeed() {
		// Do your magic code here
		return 0.0);
	}

	String HeaderString() override;
	String SensorString() override;

private:
	bfs::Ms4525do* _myPitot;
	const int PITOT_I2C_ADDRESS = 0x28;  // I2C address of the Pitot sensor
	float p0 = 0.0;
	
};

#endif