#include <station.h>
#include <wind.h>

Station *Station::_windSelf;

bool Station::_setupWind() {
  if (wind) {
    pinMode(WIND_SPEED_PIN, INPUT_PULLUP);
    attachInterrupt(WIND_SPEED_PIN, _handleWindIRQ, FALLING);
    _windSelf = this;
  }
  return true;
}

void Station::_handleWindIRQ() { Station::_windSelf->_windSpeedIRQ(); }

void Station::_windSpeedIRQ() {
  // Activated by the magnet in the anemometer (2 ticks per rotation), attached
  // to input D3 Ignore switch-bounce glitches less than 10ms (142MPH max
  // reading) after the reed switch closes
  if (millis() - lastWindIRQ > 10) {
    lastWindIRQ = millis(); // Grab the current time
    windClicks++;           // There is 1.492MPH for each click per second.
  }
}

void Station::_loopWind() {
  if (wind) {
    // Calc the wind speed and direction every second for 120 second to get 2
    // minute average
    float deltaTime = millis() - lastWindCheck; // 750ms

    deltaTime /= 1000.0; // Covert to seconds

    noInterrupts();
    float windSpeed = (float)windClicks / deltaTime; // 3 / 0.750s = 4
    windClicks = 0; // Reset and start watching for new wind
    interrupts();
    lastWindCheck = millis();

    float currentSpeed = windSpeed * CLICKS_TO_KPH; // 4 * 1.492 = 5.968MPH
    windSpeedKPH = currentSpeed;
    ws_minAvg[seconds] = currentSpeed;
    ws_hourAvg[minutes] = currentSpeed;

    _readVane();
  }
}

windVaneDir Station::_voltToDir(float in) {
  // Serial.print("wind voltage: '");
  // Serial.print(in);
  // Serial.print("', direction: ");

  // North
  if (windVoltage >= 3.80 && windVoltage <= 3.95) {
    // Serial.println(" north ");
    return NORTH;
  }

  // North_NorthWest
  if (windVoltage >= 3.35 && windVoltage <= 3.6) {
    // Serial.println(" north north-west ");
    return NORTH_NORTHWEST;
  }

  // NorthWest
  if (windVoltage >= 4.40 && windVoltage <= 4.50) {
    // Serial.println(" north-west ");
    return NORTHWEST;
  }

  // West_NorthWest
  if (windVoltage >= 4.0 && windVoltage <= 4.15) {
    // Serial.println(" west north-west ");
    return WEST_NORTHWEST;
  }

  // West
  if (windVoltage >= 4.75 && windVoltage <= 5.0) {
    // Serial.println(" west ");
    return WEST;
  }

  // West_SouthWest
  if (windVoltage >= 2.85 && windVoltage <= 2.95) {
    // Serial.println(" west south-west ");
    return WEST_SOUTHWEST;
  }

  // SouthWest
  if (windVoltage > 2.95 && windVoltage < 3.35) {
    // Serial.println(" south-west ");
    return SOUTHWEST;
  }

  // South_SouthWest
  if (windVoltage >= 1.0 && windVoltage <= 1.15) {
    // Serial.println(" south south-west ");
    return SOUTH_SOUTHWEST;
  }

  // South
  if (windVoltage >= 1.20 && windVoltage <= 1.35) {
    // Serial.println(" south ");
    return SOUTH;
  }

  // South_SouthEast
  if (windVoltage >= 0.40 && windVoltage <= 0.50) {
    // Serial.println(" south south-east");
    return SOUTH_SOUTHEAST;
  }

  // SouthEast
  if (windVoltage >= 0.65 && windVoltage <= 0.85) {
    // Serial.println(" south-east ");
    return SOUTHEAST;
  }

  // East_SouthEast
  if (windVoltage >= 0.10 && windVoltage <= 0.15) {
    // Serial.println(" east south-east ");
    return EAST_SOUTHEAST;
  }

  // East
  if (windVoltage >= 0.25 && windVoltage <= 0.34) {
    // Serial.println(" east ");
    return EAST;
  }

  // East_NorthEast
  if (windVoltage >= 0.20 && windVoltage < 0.25) {
    // Serial.println(" east north-east");
    return EAST_NORTHEAST;
  }

  // NorthEast
  if (windVoltage >= 2.10 && windVoltage <= 2.22) {
    // Serial.println("north-east ");
    return NORTHEAST;
  }

  // North_NorthEast
  if (windVoltage >= 1.85 && windVoltage <= 1.90) {
    // Serial.println(" north north-east ");
    return NORTH_NORTHEAST;
  }

  // Serial.println(" unknown? ");

  return UNKNOWN;
}

void Station::_readVane() {
  rawWindDir = analogRead(WIND_VANE_PIN);
  windVoltage = rawWindDir * (5.0 / 4095.0);
  currentWindDir = _voltToDir(windVoltage);
}

void Station::_windMinute() {
  if (wind) {
    float tempM = 0;
    for (int i = 0; i < ONE_MIN_AVG_SIZE; i++) {
      tempM += ws_minAvg[i];
    }
    windSpeedKPH_minAvg = tempM > 0 ? tempM / 60.0 : 0;
    // TODO: map degrees to direction and store in avgWindDirOneMin
  }
}

void Station::_windHour() {
  if (wind) {
    float tempH = 0;
    for (int i = 0; i < ONE_MIN_AVG_SIZE; i++) {
      tempH += ws_hourAvg[i];
    }
    windSpeedKPH_hourAvg = tempH > 0 ? tempH / 60.0 : 0;
  }
}

float Station::currentWindSpeedKPH() { return windSpeedKPH; }

float Station::oneMinAvgWindSpeedKPH() { return windSpeedKPH_minAvg; }

float Station::oneHourAvgWindSpeedKPH() { return windSpeedKPH_hourAvg; }

windVaneDir Station::currentWindHeading() { return currentWindDir; }

char *Station::currentWindDirStr() {
  char *str = (char *)malloc(20 * sizeof(char));

  switch (currentWindDir) {
  case SOUTH:
    strcpy(str, "South");
    break;

  case SOUTH_SOUTHWEST:
    strcpy(str, "South South-West");
    break;

  case SOUTHWEST:
    strcpy(str, "South-West");
    break;

  case WEST_SOUTHWEST:
    strcpy(str, "West South-West");
    break;

  case WEST:
    strcpy(str, "West");
    break;

  case WEST_NORTHWEST:
    strcpy(str, "West North-West");
    break;

  case NORTHWEST:
    strcpy(str, "North-West");
    break;

  case NORTH_NORTHWEST:
    strcpy(str, "North North-West");
    break;

  case NORTH:
    strcpy(str, "North");
    break;

  case NORTH_NORTHEAST:
    strcpy(str, "North North-East");
    break;

  case NORTHEAST:
    strcpy(str, "North-East");
    break;

  case EAST_NORTHEAST:
    strcpy(str, "East North-East");
    break;

  case EAST:
    strcpy(str, "East");
    break;

  case EAST_SOUTHEAST:
    strcpy(str, "East South-East");
    break;

  case SOUTHEAST:
    strcpy(str, "South-East");
    break;

  case SOUTH_SOUTHEAST:
    strcpy(str, "South South-East");
    break;

  default:
    strcpy(str, "Unknown");
    break;
  }
  return str;
}
