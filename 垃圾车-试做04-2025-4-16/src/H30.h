#ifndef H30_h
#define H30_h
#include <Arduino.h>


#define H30_RX_PIN 17  // G17 作为 RX
#define H30_TX_PIN 18  // G18 作为 TX

#define H30_SERIAL Serial2  // 使用 Serial2，但需要重新配置引脚


float H30_Data();

void H30_init();

#endif