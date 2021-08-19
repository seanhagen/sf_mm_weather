#include <station.h>

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

void Station::_loopAS3935() {
  if (digitalRead(LIGHTNING_INTERRUPT_PIN) == HIGH) {
    intVal = lightning.readInterruptReg();
    Serial.print("Lightning: ");
    Serial.println(intVal);
    if (intVal == NOISE_INT) {
      // Too much noise? Uncomment the code below, a higher number means better
      // noise rejection.
      lightning.setNoiseLevel(noise);
      return lightningInfo{false, true, false, 0};
    } else if (intVal == DISTURBER_INT) {
      // Too many disturbers? Uncomment the code below, a higher number means
      // better disturber rejection.
      lightning.watchdogThreshold(disturber);
      return lightningInfo{false, false, true, 0};
    } else if (intVal == LIGHTNING_INT) {
      // Lightning! Now how far away is it? Distance estimation takes into
      // account any previously seen events in the last 15 seconds.
      byte distance = lightning.distanceToStorm();
      return lightningInfo{true, false, false, distance};
    }
  }
}