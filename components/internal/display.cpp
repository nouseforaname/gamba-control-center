#include "main/settings.h"
#include "rom/gpio.h"
#include <stdint.h>
#include <TFT_eSPI.h>

TFT_eSPI tft = TFT_eSPI();
void setBrightness(uint8_t value)
{
  if (value == 0){
    return;
  }
  digitalWrite(BK_LIGHT_PIN, 1);
  uint8_t steps = 16 - value;
  for (int i = 0; i < steps; i++) {
      digitalWrite(BK_LIGHT_PIN, 0);
      digitalWrite(BK_LIGHT_PIN, 1);
  }
}
TFT_eSPI *getTFT(){
   return &tft;
} 

void init_display() {
  // BACKLIGHT PIN
  pinMode(PWR_EN_PIN, OUTPUT);
  pinMode(BK_LIGHT_PIN, OUTPUT);  // Code Crashed without setting this. not totally sure why..
  digitalWrite(PWR_EN_PIN, HIGH);

  tft.init();
  setBrightness(16);
  tft.fillScreen(BACKGROUND_COLOR);
  tft.setTextColor(FONT_COLOR,BACKGROUND_COLOR);
}
