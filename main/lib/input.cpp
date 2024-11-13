#include "settings.h"
#include <SPI.h>
#include <Arduino.h>
#include <TFT_eSPI.h>
#include <XPT2046_Touchscreen.h>
#include <stdio.h>
#include "input.h"
#include "esp_log.h"
#include "display.h"

XPT2046_Touchscreen ts(TOUCHSCREEN_CS_PIN);
static TFT_eSprite * sprites[MAX_MEASUREMENTS];
static int8_t currentRingIndex = MAX_MEASUREMENTS -1;
char tag[] = "test";
position *inputPositions;
r_buffer inputBuf;
int8_t getRingBufferLastForCurrentIndex(){
   return currentRingIndex == 0 ? MAX_MEASUREMENTS -1 : currentRingIndex- 1;
}
void increaseRingBufferIndex(){
  currentRingIndex = 
    currentRingIndex == MAX_MEASUREMENTS - 1 ?
      0 :
      currentRingIndex + 1;
}
void decreaseRingBufferIndex(){
  currentRingIndex = 
    currentRingIndex == 0 ? 
      MAX_MEASUREMENTS - 1:
      currentRingIndex - 1;
}

// REPO ISSUE EXAMPLE
//while(1)
//  {
//    yield();
//    uint16_t* screenContent = NULL;
//    screenContent = ( uint16_t*) calloc(100 * 100, sizeof(uint16_t));
//
//    tft.readRect( 10, 80, 100, 100, screenContent);
//    
//    tft.setTextColor(TFT_WHITE, TFT_BLACK);
//    tft.drawNumber(count++,10,80,4);
//    delay(20);
//    
//    tft.pushRect( 10, 80, 100, 100, screenContent);
//
//    // Copy to a different place
//    //tft.fillRect(130, 80, 100, 100, TFT_BLACK);
//    //delay(50);
//    //tft.pushImage(130, 80, 100, 100, screenContent);
//
//    free(screenContent);
//    //delay(50);
//  }
//}
void init_input(){
  static position *inputList = (position*)calloc( 10,3*sizeof(uint16_t));
  inputBuf = r_buffer{0,MAX_MEASUREMENTS, inputList};
  SPI.begin(TOUCHSCREEN_SCLK_PIN, TOUCHSCREEN_MISO_PIN, TOUCHSCREEN_MOSI_PIN);
  SPI.setFrequency(SPI_FREQUENCY);
  ts.begin();
  ts.setRotation(1);
  TFT_eSPI *t = getTFT();
  int8_t count = 0;
  while ( count < MAX_MEASUREMENTS ){
    static TFT_eSprite s = TFT_eSprite(t);
    s.setColorDepth(4);
    s.createSprite(12,12);
    s.createPalette(default_4bit_palette);
    s.fillSprite(BACKGROUND_COLOR);
    sprites[currentRingIndex] = &s;
    increaseRingBufferIndex();
    ESP_LOGI(tag,"init sprite: %i",currentRingIndex);
    count++;
  }
  increaseRingBufferIndex();
}

void writeScreenPosition(position *target){
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
  target->x=(int16_t)((float)TFT_HEIGHT * x_pos_as_percent_of_screen);
  target->y=(int16_t)((float)TFT_WIDTH * y_pos_as_percent_of_screen),
  target->z=p.z;
}
void readInputInto(position *input){

}
void handleAndDrawInput(){


  bool needInputclean = false;
  uint16_t *screenContent=(uint16_t *)calloc(MAX_MEASUREMENTS, 144*sizeof(uint16_t));
  TFT_eSPI t = *getTFT();

  while(ts.touched()){
    yield();
    delay(10);
    int8_t count = 0;
    ESP_LOGI(tag,"start loop ----------");
    if ( !needInputclean ) {
      while ( count < MAX_MEASUREMENTS ){
        writeScreenPosition(&inputPositions[currentRingIndex]);
        increaseRingBufferIndex();
        count++; 
      }
      needInputclean = true;
      ESP_LOGI(tag,"clean is required: %s", needInputclean ? "true": "false");
    }

    decreaseRingBufferIndex();
    if (screenContent[currentRingIndex] == 0 ){
      ESP_LOGI(tag,"buffering %i", currentRingIndex);
      t.readRect( inputPositions[currentRingIndex].x,inputPositions[currentRingIndex].y, 12, 12, &screenContent[currentRingIndex]); 
      ESP_LOGI(tag,"buffer size is: %i", sizeof(&screenContent[currentRingIndex]));
    }else {
      ESP_LOGI(tag,"filling %i from buffer", currentRingIndex);
      sprites[currentRingIndex]->pushRect(inputPositions[currentRingIndex].x,inputPositions[currentRingIndex].y,0,0,&screenContent[currentRingIndex]);
      increaseRingBufferIndex();
    }
    writeScreenPosition(&inputPositions[currentRingIndex]);
    count = 0;
    while( count < MAX_MEASUREMENTS ) {
      sprites[currentRingIndex]->fillCircle(6,6,6, FONT_COLOR);
      ESP_LOGI(tag,"drawing %i from input. size is: %i", currentRingIndex,sizeof(&sprites[currentRingIndex]));
      
      sprites[currentRingIndex]-> pushSprite(inputPositions[currentRingIndex].x,inputPositions[currentRingIndex].y);
      count++;
      increaseRingBufferIndex();
    }    
  }
  if ( needInputclean ){
    int8_t count = 0;
    decreaseRingBufferIndex();
    while( count < MAX_MEASUREMENTS ) {
      ESP_LOGI(tag,"cleaning %i", currentRingIndex);
      sprites[currentRingIndex]->pushRect(inputPositions[currentRingIndex].x,inputPositions[currentRingIndex].y,12,12,&screenContent[currentRingIndex]);
      decreaseRingBufferIndex();
      count++;
    }
    needInputclean = false;
  }
}
