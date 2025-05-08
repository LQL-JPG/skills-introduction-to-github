#ifndef GPS_H
#define GPS_H

#include <Arduino.h> // 包含 Arduino 核心库
// 定义GPS模块的串口引脚
#define GPS_RX_PIN 16  // G17 作为 RX
#define GPS_TX_PIN 15  // G18 作为 TX

// 使用 HardwareSerial 定义 GPS 串口
#define GPS_SERIAL Serial1  // 使用 Serial1，但需要重新配置引脚

#define BUF_SIZE (1024)    // 缓冲区大小

void gps_init();
bool gps_read_rmc(char *rmc);
double Get_lon(const char *rmc);
double Get_lat(const char *rmc);

// 声明 is_valid_rmc 函数
bool is_valid_rmc(const char *rmc);

#endif // GPS_H