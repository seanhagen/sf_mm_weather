#include <station.h>

void Station::_createInterrupt(uint8_t irq_pin, void (*ISR_callback)(void),
                               int value) {
  attachInterrupt(digitalPinToInterrupt(irq_pin), ISR_callback, value);
}
