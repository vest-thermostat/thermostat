#include "Atm_DHT.h"

/* Add optional parameters for the state machine to begin()
 * Add extra initialization code
 */

Atm_DHT& Atm_DHT::begin(uint8_t pin, uint8_t type, uint16_t delay) {
  // clang-format off
  const static state_t state_table[] PROGMEM = {
    /*                   ON_ENTER  ON_LOOP  ON_EXIT  EVT_INCORRECT_DATAS  EVT_CORRECT_DATAS  EVT_TIMEOUT  EVT_START  ELSE */
    /*      IDLE */            -1,      -1,      -1,                  -1,                -1,          -1,   WAITING,   -1,
    /*   WAITING */   ENT_WAITING,      -1,      -1,                  -1,                -1,      FINISH,        -1,   -1,
    /*    FINISH */    ENT_FINISH,      -1,      -1,             WAITING,         NEW_DATAS,          -1,        -1,   -1,
    /* NEW_DATAS */ ENT_NEW_DATAS,      -1,      -1,                  -1,                -1,          -1,   WAITING,   -1,
  };
  // clang-format on
  Machine::begin( state_table, ELSE );

  _pin = pin;
  _type = type;
  _delay = delay;

  return *this;
}

/* Add C++ code for each internally handled event (input)
 * The code must return 1 to trigger the event
 */

int Atm_DHT::event( int id ) {
  switch ( id ) {
    case EVT_INCORRECT_DATAS:
      return 0;
    case EVT_CORRECT_DATAS:
      return 0;
    case EVT_TIMEOUT:
      return mstimer.expired(this);
    case EVT_START:
      return 0;
  }
  return 0;
}


void Atm_DHT::action( int id ) {
  switch ( id ) {
    case ENT_WAITING:
      mstimer.set(_delay);
      return;
    case ENT_FINISH:
      if (_read()) {
        _datas.temperature = getTemperature();
        _datas.humidity = getHumidity();
        _datas.heat_index = getHeatIndex(_datas.temperature, _datas.humidity);
        trigger(EVT_CORRECT_DATAS);
      } else {
        _datas.temperature = NAN;
        _datas.humidity = NAN;
        _datas.heat_index = NAN;
        trigger(EVT_INCORRECT_DATAS);
      }
      return;
    case ENT_NEW_DATAS:
      if (_callback) {
        _callback(_datas);
      }
      trigger(EVT_START);
      return;
  }
}


float Atm_DHT::getTemperature() {
  float f = NAN;

  switch (_type) {
    case DHT11:
      f = _rawdata[2];
      break;
    case DHT22:
    case DHT21:
      f = _rawdata[2] & 0x7F;
      f *= 256;
      f += _rawdata[3];
      f *= 0.1;
      if (_rawdata[2] & 0x80) {
        f *= -1;
      }
      break;
  }
  return f;
}


float Atm_DHT::getHumidity() {
  float f = NAN;

  switch (_type) {
    case DHT11:
      f = _rawdata[0];
      break;
    case DHT22:
    case DHT21:
      f = _rawdata[0];
      f *= 256;
      f += _rawdata[1];
      f *= 0.1;
      break;
  }
  return f;
}

float Atm_DHT::getHeatIndex(float temperature, float percentHumidity) {
  // Using both Rothfusz and Steadman's equations
  // http://www.wpc.ncep.noaa.gov/html/heatindex_equation.shtml
  float hi;

  hi = 0.5 * (temperature + 61.0 + ((temperature - 68.0) * 1.2) + (percentHumidity * 0.094));

  if (hi > 79) {
    hi = -42.379 +
             2.04901523 * temperature +
            10.14333127 * percentHumidity +
            -0.22475541 * temperature*percentHumidity +
            -0.00683783 * pow(temperature, 2) +
            -0.05481717 * pow(percentHumidity, 2) +
             0.00122874 * pow(temperature, 2) * percentHumidity +
             0.00085282 * temperature*pow(percentHumidity, 2) +
            -0.00000199 * pow(temperature, 2) * pow(percentHumidity, 2);

    if((percentHumidity < 13) && (temperature >= 80.0) && (temperature <= 112.0))
      hi -= ((13.0 - percentHumidity) * 0.25) * sqrt((17.0 - abs(temperature - 95.0)) * 0.05882);

    else if((percentHumidity > 85.0) && (temperature >= 80.0) && (temperature <= 87.0))
      hi += ((percentHumidity - 85.0) * 0.1) * ((87.0 - temperature) * 0.2);
  }

  return hi;
}

// Expect the signal line to be at the specified level for a period of time and
// return a count of loop cycles spent at that level (this cycle count can be
// used to compare the relative time of two pulses).  If more than a millisecond
// ellapses without the level changing then the call fails with a 0 response.
// This is adapted from Arduino's pulseInLong function (which is only available
// in the very latest IDE versions):
//   https://github.com/arduino/Arduino/blob/master/hardware/arduino/avr/cores/arduino/wiring_pulse.c
uint32_t Atm_DHT::expectPulse(bool level) {
  uint32_t count = 0;
  // On AVR platforms use direct GPIO port access as it's much faster and better
  // for catching pulses that are 10's of microseconds in length:
  #ifdef __AVR
    uint8_t portState = level ? _bit : 0;
    while ((*portInputRegister(_port) & _bit) == portState) {
      if (count++ >= _maxcycles) {
        return 0; // Exceeded timeout, fail.
      }
    }
  // Otherwise fall back to using digitalRead (this seems to be necessary on ESP8266
  // right now, perhaps bugs in direct port access functions?).
  #else
    while (digitalRead(_pin) == level) {
      if (count++ >= _maxcycles) {
        return 0; // Exceeded timeout, fail.
      }
    }
  #endif

  return count;
}


bool Atm_DHT::_read() {
  // Reset 40 bits of received _rawdata to zero.
  _rawdata[0] = _rawdata[1] = _rawdata[2] = _rawdata[3] = _rawdata[4] = 0;

  // Send start signal.  See DHT datasheet for full signal diagram:
  //   http://www.adafruit.com/datasheets/Digital%20humidity%20and%20temperature%20sensor%20AM2302.pdf

  // Go into high impedence state to let pull-up raise data line level and
  // start the reading process.
  digitalWrite(_pin, HIGH);
  delay(250);

  // First set data line low for 20 milliseconds.
  pinMode(_pin, OUTPUT);
  digitalWrite(_pin, LOW);
  delay(20);

  uint32_t cycles[80];
  {
    // Turn off interrupts temporarily because the next sections are timing critical
    // and we don't want any interruptions.
    InterruptLock lock;

    // End the start signal by setting data line high for 40 microseconds.
    digitalWrite(_pin, HIGH);
    delayMicroseconds(40);

    // Now start reading the data line to get the value from the DHT sensor.
    pinMode(_pin, INPUT_PULLUP);
    delayMicroseconds(10);  // Delay a bit to let sensor pull data line low.

    // First expect a low signal for ~80 microseconds followed by a high signal
    // for ~80 microseconds again.
    if (expectPulse(LOW) == 0) {
      DEBUG_PRINTLN(F("Timeout waiting for start signal low pulse."));
      return false;
    }
    if (expectPulse(HIGH) == 0) {
      DEBUG_PRINTLN(F("Timeout waiting for start signal high pulse."));
      return false;
    }

    // Now read the 40 bits sent by the sensor.  Each bit is sent as a 50
    // microsecond low pulse followed by a variable length high pulse.  If the
    // high pulse is ~28 microseconds then it's a 0 and if it's ~70 microseconds
    // then it's a 1.  We measure the cycle count of the initial 50us low pulse
    // and use that to compare to the cycle count of the high pulse to determine
    // if the bit is a 0 (high state cycle count < low state cycle count), or a
    // 1 (high state cycle count > low state cycle count). Note that for speed all
    // the pulses are read into a array and then examined in a later step.
    for (int i=0; i<80; i+=2) {
      cycles[i]   = expectPulse(LOW);
      cycles[i+1] = expectPulse(HIGH);
    }
  } // Timing critical code is now complete.

  // Inspect pulses and determine which ones are 0 (high state cycle count < low
  // state cycle count), or 1 (high state cycle count > low state cycle count).
  for (int i=0; i<40; ++i) {
    uint32_t lowCycles  = cycles[2*i];
    uint32_t highCycles = cycles[2*i+1];
    if ((lowCycles == 0) || (highCycles == 0)) {
      DEBUG_PRINTLN(F("Timeout waiting for pulse."));
      return false;
    }
    _rawdata[i/8] <<= 1;
    // Now compare the low and high cycle times to see if the bit is a 0 or 1.
    if (highCycles > lowCycles) {
      // High cycles are greater than 50us low cycle count, must be a 1.
      _rawdata[i/8] |= 1;
    }
    // Else high cycles are less than (or equal to, a weird case) the 50us low
    // cycle count so this must be a zero.  Nothing needs to be changed in the
    // stored data.
  }

  DEBUG_PRINTLN(F("Received:"));
  DEBUG_PRINT(_rawdata[0], HEX); DEBUG_PRINT(F(", "));
  DEBUG_PRINT(_rawdata[1], HEX); DEBUG_PRINT(F(", "));
  DEBUG_PRINT(_rawdata[2], HEX); DEBUG_PRINT(F(", "));
  DEBUG_PRINT(_rawdata[3], HEX); DEBUG_PRINT(F(", "));
  DEBUG_PRINT(_rawdata[4], HEX); DEBUG_PRINT(F(" =? "));
  DEBUG_PRINTLN((_rawdata[0] + _rawdata[1] + _rawdata[2] + _rawdata[3]) & 0xFF, HEX);

  // Check we read 40 bits and that the checksum matches.
  if (_rawdata[4] == ((_rawdata[0] + _rawdata[1] + _rawdata[2] + _rawdata[3]) & 0xFF)) {
    return true;
  }
  else {
    DEBUG_PRINTLN(F("Checksum failure!"));
    return false;
  }
}

/* Public event methods
 *
 */
Atm_DHT& Atm_DHT::incorrect_datas() {
  trigger( EVT_INCORRECT_DATAS );
  return *this;
}

Atm_DHT& Atm_DHT::correct_datas() {
  trigger( EVT_CORRECT_DATAS );
  return *this;
}

Atm_DHT& Atm_DHT::timeout() {
  trigger( EVT_TIMEOUT );
  return *this;
}

Atm_DHT& Atm_DHT::start() {
  trigger( EVT_START );
  return *this;
}

/* State trace method
 * Sets the symbol table and the default logging method for serial monitoring
 */

Atm_DHT& Atm_DHT::trace( Stream & stream ) {
  Machine::setTrace( &stream, atm_serial_debug::trace,
    "DHT\0EVT_INCORRECT_DATAS\0EVT_CORRECT_DATAS\0EVT_TIMEOUT\0EVT_START\0ELSE\0IDLE\0WAITING\0FINISH\0NEW_DATAS" );
  return *this;
}


Atm_DHT& Atm_DHT::onNewDatas(DHTCallback callback) {
  _callback = callback;
  return *this;
}
