#include "main/settings.h"
#include "rom/gpio.h"
#include <stdint.h>
#include <TFT_eSPI.h>

TFT_eSPI tft = TFT_eSPI();
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
TFT_eSPI *getTFT(){
   return &tft;
} 

void init_display() {
  // BACKLIGHT PIN
  gpio_set_direction(GPIO_NUM_10, GPIO_MODE_OUTPUT);
  gpio_pad_select_gpio(GPIO_NUM_38);
  gpio_set_direction(GPIO_NUM_38, GPIO_MODE_OUTPUT);
  digitalWrite(BK_LIGHT_PIN, 1);

  tft.init();
  tft.setRotation(0);
  tft.fillScreen(TFT_BLACK);
  // Set backlight level, range 0 ~ 16
  uint8_t brightness = 16;
  setBrightness(brightness);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
}
