#include "Arduino.h"
#include <stdint.h>
#include <stdio.h>


uint8_t getWaterLevel() {
  uint32_t v = analogReadMilliVolts(PIN_DAC1);
   
  printf("Voltage %lu\n", v);
  uint8_t percent = v / 100 * 10; // 10% steps are enough...
  printf("%d\n",percent);  
  return percent;
}
