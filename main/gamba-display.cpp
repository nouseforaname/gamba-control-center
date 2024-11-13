#include <settings.h>
#include "TFT_eSPI.h"
#include "stdio.h"
#include "Arduino.h"
#include <lib/water.h>
#include <lib/input.h>
#include <lib/display.h>
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

char log_tag[] = "main";
static char s[14]="";

void readSensors(void* pvParameters){
  for (;;) {

    sprintf(s, "Wasser: %03d", getWaterLevel());
    delay(WAIT);

  }
}

void setup()
{
  printf("begin setup\n");
  pinMode(PWR_EN_PIN, OUTPUT);
  pinMode(38, OUTPUT);  // Code Crashed without setting this. not totally sure why..
  digitalWrite(PWR_EN_PIN, HIGH);

  water_init();
  init_display();
  init_input();
}


void loop()
{
  handleAndDrawInput(tft);

  delay(WAIT);

}
