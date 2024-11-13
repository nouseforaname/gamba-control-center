#include "pins.h"
#include <SPI.h>
#include <Arduino.h>
#include "internal/input_data.h"
#include <TFT_eSPI.h>
#include <XPT2046_Touchscreen.h>
#include <stdio.h>
#include "input.h"

XPT2046_Touchscreen ts(TOUCHSCREEN_CS_PIN);
void init_input(){
  SPI.begin(TOUCHSCREEN_SCLK_PIN, TOUCHSCREEN_MISO_PIN, TOUCHSCREEN_MOSI_PIN);
  ts.begin();
  ts.setRotation(1);
}

void writeScreenPosition(position *pos){
  //magic numbers taken by reading the output from the function below.
  float offset_x=300;
  float offset_y=350;
  float high_x=3500;
  float high_y=3800;
  float range_x=high_x-offset_x;
  float range_y=high_y-offset_y;
  float y_pos_as_percent_of_screen;
  float x_pos_as_percent_of_screen;

  TS_Point p = ts.getPoint();

  x_pos_as_percent_of_screen =1- ((float)p.x - offset_x) / range_x;
  y_pos_as_percent_of_screen = ((float)p.y - offset_y) / range_y;

  if (y_pos_as_percent_of_screen < 0) {
    y_pos_as_percent_of_screen = 0.05;
  }
  if (y_pos_as_percent_of_screen > 1) {
    y_pos_as_percent_of_screen = 0.95;
  } 
  if ( x_pos_as_percent_of_screen < 0) {
     x_pos_as_percent_of_screen = 0.05;
  }
  if (x_pos_as_percent_of_screen > 1) {
    x_pos_as_percent_of_screen = 0.95;
  } 

  // note that the dimensions are w: 240, h:320
  pos->x=(int16_t)((float)TFT_HEIGHT * x_pos_as_percent_of_screen);
  pos->y=(int16_t)((float)TFT_WIDTH * y_pos_as_percent_of_screen),
  pos->z=p.z;
}

#define MAX_MEASUREMENTS 25
void handleAndDrawInput(TFT_eSPI tft){
  position inputPositions[MAX_MEASUREMENTS];
  bool init = true; 
  bool clean = false;
  while(ts.touched()){
    if (init){
      for (int8_t i=0; i<MAX_MEASUREMENTS; i++){
        writeScreenPosition(&inputPositions[i]);
      }
     init = false;
    }
    tft.fillCircle(inputPositions[MAX_MEASUREMENTS-1].x,inputPositions[MAX_MEASUREMENTS-1].y, 6, TFT_BLACK);
    for (int8_t i=MAX_MEASUREMENTS-2; i>= 0; i--){
      tft.fillCircle(inputPositions[i].x,inputPositions[i].y, 6, TFT_GREEN+i*1);
      clean = true;
      inputPositions[i+1] = inputPositions[i]; 
    }
    writeScreenPosition(&inputPositions[0]);
  }
  if (clean){
    for (int8_t i=MAX_MEASUREMENTS-1; i>=0; i--){
      tft.fillCircle(inputPositions[i].x,inputPositions[i].y, 6, TFT_BLACK);
    }
  }
}
