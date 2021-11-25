#include <station.h>

bool Station::_setupVEML6075() {
  if (uv) {
    if (_veml6075.begin() != true) {
      // panic or return an error or SOMEHING
      return false;
    }
  }
  return true;
}

void Station::_loopVEML6075() {
  // maybe cache the values from uv.a(), uv.b(), and uv.i()?
  if (uv) {
    _veml6075.readUVABI(&uvaLast, &uvbLast, &uviLast);

    uvAvgs[VEML6075_UVA][BUCKET_MINUTES][seconds] = uvaLast;
    uvAvgs[VEML6075_UVB][BUCKET_MINUTES][seconds] = uvbLast;
    uvAvgs[VEML6075_UVI][BUCKET_MINUTES][seconds] = uviLast;
  }
}

// VEML6075_error_t Station::_VEML6075Wake() {
//   if (uv) {
//     return _veml6075.powerOn();
//   }
//   return VEML6075_ERROR_SUCCESS;
// }

// VEML6075_error_t Station::_VEML6075Sleep() {
//   if (uv) {
//     return _veml6075.shutdown();
//   }
//   return VEML6075_ERROR_SUCCESS;
// }

void Station::_veml6075Minute() {
  if (uv) {
    float uva = 0, uvb = 0, uvi = 0;

    for (int i = 0; i < AVG_BUCKET_SIZE; i++) {
      uva += uvAvgs[VEML6075_UVA][BUCKET_MINUTES][seconds];
      uvb += uvAvgs[VEML6075_UVB][BUCKET_MINUTES][seconds];
      uvi += uvAvgs[VEML6075_UVI][BUCKET_MINUTES][seconds];
    }

    uvaMinAvg = uva / ONE_MIN_AVG_SIZE_F;
    uvbMinAvg = uvb / ONE_MIN_AVG_SIZE_F;
    uviMinAvg = uvi / ONE_MIN_AVG_SIZE_F;

    uvAvgs[VEML6075_UVA][BUCKET_HOURS][minutes] = uvaMinAvg;
    uvAvgs[VEML6075_UVB][BUCKET_HOURS][minutes] = uvbMinAvg;
    uvAvgs[VEML6075_UVI][BUCKET_HOURS][minutes] = uviMinAvg;
  }
}

void Station::_veml6075Hour() {
  if (uv) {
    float uva = 0, uvb = 0, uvi = 0;

    for (int i = 0; i < AVG_BUCKET_SIZE; i++) {
      uva += uvAvgs[VEML6075_UVA][BUCKET_MINUTES][seconds];
      uvb += uvAvgs[VEML6075_UVB][BUCKET_MINUTES][seconds];
      uvi += uvAvgs[VEML6075_UVI][BUCKET_MINUTES][seconds];
    }

    uvaHourAvg = uva / ONE_MIN_AVG_SIZE_F;
    uvbHourAvg = uvb / ONE_MIN_AVG_SIZE_F;
    uviHourAvg = uvi / ONE_MIN_AVG_SIZE_F;
  }
}

float Station::uva() { return uvaLast; }
float Station::oneMinAvgUVA() { return uvaMinAvg; }
float Station::oneHourAvgUVA() { return uvaHourAvg; }

float Station::uvb() { return uvbLast; }
float Station::oneMinAvgUVB() { return uvbMinAvg; }
float Station::oneHourAvgUVB() { return uvbHourAvg; }

float Station::uvIndex() { return uviLast; }
float Station::oneMinAvgUVIndex() { return uviMinAvg; }
float Station::oneHourAvgUVIndex() { return uviHourAvg; }
