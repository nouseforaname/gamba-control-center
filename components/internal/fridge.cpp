#ifndef FRIDGE_DATA
#define FRIDGE_DATA
#include <stdint.h>

#define SERVICE_UID 1234
#define SERVICE_UUID 00001234-0000-1000-8000-00805f9b34fb
#define COMMAND_UID 1235
#define COMMAND_UUID 00001235-0000-1000-8000-00805f9b34fb
#define NOTIFY_UID 1236
#define NOTIFY_UUID 00001236-0000-1000-8000-00805f9b34fb

enum energyModeEnum { max, eco };
enum batterySaverEnum { low, medium, high};
enum tempUnit { F, C }; // Fahrenheit Celcius
struct unit {
  int8_t desiredTemp;
  int8_t hysteresis; //i have no clue what that effectively does
  int8_t temperature_correction_hot;
  int8_t temperature_correction_mid;
  int8_t temperature_correction_cold;
  int8_t temperature_correction_halt;
  int8_t temp;
};
struct fridge_status {
  int8_t temp_1;
  int8_t temp_2;
};

struct fridge_data {
  bool control_locked;
  bool on;
  energyModeEnum energyMode;
  batterySaverEnum batterySaver;
  int8_t maxTemp;
  int8_t minTemp;
  int8_t startDelay;
  tempUnit tempSetting;
  unit dataUnit1;
  unit dataUnit2;
};
void init_fridge();


#endif
