#include <station.h>

void Station::loop() {
  if (millis() - lastSecond >= 1000) {
    lastSecond += 1000;

    // Take a speed and direction reading every second for 2 minute average
    if (++seconds_2m > 119)
      seconds_2m = 0;

    seconds++;
    if (seconds > 59) {
      seconds = 0;
      // set rainM to 0, even if rain isn't enabled, because otherwise
      // trying to do this in a separate function will just suck.
      // same goes for rainH & rainD
      rainM = 0;

      minutes++;
      if (minutes > 59) {
        minutes = 0;
        rainH = 0;

        hour++;
        if (hour > 23) {
          hour = 0;
          rainD = 0;
        }
      }
      // rainHour[minutes] = 0;

      if (++minutes_10m > 9) {
        minutes_10m = 0;
        rain_10m[minutes_10m] = 0;
        windGust_10m[minutes_10m] = 0;
      }

      // zeroWindGust10m(minutes_10m);
    }
  }

  _loopRain();
  _loopWind();
  _loopAS3935();
  _loopVEML6075();
  _loopBME280();
}
