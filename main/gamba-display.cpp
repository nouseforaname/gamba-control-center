#include "pins.h"
#include "TFT_eSPI.h"
#include "stdio.h"
#include "Arduino.h"
#include <water.h>
#include <display.h>
#include <input.h>
#define WAIT 50

unsigned long targetTime = 0; // Used for testing draw times
TFT_eSPI tft = TFT_eSPI(320,240);

extern "C" void app_main() {
  // Set up the application
  setup();
  for (;;) {
    loop();
    yield();
  }
}

void setup()
{
  printf("begin setup\n");
  pinMode(PWR_EN_PIN, OUTPUT);
  pinMode(38, OUTPUT);  // Code Crashed without setting this. not totally sure why..
  digitalWrite(PWR_EN_PIN, HIGH);

  water_init();
  init_input();
  tft.init();
  tft.setRotation(0);
  tft.fillScreen(TFT_BLACK);
  // Set backlight level, range 0 ~ 16
  uint8_t brightness = 16; 
  setBrightness(brightness);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
}

bool flip = true;
uint8_t rotation = 0;
void loop()
{
  tft.resetViewport();
  tft.setRotation(rotation);
  targetTime = millis();
  tft.setTextSize(1);

  char s[14];
  sprintf(s, "Wasser: %03d", getWaterLevel());
  tft.drawString( s ,0,50, 4);


  handleAndDrawInput(tft);

  delay(WAIT);
 
}
