#include <station.h>

void Station::_setupVEML6075() {
  if (uv) {
    if (uv.begin(Wire) != VEML6075_SUCCESS) {
      // panic or return an error or SOMEHING
      return false;
    }
  }
  return true;
}

void Station::_loopVEML6075() {
  // maybe cache the values from uv.a(), uv.b(), and uv.i()?
}
