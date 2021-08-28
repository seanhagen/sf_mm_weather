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

  _bme280_address = _config.bme280_i2c;
  _as3935_address = _config.as3935_i2c;
  _as3935_cs = _config.as3935_cs;

  //     int as3935_cs = 10;
  // uint8_t as3935_i2c = 10;

  _tempScale = _config.tempScale;

  lastSecond = millis();
}

void Station::begin() {
  Wire.begin();
  SPI.begin();

  if (!_setupBME280()) {
    Serial.println("Unable to setup BME280 sensor!");
    while (true)
      ;
  }
  if (!_setupVEML6075()) {
    Serial.println("Unable to setup VEML6075 sensor!");
    while (true)
      ;
  }

  if (!_setupAS3935()) {
    Serial.println("Unable to setup AS3935 sensor!");
    while (true)
      ;
  }

  if (!_setupWind()) {
    Serial.println("Unable to setup wind sensors!");
    while (true)
      ;
  }

  if (!_setupRain()) {
    Serial.println("Unable to setup rain sensor!");
    while (true)
      ;
  }

  interrupts();

  Serial.print("Waiting a bit for sensors to warm up...");
  delay(1000);
  Serial.println("done!");
}
