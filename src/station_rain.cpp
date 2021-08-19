#include <station.h>

const int mmPerClick = 2.794;

void Station::_setupRain() {
  if (rain) {
    pinMode(RAIN_PIN, INPUT_PULLUP);
    attachInterrupt(RAIN_PIN, _rainIRQ, FALLING);
  }
  return true;
}

void Station::_loopRain() {
  if (rain) {
    float temp = 0;
    for (int i = 0; i < TWO_MIN_AVG_SIZE; i++)
      temp += rainFallAvg[i];
    rainAvg = temp / TWO_MIN_AVG_SIZE_F;
  }
}
/*
 *   // last 10 minutes of rain
  float rain_10m[10];

  // 120 bytes to keep track of 2 minute average rainfall
  byte rainFallAvg[TWO_MIN_AVG_SIZE];

  // 2 minute average of rainfall in mm
  float rainAvg = 0;

*/
void Station::_rainIRQ() {
  rainTime = millis();
  rainInterval = rainTime - rainLast;

  // ignore switch-bounce glitches less than 10mS after initial edge
  if (rainInterval > 10) {
    rainFallAvg[seconds] += mmPerClick;

    rainM += mmPerClick;
    rain_10m[minutes_10m] += mmPerClick;

    rainH += mmPerClick;
    rainD += mmPerClick;

    rainLast = rainTime;
  }
}
