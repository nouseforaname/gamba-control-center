#ifndef DISPLAY_DATA
#define DISPLAY_DATA
#include <TFT_eSPI.h>
#include <sys/types.h>
void setBrightness(uint8_t);
void init_display();
TFT_eSPI * getTFT();

#endif
