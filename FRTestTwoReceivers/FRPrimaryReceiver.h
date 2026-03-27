// Class for receiving PPM signals
// It uses the FRSensor class, such that the Logger class can log the sensor.
//
// 2024-03-07, Jos Meuleman, Inholland Aeronautical & Precision Engineering, The Netherlands

#ifndef FRPrimaryReceiver_h
#define FRPrimaryReceiver_h

#include <FRSensor.h>
#include <FRGeneric.h> //Needed for converting data to strings

class FRPrimaryReceiver : public FRSensor {
public:
	FRPrimaryReceiver(byte pinNumber, byte numberOfChannels) {
		//_instance = this;  // Store a pointer to this instance
		_numberOfChannels = numberOfChannels;
		_pinNumber = pinNumber;

		if (_channelValues != 0) {
			delete[] _channelValues;
		}
		_channelValues = new int[_numberOfChannels];

		for (byte i = 0; i < _numberOfChannels; i++) {
			_channelValues[i] = 0;
		}
		_channel = 0;
	}

	~FRPrimaryReceiver() {
		detachInterrupt(_pinNumber);
		delete[] _channelValues;
	}

	void Init() {
		pinMode(_pinNumber, INPUT);
		attachInterruptArg(digitalPinToInterrupt(_pinNumber), primary_ppm_isr, this, CHANGE);
		_lastPulseUS = micros();
	}

	void SetPrefix(String prefix) {
		_prefix = prefix;
	}

	int ReadChannel(int ChannelNumber) {
		if ((ChannelNumber >= 0) && (ChannelNumber < _numberOfChannels)) {
			return (_channelValues[ChannelNumber]);
		} else {
			return (0);
		}
	}

	String HeaderString() override {
		String tempString;
		for (byte i = 0; i < _numberOfChannels; i++) {
			tempString.concat(_prefix);
			tempString.concat(i);
			tempString.concat("; ");
		}
		return tempString;
	}

	String SensorString() override {
		String tempString;
		for (byte i = 0; i < _numberOfChannels; i++) {
			tempString.concat(createIntString(_channelValues[i]));
		}
		return tempString;
	}

private:
	byte _pinNumber;
	byte _numberOfChannels;
	volatile unsigned long _lastPulseUS;
	volatile uint16_t _channel;
	volatile int* _channelValues;

	//static FRPrimaryReceiver* _instance;
	const int _MAXPULSEWIDTH = 1800;
	const int _MINPULSEWIDTH = 400;
	const int _PULSETHRESHUS = 3000;  // Threshold for pulse duration in microseconds
	String _prefix = "Ch";

	void CountPulse() {
		unsigned long tNow = micros();
		if (digitalRead(_pinNumber)) {
			_lastPulseUS = micros();
		} else {
			long x = tNow - _lastPulseUS;
			_lastPulseUS = tNow;
			if (x > _PULSETHRESHUS) {
				_channel = 0;
			} else {
				if (_channel < _numberOfChannels) {
					_channelValues[_channel] = x;
					_channel++;
				}
			}
		}
	}

	static void primary_ppm_isr(void* arg) {
		FRPrimaryReceiver* receiver = reinterpret_cast<FRPrimaryReceiver*>(arg);
		receiver->CountPulse();
	}
};


#endif