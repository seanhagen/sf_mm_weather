#include <station.h>

void Station::loop() {
  now = millis();
  if (now - lastSecond >= 1000) {
    lastSecond += 1000;
    // Serial.println("Second has passed!");

    seconds++;
    if (seconds > 59) {
      Serial.println("It's been a minute");
      _minuteReset();

      minutes++;
      if (minutes > 59) {
        Serial.println("Whew! It's been a hour!");
        _hourReset();

        hours++;
        if (hours > 23) {
          Serial.println("Oh wow, it's been a whole day!");
          _dayReset();
        }
      }
    }
    if (!sleeping) {
      // Serial.print("Not sleeping, getting readings: ");
      // Serial.print("wind, ");
      _loopWind();
      // Serial.print("lightning, ");
      _loopAS3935();
      // Serial.print("uv, ");
      _loopVEML6075();
      // Serial.print("atmosphere ");
      _loopBME280();
      // Serial.println(" ...done getting readings!");
    }
  }
}

void Station::_minuteReset() {
  seconds = 0;
  if (!sleeping) {
    _rainMinute();
    _windMinute();

    _strikeMinute();
    _bme280Minute();
    _veml6075Minute();
  }
}

void Station::_hourReset() {
  minutes = 0;

  if (!sleeping) {
    _rainHour();
    _windHour();

    _strikeHour();
    _bme280Hour();
    _veml6075Hour();
  }
}

void Station::_dayReset() {
  hours = 0;

  if (!sleeping) {
    _rainDay();

    _strikeDay();
  }
}
