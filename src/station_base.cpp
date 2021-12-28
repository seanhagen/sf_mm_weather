#include <station.h>

void Station::readAll(allMeasurements *measurements) {
  measurements->immediate = (immediateMeasurements){
      .temperature = tempLast,
      .humidity = humLast,
      .pressure = presLast,
      .uva = uvaLast,
      .uvb = uvbLast,
      .uvIndex = uviLast,
      .windDir = currentWindDir,
  };

  int c = 0;
  for (int i = 0; i < 24; i++) {
    c += _strikesDay[i];
  }

  measurements->counted = (countedMeasurements){
      .rainLastMinute = rainM,
      .rainLastHour = rainH,
      .rainToday = rainD,

      .windSpeedNow = windSpeedKPH,
      .windSpeedMinuteAvg = windSpeedKPH_minAvg,
      .windSpeedHourAvg = windSpeedKPH_hourAvg,

      .lightningLastMinute = _strikesMinute[seconds],
      .lightningLastHour = _strikesHour[minutes],
      .lightningToday = c,
  };
}

bool Station::isRain() { return rain; }
bool Station::isWind() { return wind; }
bool Station::isLightning() { return lightning; }
bool Station::isUv() { return uv; }
bool Station::isAtmos() { return atmos; }
