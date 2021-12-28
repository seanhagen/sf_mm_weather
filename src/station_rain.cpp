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

void Station::_loopRain() {
  if (rain) {
    // float temp = 0;
    // for (int i = 0; i < ONE_MIN_AVG_SIZE; i++)
    //   temp += rainFallAvg[i];
    // rainAvg = temp / ONE_MIN_AVG_SIZE_F;
  }
}

void Station::_handleRainIRQ() { Station::_rainSelf->_rainIRQ(); }

void Station::_rainIRQ() {
  rainTime = millis();
  rainInterval = rainTime - rainLast;

  // ignore switch-bounce glitches less than 10mS after initial edge
  if (rainInterval > 10) {
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
      tempM += rainfallMinuteAvg[i];
    }
    rainMinuteAvg = tempM > 0 ? tempM / ONE_MIN_AVG_SIZE_F : 0;
  }
}

void Station::_rainTenMinute() {
  if (rain) {
    // rain_10m[minutes_10m] = 0;
  }
}

void Station::_rainHour() {
  if (rain) {
    rainH = 0;
    rainHourAvg = 0;

    float tempH = 0;
    for (int i = 0; i < ONE_MIN_AVG_SIZE; i++) {
      tempH += rainfallHourAvg[i];
    }
    rainHourAvg = tempH > 0 ? tempH / ONE_MIN_AVG_SIZE_F : 0;
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
