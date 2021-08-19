#include <station.h>

Station::Station(void) {
  config c = config{
      enableRain = true, enableWind = true,       enableLightning = true,
      enableUV = true,   enableAtmosphere = true,
  };
  _setup(c);
}

Station::Station(config conf) { _setup(conf); }

Station::_setup(config conf) {
  rain = conf.enableRain;
  wind = conf.enableWind;
  lightning = conf.enableLightning;
  uv = conf.enableUV;
  atmos = conf.enableAtmosphere;

  lastSecond = millis();

  _setupSensors();
}

Station::_setupSensors() {
  Wire.begin();

  bool bme280setup = _setupBME280();
  bool veml6076setup = _setupVEML6075();
  bool as3935setup = _setupAS3935();
  bool windSetup = _setupWind();
  bool rainSetup = _setupRain();

  interrupts();
}

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

void Station::_setupVEML6075() {
  if (uv) {
    if (uv.begin(Wire) != VEML6075_SUCCESS) {
      // panic or return an error or SOMEHING
      return false;
    }
  }
  return true;
}

void Station::_setupAS3935() {
  if (lightning) {
    pinMode(AS3935_INT_PIN, INPUT);
    _as3935 = SparkFun_AS3935(_as3935_address);
    if (_as3935.begin() == false) {
      // panic or return an error or SOMEHING
      return false;
    }
  }
  return true;
}
