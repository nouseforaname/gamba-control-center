#ifndef INPUT_SENSOR
#define INPUT_SENSOR
#include "TFT_eSPI.h"
void init_input();
void handleAndDrawInput(TFT_eSPI);

enum directionEnum { left, right, up, down, none };
struct position{
  int16_t x;
  int16_t y;
  int16_t z;
}; 
struct action {
  directionEnum direction;
  bool buttonPressed;
  position coordinates;
};

#endif
