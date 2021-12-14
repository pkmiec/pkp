#include "DCCSensor.h"
#include <Arduino.h>

DCCSensor::DCCSensor(uint8_t pin, uint16_t debounce_ms, int on_threshold)
:  _pin(pin)
,  _delay(debounce_ms)
,  _state(STOPPED)
,  _ignore_until(0)
,  _has_changed(false)
,  _on_threshold(on_threshold)
{
}

void DCCSensor::setup()
{
	pinMode(_pin, INPUT);
}

//
// public methods
//

uint8_t DCCSensor::pin()
{
  return _pin;
}

bool DCCSensor::read()
{
	// ignore pin changes until after this delay time
	if (_ignore_until > millis())
	{
		// ignore any changes during this period
	}

	// pin has changed
	else if (_read() != _state)
	{
		_ignore_until = millis() + _delay;
		_state = !_state;
		_has_changed = true;
	}

	return _state;
}

// has the button been toggled from on -> off, or vice versa
bool DCCSensor::toggled()
{
	read();
	return has_changed();
}

// mostly internal, tells you if a button has changed after calling the read() function
bool DCCSensor::has_changed()
{
	if (_has_changed)
	{
		_has_changed = false;
		return true;
	}
	return false;
}

// has the button gone from off -> on
bool DCCSensor::started()
{
	return (read() == STARTED && has_changed());
}

// has the button gone from on -> off
bool DCCSensor::stopped()
{
	return (read() == STOPPED && has_changed());
}

//
// private methods
//

bool DCCSensor::_read()
{
  analogRead(_pin) <= _on_threshold;
}