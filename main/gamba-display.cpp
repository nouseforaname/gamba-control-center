#include "pins.h"
#include "TFT_eSPI.h"/* Please use the TFT library provided in the library. */
#include "logo.h"
#include "stdio.h"
#include "Arduino.h"
//#include "hal/gpio_ll.h"

#include "esp_adc_cal.h"

#define WAIT 100

unsigned long targetTime = 0; // Used for testing draw times
TFT_eSPI tft = TFT_eSPI(320,240);

extern "C" void app_main() {
  // Set up the application
  setup();
  for (;;) {
    printf("executing loop\n");
    
    loop();
    yield();
    // Loop forever
  }
}



void setup()
{
  printf("begin setup\n");
  pinMode(PWR_EN_PIN, OUTPUT);
  pinMode(38, OUTPUT);  // Code Crashed without setting this. not totally sure why..
  digitalWrite(PWR_EN_PIN, HIGH);

  tft.init();
  tft.setRotation(0);
  tft.fillScreen(TFT_BLACK);
  // Set backlight level, range 0 ~ 16
  uint8_t brightness = 16; 
  setBrightness(brightness);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  
  //setup waterlevel sensor. it's a voltage divider & resistor setup 
  pinMode(PIN_DAC1, INPUT); //It is necessary to declare the input pin
  analogSetPinAttenuation(PIN_DAC1,ADC_0db);
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
  sprintf(s, "Wasser: %i", getWaterLevel());
  tft.setTextPadding(tft.textWidth("Wasser: 000",4));                // extra whitespace required to override last digit of three digit value, 
  tft.drawString( s ,0,50, 4);
  delay(WAIT);

}
