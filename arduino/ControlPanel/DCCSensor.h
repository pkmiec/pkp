#ifndef DCCSensor_h
#define DCCSensor_h
#include "Arduino.h"

class DCCSensor
{
	public:
		DCCSensor(uint8_t pin, uint16_t debounce_ms = 100, int on_threshold = 512);

    uint8_t pin();
		void setup();
		bool read();
		bool toggled();
		bool started();
		bool stopped();
		bool has_changed();

		const static bool STARTED = true;
		const static bool STOPPED = false;

	private:
		uint8_t  _pin;
    int      _on_threshold;
		uint16_t _delay;
		bool     _state;
		uint32_t _ignore_until;
		bool     _has_changed;

    bool _read();
};

#endif
