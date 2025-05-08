#include <HAL.hpp>
#include <HAL.hpp>



void setup()
{

  HAL::HAL_Init();

}

void loop() {
  // 高频处理IMU数据（每1ms）
  static uint32_t lastIMU = 0;
  if (millis() - lastIMU >= 1) {
      lastIMU = millis();
      float currentYaw = H30_Data();
      if (!isnan(currentYaw)) { // 仅更新有效数据
          HAL::yaw = currentYaw;
      }
  }
/*
  // 中频处理GPS（200ms）
  static uint32_t lastGPS = 0;
  if (millis() - lastGPS >= 200) {
      lastGPS = millis();
      char rmc[BUF_SIZE];
      if (gps_read_rmc(rmc)) {
          HAL::lat = Get_lat(rmc);
          HAL::lon = Get_lon(rmc);
      }
  }
*/
  // 水面定向控制（20ms）
  static uint32_t lastWaterCtrl = 0;
  if (millis() - lastWaterCtrl >= 20) {
      lastWaterCtrl = millis();
      HAL::waterDriver.update(HAL::yaw);
  }
/*
  // 导航控制（100ms）
  static uint32_t lastNav = 0;
  if (HAL::navigationStarted && (millis() - lastNav >= 100)) {
      lastNav = millis();
      HAL::navigateToTarget();
  }
*/
  delay(1); // 防止CPU过载
}

