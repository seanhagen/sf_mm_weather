#ifndef WIND_H
#define WIND_H

#include "driver/adc.h"
#include "esp_adc_cal.h"
#include <stdint.h>

#define CLICKS_TO_KPH 2.4011412
#define WIND_DIR_AVG_SIZE 60

enum windVaneDir {
  UNKNOWN = -1,
  NORTH = 360,           // 0/360 DEG
  NORTH_NORTHEAST = 22,  // 22 DEG
  NORTHEAST = 45,        // 45 DEG
  EAST_NORTHEAST = 67,   // 67 DEG
  EAST = 90,             // 90 DEG
  EAST_SOUTHEAST = 112,  // 112 DEG
  SOUTHEAST = 135,       // 135 DEG
  SOUTH_SOUTHEAST = 157, // 157 DEG
  SOUTH = 180,           // 180
  SOUTH_SOUTHWEST = 202, // 202 DEG
  SOUTHWEST = 225,       // 225 DEG
  WEST_SOUTHWEST = 248,  // 248 DEG
  WEST = 270,            // 270 DEG
  WEST_NORTHWEST = 292,  // 292 DEG
  NORTHWEST = 315,       // 315 DEG
  NORTH_NORTHWEST = 338, // 338 DEG
};

#endif
