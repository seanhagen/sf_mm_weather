#include <station.h>

bool Station::_setupAS3935() {
  for (int i = 0; i < AVG_BUCKET_SIZE; i++) {
    _strikesMinute[i] = 0;
    _strikesHour[i] = 0;
  }

  for (int i = 0; i < DAY_BUCKET_SIZE; i++) {
    _strikesDay[i] = 0;
  }

  if (lightning) {
    pinMode(AS3935_INT_PIN, INPUT);

    // // i2c setup
    // _as3935 = SparkFun_AS3935(_as3935_address);
    // if (_as3935.begin() == false) {
    //   // panic or return an error or SOMEHING
    //   return false;
    // }

    _as3935 = SparkFun_AS3935();
    // spi setup
    if (!_as3935.beginSPI(_as3935_cs)) {
      return false;
    }
  }

  return true;
}

void Station::_loopAS3935() {
  if (lightning && digitalRead(AS3935_INT_PIN) == HIGH) {
    uint8_t intVal = _as3935.readInterruptReg();
    // Serial.print("Lightning: ");
    // Serial.println(intVal);
    if (intVal == NOISE_INT) {
      // Too much noise? Uncomment the code below, a higher number means
      // better noise rejection.
      _as3935.setNoiseLevel(_noise);
      // return lightningInfo{false, true, false, 0};
    } else if (intVal == DISTURBER_INT) {
      // Too many disturbers? Uncomment the code below, a higher number means
      // better disturber rejection.
      _as3935.watchdogThreshold(_disturber);
      // return lightningInfo{false, false, true, 0};
    } else if (intVal == LIGHTNING_INT) {
      // Lightning! Now how far away is it? Distance estimation takes into
      // account any previously seen events in the last 15 seconds.
      _distance = _as3935.distanceToStorm();
      _distances[_distIdx] = _distance;
      _distIdx++;
      if (_distIdx > AS3935_AVG_SIZE) {
        _distIdx = 0;
      }

      _strikesMinute[seconds]++;
      _strikesHour[minutes]++;
      _strikesDay[hours]++;
    }
  }
}

bool Station::_AS3935Wake() {
  if (lightning) {
    return _as3935.wakeUp();
  }
  return true;
}

void Station::_AS3935Sleep() {
  if (lightning) {
    _as3935.powerDown();
  }
}

void Station::_strikeMinute() { _strikesMinute[seconds] = 0; }

void Station::_strikeHour() { _strikesHour[minutes] = 0; }

void Station::_strikeDay() { _strikesDay[hours] = 0; }

void Station::setIndoorOutdoor(uint8_t _setting) {
  _as3935.setIndoorOutdoor(_setting);
}

byte Station::distanceToStorm() { return _distance; }

byte Station::avgDistanceToStorm() {
  byte d = 0;
  int c = 0;
  for (int i = 0; i < AS3935_AVG_SIZE; i++) {
    if (_distances[i] > 0) {
      d += _distances[i];
      c++;
    }
  }
  return d / c;
}

int Station::strikeThisMinute() {
  int c = 0;
  for (int i = 0; i < AVG_BUCKET_SIZE; i++) {
    c += _strikesMinute[i];
  }
  return c;
}

int Station::strikeThisHour() {
  int c = 0;
  for (int i = 0; i < AVG_BUCKET_SIZE; i++) {
    c += _strikesHour[i];
  }
  return c;
}

int Station::strikeToday() {
  int c = 0;
  for (int i = 0; i < 24; i++) {
    c += _strikesDay[i];
  }
  return c;
}
