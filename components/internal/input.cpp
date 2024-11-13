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
static TFT_eSprite * sprites[MAX_MEASUREMENTS];
char tag[] = "test";
position *inputPositions;
InputBuffer inputBuf = InputBuffer{10,0,inputPositions};
void init_input(){
  SPI.begin(TOUCHSCREEN_SCLK_PIN, TOUCHSCREEN_MISO_PIN, TOUCHSCREEN_MOSI_PIN);
  SPI.setFrequency(SPI_FREQUENCY);
  ts.begin();
  ts.setRotation(1);
  inputPositions = (position*)calloc(MAX_MEASUREMENTS, 3 * sizeof(uint16_t));
  for ( int count= 0; count < MAX_MEASUREMENTS ; count ++){
    static TFT_eSprite s = TFT_eSprite(getTFT());
    s.setColorDepth(4);
    s.createSprite(12,12);
    s.createPalette(default_4bit_palette);
    s.fillSprite(BACKGROUND_COLOR);
    sprites[count] = &s;
    ESP_LOGI(tag,"init sprite: %i",count);
  }
}

void writeScreenPosition(InputBuffer inputBuf){
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
  auto pos = position{
    (uint16_t)(TFT_HEIGHT * x_pos_as_percent_of_screen),
    (uint16_t)(TFT_WIDTH * y_pos_as_percent_of_screen),
    (uint16_t)p.z
  };
   
  inputBuf.setVal(&pos);
}

void handleAndDrawInput(){
  bool needInputclean = false;
  uint16_t *screenContent=(uint16_t *)calloc(MAX_MEASUREMENTS, 144*sizeof(uint16_t));

  while(ts.touched()){
    yield();
    delay(10);
    int8_t count = 0;
    ESP_LOGI(tag,"start loop ----------");
    if ( !needInputclean ) {
      while ( count < MAX_MEASUREMENTS ){
        writeScreenPosition(inputBuf);
        inputBuf.next();
        count++; 
      }
      needInputclean = true;
      ESP_LOGI(tag,"clean is required: %s", needInputclean ? "true": "false");
    }

    inputBuf.prev();
    if (screenContent[inputBuf.current] == 0 ){
      ESP_LOGI(tag,"buffe/ing %i", inputBuf.current);
      getTFT()->readRect( inputPositions[inputBuf.current].x,inputPositions[inputBuf.current].y, 12, 12, &screenContent[inputBuf.current]); 
      ESP_LOGI(tag,"buffer size is: %i", sizeof(&screenContent[inputBuf.current]));
    }else {
      ESP_LOGI(tag,"filling %i from buffer", inputBuf.current);
      sprites[inputBuf.current]->pushRect(inputPositions[inputBuf.current].x,inputPositions[inputBuf.current].y,0,0,&screenContent[inputBuf.current]);
      inputBuf.next();
    }
    writeScreenPosition(inputBuf);
    count = 0;
    while( count < MAX_MEASUREMENTS ) {
      sprites[inputBuf.current]->fillCircle(6,6,6, FONT_COLOR);
      ESP_LOGI(tag,"drawing %i from input. size is: %i", inputBuf.current,sizeof(&sprites[inputBuf.current]));
      
      sprites[inputBuf.current]-> pushSprite(inputPositions[inputBuf.current].x,inputPositions[inputBuf.current].y);
      count++;
      inputBuf.next();
    }    
  }
  if ( needInputclean ){
    int8_t count = 0;
    inputBuf.prev();
    while( count < MAX_MEASUREMENTS ) {
      ESP_LOGI(tag,"cleaning %i", inputBuf.current);
      sprites[inputBuf.current]->pushRect(
        inputBuf.getVal()->x,
        inputPositions[inputBuf.current].y,
        12,12,
        &screenContent[inputBuf.current]
      );
      inputBuf.prev();
      count++;
    }
    needInputclean = false;
  }
}
