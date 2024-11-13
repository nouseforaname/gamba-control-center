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

void sensorLoop(void* pvParameters){
  for (;;) {
    sprintf(s, "Wasser: %03d", getWaterLevel());
    vTaskDelay(100); // so other crap can get to RUN (IDLE TASK) and it can reset the watchdog 
  }
}
void renderLoop(void* pvParameters){
  for (;;) {
    getTFT()->drawString(s,0,0,4);
    handleAndDrawInput();
    vTaskDelay(100); // so other crap can get to RUN (IDLE TASK) and it can reset the watchdog 
  }
}
void setup()
{
  init_display();
  water_init();
  init_input();

  ESP_LOGI(main_tag,"app setup done");
  xTaskCreate(
    sensorLoop,     // Function to implement the task
    "sensors",   // Name of the task
    4096,      // Stack size in bytes
    NULL,      // Task input parameter
    1,         // Priority of the task
    NULL      // Task handle.
  );
  xTaskCreate(
    renderLoop,     // Function to implement the task
    "render",   // Name of the task
    8192,      // Stack size in bytes
    NULL,      // Task input parameter
    1,         // Priority of the task
    NULL      // Task handle.
  );
}

extern "C" void app_main() {
  // Set up the application
  setup();
}
