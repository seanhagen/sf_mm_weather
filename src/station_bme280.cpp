#include <station.h>

bool Station::_setupBME280() {
  if (atmos) {
    _bme280.settings.I2CAddress = _bme280_address;

    _bme280_begin = _bme280.beginI2C();
    if (_bme280_begin == false) {
      Serial.println("The sensor did not respond. Please check wiring.");
      // panic or return an error or SOMEHING
      return false;
    }
  }
  return true;
}

void Station::_loopBME280() {
  if (atmos) {
    BME280_SensorMeasurements reading;
    _bme280.readAllMeasurements(&reading, _tempScale);
    tempLast = reading.temperature;
    humLast = reading.humidity;
    presLast = reading.pressure;

    bmeAvgs[BME280_TEMP][BUCKET_MINUTES][seconds] = tempLast;
    bmeAvgs[BME280_HUM][BUCKET_MINUTES][seconds] = humLast;
    bmeAvgs[BME280_PRES][BUCKET_MINUTES][seconds] = presLast;
  }
}

void Station::_bme280Minute() {
  if (atmos) {
    float t = 0, h = 0, p = 0;

    for (int i = 0; i < AVG_BUCKET_SIZE; i++) {
      t += bmeAvgs[BME280_TEMP][BUCKET_MINUTES][i];
      h += bmeAvgs[BME280_HUM][BUCKET_MINUTES][i];
      p += bmeAvgs[BME280_PRES][BUCKET_MINUTES][i];
    }

    tempMinAvg = t / 60.0;
    humMinAvg = h / 60.0;
    presMinAvg = p / 60.0;

    bmeAvgs[BME280_TEMP][BUCKET_HOURS][minutes] = tempMinAvg;
    bmeAvgs[BME280_HUM][BUCKET_HOURS][minutes] = humMinAvg;
    bmeAvgs[BME280_PRES][BUCKET_HOURS][minutes] = presMinAvg;
  }
}

void Station::_bme280Hour() {
  if (atmos) {
    float t = 0, h = 0, p = 0;
    for (int i = 0; i < AVG_BUCKET_SIZE; i++) {
      t += bmeAvgs[BME280_TEMP][BUCKET_HOURS][i];
      h += bmeAvgs[BME280_HUM][BUCKET_HOURS][i];
      p += bmeAvgs[BME280_PRES][BUCKET_HOURS][i];
    }

    tempHourAvg = t / 60.0;
    humHourAvg = h / 60.0;
    presHourAvg = p / 60.0;
  }
}

void Station::_BME280Wake() {
  if (atmos) {
    _bme280.setMode(MODE_NORMAL);
    // Wait for sensor to start measurment
    while (_bme280.isMeasuring() == false)
      ;
    // Hang out while sensor completes the reading
    while (_bme280.isMeasuring() == true)
      ;
  }
}

void Station::_BME280Sleep() {
  if (atmos) {
    _bme280.setMode(MODE_SLEEP);
  }
}

void Station::setStandbyTime(uint8_t timeSetting) {
  if (atmos) {
    _bme280.setStandbyTime(timeSetting);
  }
}

void Station::setTempOverSample(uint8_t overSampleAmount) {
  if (atmos) {
    _bme280.setTempOverSample(overSampleAmount);
  }
}

void Station::setPressureOverSample(uint8_t overSampleAmount) {
  if (atmos) {
    _bme280.setPressureOverSample(overSampleAmount);
  }
}

void Station::setHumidityOverSample(uint8_t overSampleAmount) {
  if (atmos) {
    _bme280.setHumidityOverSample(overSampleAmount);
  }
}

void Station::setReferencePressure(float refPressure) {
  if (atmos) {
    _bme280.setReferencePressure(refPressure);
  }
}

float Station::getReferencePressure() {
  if (atmos) {
    return _bme280.getReferencePressure();
  }
  return 0;
}

void Station::setTemperatureCorrection(float corr) {
  if (atmos) {
    _bme280.setTemperatureCorrection(corr);
  }
}

// tempScale = 0 for Celsius scale (default setting)
// tempScale = 1 for Fahrenheit scale
void Station::setTempScale(byte tempScale) {
  switch (tempScale) {
  case 0:
    _tempScale = 0;
  case 1:
  default:
    _tempScale = 1;
  }
}

float Station::temperature() { return tempLast; }

float Station::oneMinAvgTemp() { return tempMinAvg; }

float Station::oneHourAvgTemp() { return tempHourAvg; }

float Station::airPressure() { return presLast; }

float Station::oneMinAvgPressure() { return presMinAvg; }

float Station::oneHourAvgPressure() { return presHourAvg; }

float Station::humidity() { return humLast; }

float Station::oneMinAvgHumidity() { return humMinAvg; }

float Station::oneHourAvgHumidity() { return humHourAvg; }

double Station::dewPointC() {
  if (atmos) {
    return _bme280.dewPointC();
  }
  return -1.0;
}

double Station::dewPointF() {
  if (atmos) {
    return _bme280.dewPointF();
  }
  return -1.0;
}
