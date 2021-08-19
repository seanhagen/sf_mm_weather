#include <station.h>

void Station::_setupBME280() {
  if (atmos) {
    //_bme280
    _bme280.settings.I2CAddress = _bme280_address;
    if (_bme280.beginI2C() == false) {
      // panic or return an error or SOMEHING
      return false;
    }
  }
  return true;
}
