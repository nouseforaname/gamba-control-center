#include "display.h"
#include "main/settings.h"
#include <SPI.h>
#include <TFT_eSPI.h>
#include <XPT2046_Touchscreen.h>
#include <stdio.h>
#include "input.h"
#include "esp_log.h"
#include "display.h"
XPT2046_Touchscreen ts(TOUCHSCREEN_CS_PIN);
static TFT_eSprite * touch_event_sprites[MAX_MEASUREMENTS];
char tag[] = "input";
static touchEvent *input_events = (touchEvent*)calloc(MAX_MEASUREMENTS, 3 * sizeof(uint16_t));
static InputBuffer input_buf = InputBuffer{MAX_MEASUREMENTS,0,input_events};
void init_input(){
  SPI.begin(TOUCHSCREEN_SCLK_PIN, TOUCHSCREEN_MISO_PIN, TOUCHSCREEN_MOSI_PIN);
  ts.begin();
  ts.setRotation(1);
  for ( int count= 0; count < MAX_MEASUREMENTS ; count ++){
    static TFT_eSprite s = TFT_eSprite(getTFT());
    s.setColorDepth(4);
    s.createSprite(12,12);
    s.createPalette(default_4bit_palette);
    s.fillSprite(BACKGROUND_COLOR);
    touch_event_sprites[count] = &s;
    ESP_LOGD(tag,"init sprite: %i",count);
  }
}

void writeScreenPosition(InputBuffer input_buf){
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

  x_pos_as_percent_of_screen = 1- ((float)p.x - offset_x) / range_x;
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
  touchEvent latest_input_event = touchEvent{
    (uint16_t)(TFT_HEIGHT * x_pos_as_percent_of_screen),
    (uint16_t)(TFT_WIDTH * y_pos_as_percent_of_screen),
    (uint16_t)p.z
  };
   
  input_buf.setVal(&latest_input_event);
}

void handleAndDrawInput(){
  bool need_input_clean = false;

  while(ts.touched()){
    if ( !need_input_clean ) {
      auto until = input_buf.last();
      while ( input_buf.next() != until ){
        writeScreenPosition(input_buf);
        touch_event_sprites[input_buf.current]->fillCircle(6,6,6, BACKGROUND_COLOR);
      }
      need_input_clean = true;
      ESP_LOGD(tag,"clean is required: %s", need_input_clean ? "true": "false");
    }
    ESP_LOGD(tag,"resetting %i to black", input_buf.current);
    touch_event_sprites[input_buf.current]->fillCircle(6,6,6, BACKGROUND_COLOR);
    touch_event_sprites[input_buf.current]->pushSprite(input_buf.getVal()->x,input_buf.getVal()->y);
    ESP_LOGD(tag,"wrote latest event to %i", input_buf.current);
    writeScreenPosition(input_buf);
    auto until = input_buf.last();
    while( input_buf.next() != until ) {
      touch_event_sprites[input_buf.current]->fillCircle(6,6,6, FONT_COLOR);
      touch_event_sprites[input_buf.current]->pushSprite(input_buf.getVal()->x,input_buf.getVal()->y);
    } 
  }
  if ( need_input_clean ){
    auto next = input_buf.next();
    while( input_buf.prev() != next ) {
      ESP_LOGD(tag,"cleaning %i", input_buf.current);
      touch_event_sprites[input_buf.current]->fillCircle(6,6,6, BACKGROUND_COLOR);
      touch_event_sprites[input_buf.current]->pushSprite(input_buf.getVal()->x,input_buf.getVal()->y);
    }
    need_input_clean = false;
  }
}
