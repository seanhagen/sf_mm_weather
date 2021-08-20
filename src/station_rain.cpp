#include <station.h>

const int mmPerClick = 2.794;

Station *Station::_rainSelf;

bool Station::_setupRain() {
  if (rain) {
    pinMode(RAIN_PIN, INPUT_PULLUP);
    attachInterrupt(RAIN_PIN, Station::_handleRainIRQ, FALLING);
    _rainSelf = this;
  }
  return true;
}

void Station::_loopRain() {
  if (rain) {
    float temp = 0;
    for (int i = 0; i < ONE_MIN_AVG_SIZE; i++)
      temp += rainFallAvg[i];
    rainAvg = temp / ONE_MIN_AVG_SIZE_F;
  }
}

void Station::_handleRainIRQ() { Station::_rainSelf->_rainIRQ(); }

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
