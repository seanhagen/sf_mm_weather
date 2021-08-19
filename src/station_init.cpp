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
