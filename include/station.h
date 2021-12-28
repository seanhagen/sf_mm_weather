/****************************************************************
 * SparkFun MicroMod Weather Station Carrier Board Helper Library
 *
 * This library is intended to help users read the various sensors on the
 * weather station carrier board.
 *
 * Because some of the sensors are momentary ( lightning, rain ),
 * some readings are stored as a value that relects the last 10 minutes. For
 * rain this is how many inches, for lightning it's number of strikes. All other
 * sensors report immediately, so their values are from "right now".
 *
 * Sensor information:
 *
 *  BME280 -- Atmospheric Sensor
 *   Temperature Range:
 *     Full Accuracy: 0°C-65°C (32°F-149°F)
 *     Operational: -40°C-85°C (-40°F-185°F)
 *   Humidity Range: 0-100% RH
 *   Pressure Range: 300-1100 hPa (30,000-110,000 Pa or approx. 4.35-15.95 PSI)
 *
 *  VEML6075 -- UV Light Sensor
 *    UVA Resolution: 0.93 counts/µW/cm2
 *    UVA Resolution: 2.1 counts/µW/cm2
 *
 *  AS3935 -- Lightning Detector
 *    40km Range with accuracy of 1km in 14 steps
 ****************************************************************
 */
#ifndef STATION_H
#define STATION_H

#define BME280_NUM_SEN 3

// two buckets, one for minutes, one for hours
#define NUM_BUCKET 2

enum bucketType {
  BUCKET_MINUTES = 0,
  BUCKET_HOURS = 1,
};

#define AVG_BUCKET_SIZE 60
#define DAY_BUCKET_SIZE 24

#define ONE_MIN_AVG_SIZE 60
#define ONE_MIN_AVG_SIZE_F 60.0

enum bme280SenType {
  BME280_TEMP = 0,
  BME280_HUM = 1,
  BME280_PRES = 2,
};

#define VEML6075_NUM_SEN 3
enum veml6075SenType {
  VEML6075_UVA = 0,
  VEML6075_UVB = 1,
  VEML6075_UVI = 2,
};

/*
 * All pin references:
 * ESP32:
 *   https://learn.sparkfun.com/tutorials/micromod-esp32-processor-board-hookup-guide/all
 * MicroMod Weather Carrier Board:
 *   https://learn.sparkfun.com/tutorials/micromod-weather-carrier-board-hookup-guide#hardware-overview
 */

#ifndef WIND_VANE_PIN
// A1 - GPIO pin 35, pad 38 on the MicroMod Weather Carrier Board (Input Only!)
#define WIND_VANE_PIN 35
#endif

#ifndef WIND_SPEED_PIN
// D0 - GPIO pin 14, pad 10 on the MicroMod Weather Carrier Board
#define WIND_SPEED_PIN 14
#endif

#ifndef RAIN_PIN
// D1 - GPIO pin 27, pad 18 on the MicroMod Weather Carrier Board
#define RAIN_PIN 27
#endif

#ifndef AS3935_INT_PIN
// The pin used for IRQ for the AS3935 lightning detector.
// G3 - GPIO pin 17, pad 46 on the MicroMod Weather Carrier Board
#define AS3935_INT_PIN 17
#endif

#ifndef AS3935_CS_PIN
// The chip-select pin used for the AS3935 lightning detector
// G1 - GPIO pin 25, pad 42 on the MicroMod Weather Carrier Board
#define AS3935_CS_PIN 35
#endif

#define AS3935_AVG_SIZE 10

#include <stdint.h>

#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>

#include <SparkFunBME280.h>
#include <SparkFun_AS3935.h>
// #include <SparkFun_VEML6075_Arduino_Library.h>
#include <Adafruit_VEML6075.h>

#include "lightning.h"
#include "wind.h"

struct stationConfig {
public:
  // will report rainfall readings from attached rain bucket
  bool enableRain = true;
  // will report wind direction & speed from the attached anemometer and wind
  // vane
  bool enableWind = true;
  // will report readings from the AS3935 lightning detection sensor if true
  bool enableLightning = true;
  // will report readings from the VEML6075 UV sensor if true
  bool enableUV = true;
  // will report readings from the BME280 temperature, humidity, and air
  // pressure sensor if true
  bool enableAtmosphere = true;

  // what scale to use for temperature, 0 for Celcius, literally anything else
  // for Fahrenheit
  byte tempScale = 0;

  // the SPI chip select pin for the as3935
  int as3935_cs = 10;
  uint8_t as3935_i2c = 10;

  // the i2c address for the bme280
  uint8_t bme280_i2c = 0x77;

  // the i2c address for the veml6075
  // uint8_t veml6075_i2c = 0x10;
};

// struct station_error {
// public:
//   VEML6075_error_t veml6075_error;
//   bool as3935_had_error;
// };

struct immediateMeasurements {
public:
  float temperature;
  float humidity;
  float pressure;

  float uva;
  float uvb;
  float uvIndex;

  windVaneDir windDir;
};

struct countedMeasurements {
public:
  float rainLastMinute;
  float rainLastHour;
  float rainToday;

  float windSpeedNow;
  float windSpeedMinuteAvg;
  float windSpeedHourAvg;

  int lightningLastMinute;
  int lightningLastHour;
  int lightningToday;
};

struct allMeasurements {
public:
  immediateMeasurements immediate;
  countedMeasurements counted;
};

class Station {
public:
  Station(void);
  Station(stationConfig conf);

  // initializes all enabled sensors
  void begin(void);

  // MUST BE CALLED WITHIN LOOP OF YOUR CODE!
  // NONE OF THIS WILL WORK OTHERWISE ( at least, not properly )
  void loop();

  // Reads all measurements
  void readAll(allMeasurements *measurements);

  // methods to see if various features are turned on or off
  bool isRain();
  bool isWind();
  bool isLightning();
  bool isUv();
  bool isAtmos();

  
  // // turns on all the sensors
  // station_error powerOn();
  // // shuts off all the sensors
  // station_error shutdown();

  /*****************************************************
   * BME280
   * This sensor reads temperature, humidity, and air
   * pressure.
   *
   * Datasheet:
   *  https://cdn.sparkfun.com/assets/learn_tutorials/4/1/9/BST-BME280_DS001-10.pdf
   *****************************************************/

  /* BME280 HELPERS  & SETUP FUNCTIONS */

  // Set the standby time between measurements
  void setStandbyTime(uint8_t timeSetting);

  // Set the temperature sample mode, 0 to 16 are valid. 0 disables temp
  // sensing. See table 24 in datasheet.
  void setTempOverSample(uint8_t overSampleAmount);
  // Set the pressure sample mode, 0 to 16 are valid. 0 disables temp sensing.
  // See table 23 in datasheet.
  void setPressureOverSample(uint8_t overSampleAmount);
  // Set the humidity sample mode, 0 to 16 are valid. 0 disables temp sensing.
  // See table 19 in datasheet.
  void setHumidityOverSample(uint8_t overSampleAmount);

  // Allows user to set local sea level reference pressure
  void setReferencePressure(float refPressure);
  float getReferencePressure();

  /* BME280 TEMPERATURE */
  void setTemperatureCorrection(float corr);
  // A value of 0 for temperatureScale means Celsius, any other value means
  // Fahrenheit.
  void setTempScale(byte tempScale);

  float temperature();
  float oneMinAvgTemp();
  float oneHourAvgTemp();

  /* BME280 AIR PRESSURE */
  float airPressure();
  float oneMinAvgPressure();
  float oneHourAvgPressure();

  /* BME280 HUMIDITY */
  float humidity();
  float oneMinAvgHumidity();
  float oneHourAvgHumidity();
  double dewPointC();
  double dewPointF();

  /*****************************************************
   * VEML6075
   * This sensor reads the level of UV light. It reports both UVA, UVB, and can
   * calculate the UV Index.
   *
   * Datasheet:
   *  https://cdn.sparkfun.com/assets/3/c/3/2/f/veml6075.pdf
   *****************************************************/

  float uva(void);
  float oneMinAvgUVA(void);
  float oneHourAvgUVA(void);

  float uvb(void);
  float oneMinAvgUVB();
  float oneHourAvgUVB();

  float uvIndex(void);
  float oneMinAvgUVIndex();
  float oneHourAvgUVIndex();

  /*****************************************************
   * AS3935
   * Can detect lightning strikes up to 40km with an accuracy of 1km.
   *
   * Datasheet:
   *  https://cdn.sparkfun.com/assets/learn_tutorials/9/2/1/AS3935_Datasheet_EN_v2.pdf
   *****************************************************/

  // This funciton changes toggles the chip's settings for Indoors and Outdoors.
  void setIndoorOutdoor(uint8_t _setting);

  // This register holds the distance to the front of the storm and not the
  // distance to a lightning strike.
  byte distanceToStorm();

  // returns the average of the last 10 lightning strikes
  byte avgDistanceToStorm();

  // returns the number of lightning strikes in the last minute
  int strikeThisMinute();
  // returns the number of lightning strikes in the last hour
  int strikeThisHour();
  // returns the number of lightning strikes in the last 24 hours
  int strikeToday();

  /*****************************************************
   * RAIN
   * The rain gauge is a self-emptying bucket-type rain gauge which activates a
   * momentary button closure for each 0.011" of rain that are collected.
   *
   *****************************************************/
  // returns true if there's been rain in the last minute
  bool isRaining();
  // returns how much rain in the last minute
  float rainLastMinute();
  // returns avg rainfall over last minute
  float rainAvgLastMinute();
  // returns how much rain in the last hour
  float rainLastHour();
  // returns avg rainfall over last hour
  float rainAvgLastHour();
  // returns how much rain since 12am today
  float rainToday();

  /*****************************************************
   * ANEMOMETER (WIND SPEED)
   * The anemometer (wind speed meter) encodes the wind speed by simply closing
   * a switch which each rotation. A wind speed of 1.492 MPH produces a switch
   * closure once per second.
   *
   *****************************************************/
  float currentWindSpeedKPH();
  float oneMinAvgWindSpeedKPH();
  float oneHourAvgWindSpeedKPH();

  /*****************************************************
   * VANE (WIND DIRECTION)
   * Reports wind direction as a voltage which is produced by the combination of
   * resistors inside the sensor. The vane’s magnet could potentially close two
   * switches at once, allowing up to 16 different positions to be indicated
   *
   *****************************************************/
  windVaneDir currentWindHeading();
  char *currentWindDirStr();

private:
  bool sleeping = false;

  stationConfig _config;
  static Station *_rainSelf;
  static Station *_windSelf;

  bool bme280setup = false, veml6075setup = false, as3935setup = false,
       windSetup = false, rainSetup = false;

  void _setup();

  /* time vars  */
  long now = 0;
  // The millis counter to see when a second rolls by
  long lastSecond = 0;
  // When it hits 60, increase the current minute
  byte seconds = 0;
  // Keeps track of where we are in various arrays of data
  byte minutes = 0;
  // keeps track of the current hour, goes from 0 to 23
  byte hours = 0;

  byte loopMeasure = 0;

  // gets called at the start of every minute
  void _minuteReset();
  // gets called at the start of every hour
  void _hourReset();
  // gets called at the start of every day
  void _dayReset();

  // which sensors are enabled
  bool rain;
  bool wind;
  bool lightning;
  bool uv;
  bool atmos;

  void _createInterrupt(uint8_t irq_pin, void (*ISR_callback)(void), int value);

  /*************************
   * BME280  -- ATMOSPHERE *
   *************************/
  BME280 _bme280;
  uint8_t _bme280_address;
  uint8_t _bme280_begin;

  bool _setupBME280();
  void _loopBME280();

  void _BME280Sleep();
  void _BME280Wake();

  void _bme280Minute();
  void _bme280Hour();

  byte _tempScale;

  float tempLast = 0, humLast = 0, presLast = 0;
  float tempMinAvg = 0, humMinAvg = 0, presMinAvg = 0;
  float tempHourAvg = 0, humHourAvg = 0, presHourAvg = 0;

  float bmeAvgs[BME280_NUM_SEN][NUM_BUCKET][AVG_BUCKET_SIZE];

  /*******************
   * VEML6075 -- UV  *
   *******************/
  // VEML6075 _veml6075;
  Adafruit_VEML6075 _veml6075 = Adafruit_VEML6075();
  uint8_t _veml6075_address;
  bool _setupVEML6075();
  void _loopVEML6075();

  // VEML6075_error_t _VEML6075Sleep();
  // VEML6075_error_t _VEML6075Wake();

  void _veml6075Minute();
  void _veml6075Hour();

  float uvAvgs[VEML6075_NUM_SEN][NUM_BUCKET][AVG_BUCKET_SIZE];

  float uvaLast = 0, uvaMinAvg = 0, uvaHourAvg = 0;
  float uvbLast = 0, uvbMinAvg = 0, uvbHourAvg = 0;
  float uviLast = 0, uviMinAvg = 0, uviHourAvg = 0;

  /***********************
   * AS3935 -- LIGHTNING *
   ***********************/
  SparkFun_AS3935 _as3935;
  uint8_t _as3935_address;
  int _as3935_cs;

  bool _setupAS3935();
  void _loopAS3935();

  bool _AS3935Wake();
  void _AS3935Sleep();

  void _strikeMinute();
  void _strikeHour();
  void _strikeDay();

  int _noise = 2;     // Value between 1-7
  int _disturber = 2; // Value between 1-10
  byte _distance = 0;
  byte _distances[AS3935_AVG_SIZE];
  int _distIdx = 0;

  int _strikesMinute[AVG_BUCKET_SIZE];
  int _strikesHour[AVG_BUCKET_SIZE];
  int _strikesDay[24];

  /********
   * RAIN *
   ********/
  bool _setupRain();
  static void _handleRainIRQ();
  void _rainIRQ();

  void _loopRain();

  void _rainMinute();
  void _rainTenMinute();
  void _rainHour();
  void _rainDay();

  float rainfallMinuteAvg[ONE_MIN_AVG_SIZE];
  float rainfallHourAvg[ONE_MIN_AVG_SIZE];

  const int mmPerClick = 2.794;

  float rainMinuteAvg = 0;
  float rainHourAvg = 0;

  // rain so far this minute in mm
  volatile float rainM = 0;
  // rain so far this hour in mm
  volatile float rainH = 0;
  // rain so far today in mm
  volatile float rainD = 0;

  // modified in the rain IRQ function
  volatile unsigned long rainTime = 0, rainLast = 0, rainInterval = 0;

  /********
   * WIND *
   ********/
  bool _setupWind();
  static void _handleWindIRQ();
  void _windSpeedIRQ();

  void _loopWind();
  windVaneDir _voltToDir(float);
  void _readVane();

  void _windMinute();
  void _windTenMinute();
  void _windHour();
  void _windDay();

  // 60 bytes to keep track of the one minute average
  windVaneDir windDirAvg[ONE_MIN_AVG_SIZE];

  // calculated value
  windVaneDir avgWindDirOneMin = UNKNOWN;

  // where is the vane pointing rite nao
  windVaneDir currentWindDir = UNKNOWN;

  // instantaneous wind speed in kilometers per hour
  float windSpeedKPH = 0;

  float windSpeedKPH_minAvg = 0;
  float windSpeedKPH_hourAvg = 0;

  float ws_minAvg[ONE_MIN_AVG_SIZE];
  float ws_hourAvg[ONE_MIN_AVG_SIZE];

  // private wind stuff
  /// direction
  // voltage of the vane, used to determinte direction
  float windVoltage = 0;
  // the direction, converted from volts
  float rawWindDir = 0;

  /// speed
  long lastWindCheck = 0;
  volatile long lastWindIRQ = 0;
  volatile byte windClicks = 0;
};

#endif
