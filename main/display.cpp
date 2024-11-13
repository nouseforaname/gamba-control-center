#include "pins.h"
#include <stdint.h>
#include <Arduino.h>

void setBrightness(uint8_t value)
{
  static uint8_t steps = 16;
  static uint8_t _brightness = 0;

  if (_brightness == value) {
      return;
  }

  if (value > 16) {
      value = 16;
  }
  if (value == 0) {
      digitalWrite(BK_LIGHT_PIN, 0);
      _brightness = 0;
      return;
  }
  if (_brightness == 0) {
      digitalWrite(BK_LIGHT_PIN, 1);
      _brightness = steps;
      delayMicroseconds(30);
  }
  int from = steps - _brightness;
  int to = steps - value;
  int num = (steps + to - from) % steps;
  for (int i = 0; i < num; i++) {
      digitalWrite(BK_LIGHT_PIN, 0);
      digitalWrite(BK_LIGHT_PIN, 1);
  }
}
