#include <station.h>

bool Station::_setupWind() {
  if (wind) {
    pinMode(WIND_SPEED_PIN, INPUT_PULLUP);
    attachInterrupt(WIND_SPEED_PIN, _windSpeedIRQ, FALLING);
  }
  return true;
}

void Station::_loopWind() {
  if (wind) {
    // Calc the wind speed and direction every second for 120 second to get 2
    // minute average
    float deltaTime = millis() - lastWindCheck; // 750ms

    deltaTime /= 1000.0; // Covert to seconds

    nointerrupts();
    float windSpeed = (float)windClicks / deltaTime; // 3 / 0.750s = 4
    windClicks = 0; // Reset and start watching for new wind
    interrupts();
    lastWindCheck = millis();

    float currentSpeed = windSpeed * CLICKS_TO_KPH; // 4 * 1.492 = 5.968MPH

    windSpeedKPH = currentSpeed;

    windSpdAvg[seconds_2m] = (int)currentSpeed;

    // Check to see if this is a gust for the minute
    if (currentSpeed > windGust_10m[minutes_10m]) {
      windGust_10m[minutes_10m] = currentSpeed;
    }

    float temp = 0;
    for (int i = 0; i < TWO_MIN_AVG_SIZE; i++)
      temp += windSpdAvg[i];
    windSpdAvgKPH = temp / TWO_MIN_AVG_SIZE_F;

    rawWindDir = analogRead(WIND_VANE_PIN);
    windVoltage = rawWindDir * (5.0 / 4095.0);

    windVaneDir vd = UNKNOWN;

    // South
    if (windVoltage >= 1.20 && windVoltage <= 1.35) {
      vd = SOUTH;
    }

    // South_SouthWest
    if (windVoltage >= 1.0 && windVoltage <= 1.15) {
      vd = SOUTH_SOUTHWEST;
    }

    // SouthWest
    if (windVoltage > 2.95 && windVoltage < 3.4) {
      vd = SOUTHWEST;
    }

    // West_SouthWest
    if (windVoltage >= 2.85 && windVoltage <= 2.95) {
      vd = WEST_SOUTHWEST;
    }

    // West
    if (windVoltage >= 4.75 && windVoltage <= 5.0) {
      vd = WEST;
    }

    // West_NorthWest
    if (windVoltage >= 4.0 && windVoltage <= 4.15) {
      vd = WEST_NORTHWEST;
    }

    // NorthWest
    if (windVoltage >= 4.40 && windVoltage <= 4.50) {
      vd = NORTHWEST;
    }

    // North_NorthWest -- 3.40
    if (windVoltage >= 3.40 && windVoltage <= 3.6) {
      vd = NORTH_NORTHWEST;
    }

    // North
    if (windVoltage >= 3.80 && windVoltage <= 3.95) {
      vd = NORTH;
    }

    // North_NorthEast
    if (windVoltage >= 1.85 && windVoltage <= 1.90) {
      vd = NORTH_NORTHEAST;
    }

    // NorthEast
    if (windVoltage >= 2.10 && windVoltage <= 2.22) {
      vd = NORTHEAST;
    }

    // East_NorthEast
    if (windVoltage >= 0.20 && windVoltage < 0.25) {
      vd = EAST_NORTHEAST;
    }

    // East
    if (windVoltage >= 0.25 && windVoltage <= 0.34) {
      vd = EAST;
    }

    // East_SouthEast
    if (windVoltage >= 0.10 && windVoltage <= 0.15) {
      vd = EAST_SOUTHEAST;
    }

    // SouthEast
    if (windVoltage >= 0.65 && windVoltage <= 0.85) {
      vd = SOUTHEAST;
    }

    // South_SouthEast
    if (windVoltage >= 0.40 && windVoltage <= 0.50) {
      vd = SOUTH_SOUTHEAST;
    }

    currentWindDir = vd;
    windDirAvg[seconds_2m] = vd;

    long sum = windDirAvg[0];
    int D = windDirAvg[0];
    for (int i = 1; i < TWO_MIN_AVG_SIZE; i++) {
      int delta = windDirAvg[i] - D;

      if (delta < -180)
        D += delta + 360;
      else if (delta > 180)
        D += delta - 360;
      else
        D += delta;

      sum += D;
    }
    int winddir_avg2m = sum / WIND_DIR_AVG_SIZE;
    if (winddir_avg2m >= 360)
      winddir_avg2m -= 360;
    if (winddir_avg2m < 0)
      winddir_avg2m += 360;

    // TODO: map degrees to direction and store in avgWindDir2m
  }
}

void Station::_windSpeedIRQ() {
  // Activated by the magnet in the anemometer (2 ticks per rotation),
  // attached to input D3
  // Ignore switch-bounce glitches less than 10ms (142MPH max reading)
  // after the reed switch closes
  if (millis() - lastWindIRQ > 10) {
    lastWindIRQ = millis(); // Grab the current time
    windClicks++;           // There is 1.492MPH for each click per second.
  }
}
