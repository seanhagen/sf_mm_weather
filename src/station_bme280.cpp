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
    // _bme280.
  }
}
