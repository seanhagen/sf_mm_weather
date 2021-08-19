#include <station.h>

Station::Station(void) { _setup(); }

Station::Station(stationConfig conf) {
  _config = conf;
  _setup();
}

void Station::_setup() {
  randomSeed(micros());

  rain = _config.enableRain;
  wind = _config.enableWind;
  lightning = _config.enableLightning;
  uv = _config.enableUV;
  atmos = _config.enableAtmosphere;

  tempScale = _config.tempScale;

  lastSecond = millis();

  Wire.begin();
  _setupSensors();
}

void Station::_setupSensors() {
  Wire.begin();

  bool bme280setup = _setupBME280();
  bool veml6076setup = _setupVEML6075();
  bool as3935setup = _setupAS3935();
  bool windSetup = _setupWind();
  bool rainSetup = _setupRain();

  interrupts();
}
