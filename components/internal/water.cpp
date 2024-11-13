#include <stdint.h>
#include <stdio.h>
#include <esp_adc/adc_oneshot.h>
#include <esp_log.h>

adc_oneshot_unit_handle_t adc2_handle;
adc_oneshot_unit_init_cfg_t init_config = {
    .unit_id = ADC_UNIT_2,
    .ulp_mode = ADC_ULP_MODE_DISABLE,
};
adc_oneshot_chan_cfg_t config = {
    .atten = ADC_ATTEN_DB_0,
    .bitwidth = ADC_BITWIDTH_DEFAULT,
};
void water_init(){
  ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config, &adc2_handle));
}
char water_tag[] = "water";
uint8_t getWaterLevel() {
  // range is 0-4095
  int v = 0;
  ESP_ERROR_CHECK(adc_oneshot_config_channel(adc2_handle, ADC_CHANNEL_6, &config));
  ESP_ERROR_CHECK(adc_oneshot_read(adc2_handle, ADC_CHANNEL_6, &v));

  float factor = (float)v/4095;
  uint8_t percent = 100 * factor; // 10% steps are enough...
  
  ESP_LOGD(water_tag, "calculated %i", percent);
  if (percent > 100) {
    return 0;
  }
  return percent;
}
