#include <stdint.h>
#include <stdio.h>
#include <driver/adc.h>

void water_init(){

  adc2_config_channel_atten( ADC2_CHANNEL_6, ADC_ATTEN_DB_0 );
//pinMode(PIN_DAC1, INPUT); //It is necessary to declare the input pin
//analogSetPinAttenuation(PIN_DAC1,ADC_0db);
}
uint8_t getWaterLevel() {
  // range is 0-4095
  int v = 0;

  adc2_get_raw(ADC2_CHANNEL_6,ADC_WIDTH_BIT_12, &v);
  float factor = (float)v/4095;
  uint8_t percent = 100 * factor; // 10% steps are enough...
  if (percent > 100) {
    return 0;
  }
  return percent;
}
