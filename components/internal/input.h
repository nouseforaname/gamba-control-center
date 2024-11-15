#ifndef INPUT_DATA
#define INPUT_DATA 
#include <stdio.h>
#include <stdint.h>
#include <limits.h>

typedef struct{
  uint16_t x;
  uint16_t y;
  uint16_t z;
} touchEvent;

typedef struct r_buffer {
  size_t length;
  int8_t current;
  touchEvent *pos;

  int8_t next(){
    current = current == length -1 ? 0 : current + 1; 
    return current;
  };
  int8_t prev(){
    current = current == 0 ? length -1 : current - 1;
    return current;
  };
  int8_t last(){
    return current == 0 ? length -1 : current - 1;
  }
  void setVal(touchEvent *input){
     pos[current] = *input;
  }
  touchEvent * getVal(){
    return &pos[current];
  }
} InputBuffer;

void init_input();
void handleAndDrawInput();
#endif
