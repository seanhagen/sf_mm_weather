#include <station.h>

void Station::loop() {
  if (millis() - lastSecond >= 1000) {
    lastSecond += 1000;

    seconds++;
    if (seconds > 59) {
      _minuteReset();

      minutes++;
      if (minutes > 59) {
        _hourReset();

        hour++;
        if (hour > 23) {
          _dayReset();
        }
      }

      minutes_10m++;
      if (minutes_10m > 9) {
        _10MinReset();
      }
    }
  }

  // TODO: only do one each loop -- have an looping index counter stored
  // and use a switch to only do one each loop
  _loopRain();
  _loopWind();
  _loopAS3935();
  _loopVEML6075();
  _loopBME280();
}

void Station::_10MinReset() {
  minutes_10m = 0;
  rain_10m[minutes_10m] = 0;
  windGust_10m[minutes_10m] = 0;
  // zeroWindGust10m(minutes_10m);

  _uvTenMinute();
}

void Station::_minuteReset() {
  seconds = 0;
  // set rainM to 0, even if rain isn't enabled, because otherwise
  // trying to do this in a separate function will just suck.
  // same goes for rainH & rainD
  rainM = 0;
  // rainHour[minutes] = 0;
  _uvMinute();
}

void Station::_hourReset() {
  minutes = 0;
  rainH = 0;
}

void Station::_dayReset() {
  hour = 0;
  rainD = 0;
}
