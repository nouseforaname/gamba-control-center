#include <settings.h>
#include "components/internal/input.h"
#include "components/internal/water.h"
#include "components/internal/display.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <esp_log.h>

char main_tag[] = "main";
char sensor_tag[] = "sensor";
static char s[14]="";

void readSensors(void* pvParameters){
  for (;;) {

    sprintf(s, "Wasser: %03d", getWaterLevel());
    delay(WAIT);

  }
}

void setup()
{
  water_init();
  init_display();
  init_input();
  ESP_LOGI(log_tag,"app setup done");
  xTaskCreate(
    readSensors,     // Function to implement the task
    "loop2",   // Name of the task
    1000,      // Stack size in bytes
    NULL,      // Task input parameter
    0,         // Priority of the task
    NULL      // Task handle.
  );
}


void loop()
{
  handleAndDrawInput(tft);

  delay(WAIT);

}
