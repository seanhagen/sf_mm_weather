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

#define TWO_MIN_AVG_SIZE 120
#define TWO_MIN_AVG_SIZE_F 120.0

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
#define AS3935_INT_PIN
#endif

#ifndef AS3935_CS_PIN
// The chip-select pin used for the AS3935 lightning detector
// G1 - GPIO pin 25, pad 42 on the MicroMod Weather Carrier Board
#define AS3935_CS_PIN 35
#endif

#define WIND_DIR_AVG_SIZE 120

#include <stdint.h>

#include <Arduino.h>
#include <Wire.h>

#include <SparkFunBME280.h>
#include <SparkFun_AS3935.h>
#include <SparkFun_VEML6075_Arduino_Library.h>

#include "wind.h"

struct config {
public:
  // will report rainfall readings from attached rain bucket
  bool enableRain;
  // will report wind direction & speed from the attached anemometer and wind
  // vane
  bool enableWind;
  // will report readings from the AS3935 lightning detection sensor if true
  bool enableLightning;
  // will report readings from the VEML6075 UV sensor if true
  bool enableUV;
  // will report readings from the BME280 temperature, humidity, and air
  // pressure sensor if true
  bool enableAtmosphere;

  // the i2c address for the as3935
  uint8_t as3935_i2c = 0x03;

  // the i2c address for the bme280
  uint8_t bme280_i2c = 0x77;

  // the i2c address for the veml6075
  uint8_t veml6075_i2c = 0x10;
};

struct station_error {
public:
  VEML6075_error_t veml6075_error;
  uint8_t bme280_error;
  bool as3935_had_error;
};

struct isMeasuring {
public:
  bool bme280_temp;
  bool bme280_pres;
  bool bme280_hum;
  bool veml6075;
  bool as3935;
};

struct immediateMeasurements {
public:
  float temperature;
  float humidity;
  float pressure;
  float uva;
  float uvb;
  float uvIndex;
};

struct countedMeasurements {
public:
  float rainLastMinute;
  float rainLastHour;
  float rainToday;

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
  Station(config conf);

  // initializes all enabled sensors
  station_error begin(void);

  // returns which sensors are currently active
  isMeasuring isMeasuring(void);

  // MUST BE CALLED WITHIN LOOP OF YOUR CODE!
  // NONE OF THIS WILL WORK OTHERWISE ( at least, not properly )
  void loop();

  // Reads all measurements from non-interrupt based sensors. A value of 0 for
  // temperatureScale means Celsius, any other value means Fahrenheit.
  void readAll(allMeasurements *measurements, byte temperatureScale = 0);

  // turns on all the sensors
  station_error powerOn();
  // shuts off all the sensors
  station_error shutdown();

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
  float tempC();
  float tempF();

  /* BME280 AIR PRESSURE */
  float airPressure();

  /* BME280 HUMIDITY */
  float humidity();
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
  float uvb(void);
  float index(void);

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
  uint8_t distanceToStorm();

  /*****************************************************
   * RAIN
   * The rain gauge is a self-emptying bucket-type rain gauge which activates a
   * momentary button closure for each 0.011" of rain that are collected.
   *
   *****************************************************/

  bool isRaining();
  float rainLastMinute();
  float rainLastHour();
  float rainToday();

  /*****************************************************
   * ANEMOMETER (WIND SPEED)
   * The anemometer (wind speed meter) encodes the wind speed by simply closing
   * a switch which each rotation. A wind speed of 1.492 MPH produces a switch
   * closure once per second.
   *
   *****************************************************/

  float currentWindSpeedKPH();
  float windSpeedAvg();

  /*****************************************************
   * VANE (WIND DIRECTION)
   * Reports wind direction as a voltage which is produced by the combination of
   * resistors inside the sensor. The vane’s magnet could potentially close two
   * switches at once, allowing up to 16 different positions to be indicated
   *
   *****************************************************/

  windVaneDir currentWindHeading();

private:
  void _setup(config c);
  void _setupSensors();

  // the sensors
  BME280 _bme280;
  uint8_t _bme280_address;
  bool _setupBME280();
  void _loopBME280();

  VEML6075 _veml6075;
  uint8_t _veml6075_address;
  bool _setupVEML6075();
  void _loopVEML6075();

  SparkFun_AS3935 _as3935;
  uint8_t _as3935_address;
  bool _setupAS3935();
  void _loopAS3935();

  bool _setupWind();
  void _loopWind();
  void _windSpeedIRQ();

  bool _setupRain();
  void _loopRain();
  void _rainIRQ();

  /* time vars  */
  // The millis counter to see when a second rolls by
  long lastSecond = 0;
  // When it hits 60, increase the current minute
  byte seconds = 0;
  // Keeps track of the "wind speed/dir avg" over last
  // 2 minutes array of data
  byte seconds_2m = 0;
  // Keeps track of where we are in various arrays of data
  byte minutes = 0;
  // Keeps track of where we are in wind gust/dir over last
  // 10 minutes array of data
  byte minutes_10m = 0;

  byte hour = 0;

  // which sensors are enabled
  bool rain;
  bool wind;
  bool lightning;
  bool uv;
  bool atmos;

  /* rain */
  // // rain over the last hour
  // volatile float rainHour[60];

  // rain so far this minute in mm
  volatile float rainM = 0;

  // rain so far this hour in mm
  volatile float rainH = 0;

  // rain so far today in mm
  volatile float rainD = 0;

  // last 10 minutes of rain
  float rain_10m[10];

  // 120 bytes to keep track of 2 minute average rainfall
  byte rainFallAvg[TWO_MIN_AVG_SIZE];

  // 2 minute average of rainfall in mm
  float rainAvg = 0;

  // modified in the rain IRQ function
  volatile unsigned long rainTime = 0, rainLast = 0, rainInterval = 0;

  /* wind direction */
  // voltage of the vane, used to determinte direction
  float windVoltage = 0;
  // the direction, converted from volts
  float rawWindDir = 0;

  windVaneDir currentWindDir = UNKNOWN;

  windVaneDir windDirAvg[TWO_MIN_AVG_SIZE];

  windVaneDir avgWindDir2m = UNKNOWN;

  /* wind speed */
  long lastWindCheck = 0;
  volatile long lastWindIRQ = 0;
  volatile byte windClicks = 0;

  // 120 bytes to keep track of 2 minute average
  byte windSpdAvg[TWO_MIN_AVG_SIZE];

  // 120 ints to keep track of 2 minute average
  windVaneDir windDiraAg[TWO_MIN_AVG_SIZE];

  // 10 floats to keep track of 10 minute max
  float windGust_10m[10];

  // instantaneous wind speed in kilometers per hour
  float windSpeedKPH = 0;

  // 2 minute average of wind speed in kilometers per hour
  float windSpdAvgKPH = 0;
};

#endif
