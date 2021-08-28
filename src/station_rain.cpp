#include <station.h>

Station *Station::_rainSelf;

bool Station::_setupRain() {
  if (rain) {
    pinMode(RAIN_PIN, INPUT_PULLUP);
    attachInterrupt(RAIN_PIN, Station::_handleRainIRQ, FALLING);
    _rainSelf = this;
  }
  return true;
}

void Station::_handleRainIRQ() { Station::_rainSelf->_rainIRQ(); }

void Station::_rainIRQ() {
  rainTime = millis();
  rainInterval = rainTime - rainLast;

  // ignore switch-bounce glitches less than 10mS after initial edge
  if (rainInterval > 10) {
    rainfallMinuteAvg[seconds] += mmPerClick;
    rainfallHourAvg[minutes] += mmPerClick;

    rainM += mmPerClick;
    rainH += mmPerClick;
    rainD += mmPerClick;

    rainLast = rainTime;
  }
}

void Station::_rainMinute() {
  if (rain) {
    rainM = 0;
    rainMinuteAvg = 0;

    float tempM = 0;
    for (int i = 0; i < ONE_MIN_AVG_SIZE; i++) {
      if (rainfallMinuteAvg[i] > 0) {
        tempM += rainfallMinuteAvg[i];
      }
    }

    if (tempM > 0) {
      rainMinuteAvg = tempM / ONE_MIN_AVG_SIZE_F;
    }
  }
}

void Station::_rainHour() {
  if (rain) {
    rainH = 0;
    rainHourAvg = 0;

    float tempH = 0;
    for (int i = 0; i < ONE_MIN_AVG_SIZE; i++) {
      if (rainfallHourAvg[i] > 0) {
        tempH += rainfallHourAvg[i];
      }
    }

    if (tempH > 0) {
      rainHourAvg = tempH / ONE_MIN_AVG_SIZE_F;
    }
  }
}

void Station::_rainDay() {
  if (rain) {
    rainD = 0;
  }
}

bool Station::isRaining() {
  if (rainLast <= 60 * 1000) {
    return true;
  }
  return false;
}

float Station::rainLastMinute() { return rainM; }

float Station::rainAvgLastMinute() { return rainMinuteAvg; }

float Station::rainLastHour() { return rainH; }

float Station::rainAvgLastHour() { return rainHourAvg; }

float Station::rainToday() { return rainD; }
