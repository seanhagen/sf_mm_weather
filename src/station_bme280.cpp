#include <station.h>

bool Station::_setupBME280() {
  if (atmos) {
    _bme280.settings.I2CAddress = _bme280_address;
    if (_bme280.beginI2C() == false) {
      // panic or return an error or SOMEHING
      return false;
    }
  }
  return true;
}

void Station::_loopBME280() {
  if (atmos) {
    BME280_SensorMeasurements reading;
    _bme280.readAllMeasurements(&reading, tempScale);
    tempLast = reading.temperature;

    _bme280Cache[BME280_TEMP][]
    // _bme280.
  }
}

// enum bucketPurpose {
//   ONE_MIN_AVG = 0,
//   TEN_MIN_AVG = 1,
//   ONE_HR_AVG = 2,
// };

// enum bme280SenType {
//   BME280_TEMP = 0,
//   BME280_HUM = 1,
//   BME280_PRES = 2,
// };
