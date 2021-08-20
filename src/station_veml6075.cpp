#include <station.h>

bool Station::_setupVEML6075() {
  if (uv) {

    if (_veml6075.begin(Wire) != VEML6075_SUCCESS) {
      // panic or return an error or SOMEHING
      return false;
    }
  }
  return true;
}

void Station::_loopVEML6075() {
  // maybe cache the values from uv.a(), uv.b(), and uv.i()?
  if (uv) {
    uvaLast = _veml6075.a();
    uvbLast = _veml6075.b();
    uviLast = _veml6075.i();

    uvaOneMinAvg_cache[seconds] = uvaLast;
    uvbOneMinAvg_cache[seconds] = uvbLast;
    uviOneMinAvg_cache[seconds] = uviLast;
  }
}

void Station::_uvMinute() {
  if (uv) {
    uvaOneMinAvg = 0.0;
    uvbOneMinAvg = 0.0;
    uviOneMinAvg = 0.0;
    float af = 0.0, bf = 0.0, indf = 0.0;
    int ai = 0, bi = 0, ii = 0;
    for (int i = 0; i < ONE_MIN_AVG_SIZE; i++) {
      if (uvaOneMinAvg_cache[i] != 0) {
        af++;
        ai++;
        uvaOneMinAvg += uvaOneMinAvg_cache[i];
      }

      if (uvbOneMinAvg_cache[i] != 0) {
        bf++;
        bi++;
        uvbOneMinAvg += uvbOneMinAvg_cache[i];
      }

      if (uviOneMinAvg_cache[i] != 0) {
        indf++;
        ii++;
        uviOneMinAvg += uviOneMinAvg_cache[i];
      }
    }

    if (ai > 0) {
      uvaOneMinAvg /= af;
    }
    if (bi > 0) {
      uvbOneMinAvg /= bf;
    }
    if (ii > 0) {
      uviOneMinAvg /= indf;
    }

    uvaTenMinAvg_cache[minutes_10m] = uvaOneMinAvg;
    uvbTenMinAvg_cache[minutes_10m] = uvbOneMinAvg;
    uviTenMinAvg_cache[minutes_10m] = uviOneMinAvg;
  }
}

void Station::_uvTenMinute() {
  if (uv) {
    uvaTenMinAvg = 0;
    uvbTenMinAvg = 0;
    uviTenMinAvg = 0;

    float af = 0.0, bf = 0.0, indf = 0.0;
    int ai = 0, bi = 0, ii = 0;
    for (int i = 0; i < TEN_MIN_AVG_SIZE; i++) {
      if (uvaTenMinAvg_cache[i] != 0) {
        af++;
        ai++;
        uvaTenMinAvg += uvaTenMinAvg_cache[i];
      }

      if (uvbTenMinAvg_cache[i] != 0) {
        bf++;
        bi++;
        uvbTenMinAvg += uvbTenMinAvg_cache[i];
      }

      if (uviTenMinAvg_cache[i] != 0) {
        indf++;
        ii++;
        uviTenMinAvg += uviTenMinAvg_cache[i];
      }
    }

    if (ai > 0) {
      uvaTenMinAvg /= af;
    }
    if (bi > 0) {
      uvbTenMinAvg /= bf;
    }
    if (ii > 0) {
      uviTenMinAvg /= indf;
    }
  }
}
