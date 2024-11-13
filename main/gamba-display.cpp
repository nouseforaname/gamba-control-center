#include <settings.h>
#include "TFT_eSPI.h"
#include "stdio.h"
#include "Arduino.h"
#include <lib/water.h>
#include <lib/input.h>
#include <lib/display.h>
#include <esp_log.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

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
