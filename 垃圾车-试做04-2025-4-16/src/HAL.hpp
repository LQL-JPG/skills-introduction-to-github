#ifndef _HAL__
#define _HAL__

#include <Arduino.h>
#include "motor.h"
#include "GPS.h"  // 引入 GPS 模块
#include "H30.h"  // 引入 H30 模块
#include "LED.h"

#include "WiFi.h"
const char* ssid = "esp-wifi-web";
const char* password = "12345678";

// WEB SERVER
#include "AsyncTCP.h"
#include "ESPAsyncWebServer.h"
IPAddress LocalIP(192,168,2,2);
IPAddress Gateway(192,168,2,2);
IPAddress SubNet(255,255,255,0);
AsyncWebServer server(80);

namespace HAL {
  double targetLat = 0.0;          // 目标纬度
  double targetLon = 0.0;          // 目标经度
  bool navigationStarted = false;  // 导航是否启动
  const float DISTANCE_THRESHOLD = 0.5; // 目标误差范围（单位：米）
  const float ANGLE_THRESHOLD = 5.0; // 角度误差范围（单位：度）

  double lat = 0.0, lon = 0.0;
  float yaw = 0.0;

  

  class WaterDirectionalDriving {
    private:
        float targetYaw;          // 目标偏航角
        bool isActive;            // 是否激活水面定向行驶
        const float PROPULSION_SPEED = 80;  // 推进器前进速度
        const float TURNING_SPEED = 400;     // 转向轮速度
        const float CORRECTION_THRESHOLD = 5.0f; // 修正阈值(度)
        unsigned long lastUpdateTime = 0;
        const unsigned long UPDATE_INTERVAL = 100; // 100ms更新一次
        
    public:
        WaterDirectionalDriving() : targetYaw(0), isActive(false) {}
        
        // 启动水面定向行驶，使用当前偏航角作为目标
        void start(float currentYaw) {
            targetYaw = currentYaw;
            isActive = true;
            
            // 启动推进器
            motor_function(0, PROPULSION_SPEED);
            motor_function(1, PROPULSION_SPEED);
            
            Serial.printf("水面定向行驶启动 - 目标偏航角: %.2f\n", targetYaw);
        }
        
        // 停止水面定向行驶
        void stop() {
            isActive = false;
            // 停止所有轮子和推进器
            motor_duty_set(MotorC, 0);
            motor_duty_set(MotorD, 0);
            motor_function(0, 0);
            motor_function(1, 0);
            Serial.println("水面定向行驶停止");
        }
        
        // 执行水面定向行驶控制
        void update(float currentYaw) {
          if (!isActive) return;
          
          // 限制更新频率
          unsigned long now = millis();
          if (now - lastUpdateTime < UPDATE_INTERVAL) return;
          lastUpdateTime = now;
          
          float error = calculateAngleError(currentYaw, targetYaw);
          
          // 使用更积极的修正策略
          if (fabs(error) > CORRECTION_THRESHOLD) {
              // 根据误差大小调整转向速度 - 比例控制
              float turningSpeed = map(fabs(error), CORRECTION_THRESHOLD, 180.0f, 
                                      TURNING_SPEED, TURNING_SPEED * 2);
              turningSpeed = constrain(turningSpeed, TURNING_SPEED, TURNING_SPEED * 2);
              
              if (error > 0) {
                  // 右转
                  motor_dir_set(MotorC, 0); motor_duty_set(MotorC, turningSpeed);
                  motor_dir_set(MotorD, 0); motor_duty_set(MotorD, turningSpeed);
              } else {
                  // 左转
                  motor_dir_set(MotorC, 1); motor_duty_set(MotorC, turningSpeed);
                  motor_dir_set(MotorD, 1); motor_duty_set(MotorD, turningSpeed);
              }
          } else {
              maintainDirection();
          }
      }
        
    private:
        // 计算角度误差(-180到180度)
        float calculateAngleError(float current, float target) {
            float error = target - current;
            if (error > 180) error -= 360;
            if (error < -180) error += 360;
            return error;
        }
        
        // 水面转向调整(仅使用C/D轮)
        void adjustWaterSteering(float error) {
            // 推进器保持前进
            motor_function(0, PROPULSION_SPEED);
            motor_function(1, PROPULSION_SPEED);
            
            if (error > 0) {
                // 右转 - C/D轮正向转动
                motor_dir_set(MotorC, 0); motor_duty_set(MotorC, TURNING_SPEED);
                motor_dir_set(MotorD, 0); motor_duty_set(MotorD, TURNING_SPEED);
            } else {
                // 左转 - C/D轮反向转动
                motor_dir_set(MotorC, 1); motor_duty_set(MotorC, TURNING_SPEED);
                motor_dir_set(MotorD, 1); motor_duty_set(MotorD, TURNING_SPEED);
            }
        }
        
        // 保持方向直行
        void maintainDirection() {
            // 停止转向轮
            motor_duty_set(MotorC, 0);
            motor_duty_set(MotorD, 0);
            
            // 推进器保持前进
            motor_function(0, PROPULSION_SPEED);
            motor_function(1, PROPULSION_SPEED);
        }
    };
    
    // 在HAL命名空间中添加水面定向行驶实例
    static WaterDirectionalDriving waterDriver;

    const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html>
<head>
  <meta charset="utf-8">
  <title>两栖垃圾清理车控制面板</title>
  <style>
    /* 全局样式 */
    body {
      font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
      background: linear-gradient(135deg, #f5f7fa, #c3cfe2);
      margin: 0;
      padding: 0;
      display: flex;
      justify-content: center;
      align-items: center;
      min-height: 100vh;
      color: #333;
    }

    h1 {
      text-align: center;
      color: #2c3e50;
      font-size: 2.5em;
      margin-bottom: 20px;
    }

    .container {
      background: white;
      padding: 30px;
      border-radius: 15px;
      box-shadow: 0 10px 30px rgba(0, 0, 0, 0.1);
      max-width: 800px;
      width: 100%;
    }

    .button-row {
      display: flex;
      justify-content: center;
      gap: 15px;
      margin-bottom: 20px;
    }

    .button-row button {
      padding: 15px 25px;
      font-size: 1em;
      cursor: pointer;
      border: none;
      border-radius: 8px;
      background-color: #3498db;
      color: white;
      transition: background-color 0.3s, transform 0.2s;
    }

    .button-row button:hover {
      background-color: #2980b9;
      transform: translateY(-2px);
    }

    .button-row button:active {
      transform: translateY(0);
    }

    .slider-row {
      text-align: center;
      margin: 20px 0;
    }

    .slider-row label {
      font-size: 1.1em;
      color: #2c3e50;
    }

    .slider-row input[type="range"] {
      width: 60%;
      height: 8px;
      background: #ddd;
      border-radius: 5px;
      outline: none;
      opacity: 0.7;
      transition: opacity 0.2s;
    }

    .slider-row input[type="range"]:hover {
      opacity: 1;
    }

    .slider-row input[type="range"]::-webkit-slider-thumb {
      -webkit-appearance: none;
      appearance: none;
      width: 20px;
      height: 20px;
      background: #3498db;
      border-radius: 50%;
      cursor: pointer;
    }

    .data-row {
      text-align: center;
      margin: 20px 0;
      font-size: 1.2em;
      color: #2c3e50;
    }

    .data-row p {
      margin: 10px 0;
    }

    .data-row span {
      font-weight: bold;
      color: #e74c3c;
    }

    .navigation-controls {
      text-align: center;
      margin-top: 20px;
    }

    .navigation-controls input[type="text"] {
      padding: 10px;
      font-size: 1em;
      border: 1px solid #ddd;
      border-radius: 5px;
      margin: 5px;
      width: 150px;
    }

    .navigation-controls button {
      padding: 10px 20px;
      font-size: 1em;
      cursor: pointer;
      border: none;
      border-radius: 5px;
      background-color: #27ae60;
      color: white;
      transition: background-color 0.3s, transform 0.2s;
    }

    .navigation-controls button:hover {
      background-color: #219653;
      transform: translateY(-2px);
    }

    .navigation-controls button:active {
      transform: translateY(0);
    }

    .info-box {
      background: #f9f9f9;
      border: 1px solid #ddd;
      border-radius: 10px;
      padding: 20px;
      margin-top: 20px;
      text-align: left;
    }

    .info-box h3 {
      margin-top: 0;
      color: #2c3e50;
    }

    .info-box p {
      margin: 10px 0;
      font-size: 1.1em;
      color: #333;
    }

    .info-box span {
      font-weight: bold;
      color: #e74c3c;
    }
  </style>
</head>
<body>
  <div class="container">
    <h1>两栖垃圾清理车控制面板</h1>

    <div class="button-row">
      <button onclick="sendCommand('ON')">启动</button>
      <button onclick="sendCommand('OFF')">关闭</button>
    </div>

    <div class="button-row">
      <button onclick="sendCommand('AA')">前进</button>
      <button onclick="sendCommand('BB')">后退</button>
      <button onclick="sendCommand('CC')">左转</button>
      <button onclick="sendCommand('DD')">右转</button>
    </div>

    <div class="button-row">
      <button onclick="sendCommand('EE')">推进器</button>
      <button onclick="sendCommand('FF')">传送带</button>
    </div>

    // <div class="slider-row">
    //   <label for="power">速度:</label>
    //   <input type="range" id="power" name="power" min="0" max="1023" value="0" oninput="adjustPower(this.value)" />
    // </div>

    <div class="slider-row">
      <label for="powerLeft">左轮速度:</label>
      <input type="range" id="powerLeft" name="powerLeft" min="0" max="1023" value="0" oninput="adjustPowerLeft(this.value)" />
    </div>

    <div class="slider-row">
      <label for="powerRight">右轮速度:</label>
      <input type="range" id="powerRight" name="powerRight" min="0" max="1023" value="0" oninput="adjustPowerRight(this.value)" />
    </div>
    
    <div class="slider-row">
  <label for="propulsionSpeed">推进器速度:</label>
  <input type="range" id="propulsionSpeed" name="propulsionSpeed" min="77" max="3800" value="77" oninput="adjustPropulsionSpeed(this.value)" />
    </div>

    <div class="data-row">
      <p>纬度: <span id="latitude">NULL</span></p>
      <p>经度: <span id="longitude">NULL</span></p>
      <p>偏航角: <span id="yaw">NULL</span></p>
    </div>

    <div class="navigation-controls">
      <p>设置导航点:</p>
      <input type="text" id="targetLat" placeholder="目标纬度">
      <input type="text" id="targetLon" placeholder="目标经度">
      <button onclick="setNavigationPoint()">设置导航点</button>
    </div>

    <div class="button-row">
      <button onclick="startNavigation()">定点导航</button>
    </div>

    <div class="button-row">
      <button onclick="startWaterDirection()">启动水面定向</button>
      <button onclick="stopWaterDirection()">停止水面定向</button>
    </div>

  <script>
    // 模拟发送命令到服务器
    function sendCommand(command) {
      var xhttp = new XMLHttpRequest();
      xhttp.open("GET", "/command?value=" + command, true);
      xhttp.send();
    }

    // // 模拟调整速度
    // function adjustPower(power) {
    //   var xhttp = new XMLHttpRequest();
    //   xhttp.open("GET", "/power?value=" + power, true);
    //   xhttp.send();
    // }

    // 调整左轮速度
    function adjustPowerLeft(powerLeft) {
      var xhttp = new XMLHttpRequest();
      xhttp.open("GET", "/powerLeft?value=" + powerLeft, true);
      xhttp.send();
    }

    // 调整右轮速度
    function adjustPowerRight(powerRight) {
      var xhttp = new XMLHttpRequest();
      xhttp.open("GET", "/powerRight?value=" + powerRight, true);
      xhttp.send();
    }

 // 修改更新函数，确保前一个请求完成后再发送下一个
let isUpdating = false;

function updateSensorData() {
    if (isUpdating) return;
    
    isUpdating = true;
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function() {
        if (this.readyState == 4) {
            isUpdating = false;
            if (this.status == 200) {
                try {
                    var data = JSON.parse(this.responseText);
                    document.getElementById("latitude").innerText = data.latitude.toFixed(6);
                    document.getElementById("longitude").innerText = data.longitude.toFixed(6);
                    document.getElementById("yaw").innerText = data.yaw.toFixed(2);
                } catch (e) {
                    console.error("解析数据错误:", e);
                }
            }
        }
    };
    xhttp.open("GET", "/sensor", true);
    xhttp.timeout = 1000; // 设置超时时间
    xhttp.send();
}

// 保持200ms的更新间隔
setInterval(updateSensorData, 50);
    // 设置导航点函数
    function setNavigationPoint() {
      var lat = document.getElementById("targetLat").value;
      var lon = document.getElementById("targetLon").value;
      var xhttp = new XMLHttpRequest();
      xhttp.open("GET", "/setnav?lat=" + lat + "&lon=" + lon, true);
      xhttp.send();
    }

    //设置推进器速度
    function adjustPropulsionSpeed(speed) {
      var xhttp = new XMLHttpRequest();
      xhttp.open("GET", "/propulsionSpeed?value=" + speed, true);
      xhttp.send();
    }

    // 启动导航函数
    function startNavigation() {
      var xhttp = new XMLHttpRequest();
      xhttp.open("GET", "/startnav", true);
      xhttp.send();
    }

      // 启动水面定向行驶
    function startWaterDirection() {
        var xhttp = new XMLHttpRequest();
        xhttp.open("GET", "/startwaterdirection", true);
        xhttp.send();
    }

    // 停止水面定向行驶
    function stopWaterDirection() {
        var xhttp = new XMLHttpRequest();
        xhttp.open("GET", "/stopwaterdirection", true);
        xhttp.send();
    }

    // 每 0.2 秒更新一次传感器数据
    setInterval(updateSensorData, 50);

  </script>
</body>
</html>
    )rawliteral";

    //计算方向差
    float calculateBearing(double lat1, double lon1, double lat2, double lon2, float current_angle) {
        // 将经纬度转换为弧度
        lat1 = radians(lat1);
        lon1 = radians(lon1);
        lat2 = radians(lat2);
        lon2 = radians(lon2);

        // 计算经度差
        float dLon = lon2 - lon1;

        // 计算方位角的y分量和x分量
        float y = sin(dLon) * cos(lat2);
        float x = cos(lat1) * sin(lat2) - sin(lat1) * cos(lat2) * cos(dLon);

        // 计算方位角（以正北为0度）
        float bearing = atan2(y, x);
        bearing = degrees(bearing);
        bearing = fmod((bearing + 360), 360); // 确保角度在0到360之间

        // 将方位角调整为以当前角度为0度
        bearing = fmod((bearing - current_angle + 360), 360);

        return bearing;
    }

    //计算两点距离
    float calculateDistance(double lat1, double lon1, double lat2, double lon2) {
      double dLat = radians(lat2 - lat1);
      double dLon = radians(lon2 - lon1);
      double a = sin(dLat / 2) * sin(dLat / 2) +
                cos(radians(lat1)) * cos(radians(lat2)) *
                sin(dLon / 2) * sin(dLon / 2);
      double c = 2 * atan2(sqrt(a), sqrt(1 - a));
      float distance = 6371000 * c; // 地球半径（单位：米）
      return distance;
    }

    // 导航控制逻辑
    void navigateToTarget() {
       
        double targetLat_OP = lat;//当前纬度
        double targetLon_OP = lon;//当前经度
        float distance = 0;
        float angle = 0;

        if(lat != 0 && lon != 0)
        {
            // 计算距离
            distance = calculateDistance(
              targetLat_OP,
              targetLon_OP,
              targetLat,
              targetLon
            );

            //计算方向差
            angle = calculateBearing(
              targetLat_OP,
              targetLon_OP,
              targetLat,
              targetLon,
              yaw
            );

        }

        if (distance < DISTANCE_THRESHOLD) {
            motor_duty_set(MotorA, 0);
            motor_duty_set(MotorB, 0);
            motor_duty_set(MotorC, 0);
            motor_duty_set(MotorD, 0);
            HAL::navigationStarted = false;
            Serial.println("目标点已到达");
            LED_open();
            return;
        }

        // 先旋转调整方向，再前进
        if (fabs(angle) > ANGLE_THRESHOLD) {
              if (angle > ANGLE_THRESHOLD) {
                // 右转
                motor_dir_set(MotorA, 0); motor_duty_set(MotorA, 720);
                motor_dir_set(MotorB, 1); motor_duty_set(MotorB, 720);
                motor_dir_set(MotorC, 0); motor_duty_set(MotorC, 720);
                motor_dir_set(MotorD, 1); motor_duty_set(MotorD, 720);
            } else if (angle < -ANGLE_THRESHOLD) {
                // 左转
                motor_dir_set(MotorA, 1); motor_duty_set(MotorA, 720);
                motor_dir_set(MotorB, 0); motor_duty_set(MotorB, 720);
                motor_dir_set(MotorC, 1); motor_duty_set(MotorC, 720);
                motor_dir_set(MotorD, 0); motor_duty_set(MotorD, 720);
            } else {
                // 角度误差在可接受范围，停止旋转
                motor_duty_set(MotorA, 0);
                motor_duty_set(MotorB, 0);
                motor_duty_set(MotorC, 0);
                motor_duty_set(MotorD, 0);
            }
        } else {
            motor_dir_set(MotorA, 1);motor_duty_set(MotorA, 700);
            motor_dir_set(MotorB, 1);motor_duty_set(MotorA, 700);
            motor_dir_set(MotorC, 1);motor_duty_set(MotorA, 700);
            motor_dir_set(MotorD, 1);motor_duty_set(MotorA, 700);
            LED_close();
        }
        
    }

    void test_gps()
    {
      Serial.println("纬度: " + String(HAL::targetLat, 6));
      Serial.println("经度: " + String(HAL::targetLon, 6));
    }



    void handleCommand(String command) {
        if (command == "AA") {
            Serial.println("AA");
            motor_dir_set(MotorA, 1);
            motor_dir_set(MotorB, 0);
            motor_dir_set(MotorC, 1);
            motor_dir_set(MotorD, 0);
        } else if (command == "BB") {
            Serial.println("BB");
            motor_dir_set(MotorA, 0);
            motor_dir_set(MotorB, 1);
            motor_dir_set(MotorC, 0);
            motor_dir_set(MotorD, 1);
        } else if (command == "CC") {
            Serial.println("CC"); 
            //motor_dir_set(MotorA, 0); motor_duty_set(MotorA, 400);//左转
            //motor_dir_set(MotorB, 1); motor_duty_set(MotorB, 400);
            motor_dir_set(MotorC, 0); motor_duty_set(MotorC, 360);
            motor_dir_set(MotorD, 0); motor_duty_set(MotorD, 360); 
        } else if (command == "DD") {
            Serial.println("DD");
            //motor_dir_set(MotorA, 1); motor_duty_set(MotorA, 720);//右转
            //motor_dir_set(MotorB, 0); motor_duty_set(MotorB, 720);
            motor_dir_set(MotorC, 1); motor_duty_set(MotorC, 360);
            motor_dir_set(MotorD, 1); motor_duty_set(MotorD, 360);
        }
        else if (command == "EE") {
            Serial.println("EE");
            motor_function(0, 1);
            motor_function(1, 1);
        } 
        else if (command == "FF") {
            Serial.println("FF");
            motor_function(2, 1);
        }  
        else if (command == "ON") {
            // digitalWrite(LED_BUILTIN, HIGH);
            Serial.println("ON");
        } else if (command == "OFF") {
            // digitalWrite(LED_BUILTIN, LOW);
            Serial.println("OFF");
            motor_duty_set(MotorA, 0);
            motor_duty_set(MotorB, 0);
            motor_duty_set(MotorC, 0);
            motor_duty_set(MotorD, 0);
            motor_function(0, 0);
            motor_function(1, 0);
            motor_function(2, 0);
        }
    }
       // 新增 WebSocket 对象
    AsyncWebSocket ws("/ws"); 
    
    // WebSocket 事件处理函数
    void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, 
                  AwsEventType type, void *arg, uint8_t *data, size_t len) {
        if(type == WS_EVT_CONNECT){
            Serial.println("WebSocket 客户端连接");
        }
    }
    
    // 数据通知函数（在 HAL 命名空间内声明）
    void notifyH30Update(float yaw) {
        String json = String("{\"yaw\":") + yaw + "}";
        ws.textAll(json);
    }
      void HAL_Init() {
      motor_init();
      delay(1000);
      // pinMode(LED_BUILTIN, OUTPUT);

      // 初始化串口
      Serial.begin(115200);

      // 初始化 H30
      H30_init();

      // 初始化 GPS
      gps_init();

      LED_init();

      // 初始化 WiFi
      WiFi.softAPConfig(LocalIP, Gateway, SubNet);
      WiFi.softAP(ssid, password);

      // 初始化 Web 服务器
      server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
          request->send_P(200, "text/html", index_html);
      });

      server.on("/command", HTTP_GET, [](AsyncWebServerRequest *request) {
          if (request->hasParam("value")) {
              String command = request->getParam("value")->value();
              handleCommand(command);
          }
          request->send(200, "text/plain", "OK");
      });

      // 在HAL_Init()函数中添加新的路由处理
      server.on("/startwaterdirection", HTTP_GET, [](AsyncWebServerRequest *request) {
        // 使用当前偏航角作为目标方向
        HAL::waterDriver.start(yaw);
        request->send(200, "text/plain", "水面定向行驶已启动");
    });
    
    server.on("/stopwaterdirection", HTTP_GET, [](AsyncWebServerRequest *request) {
        HAL::waterDriver.stop();
        request->send(200, "text/plain", "水面定向行驶已停止");
    });

      // server.on("/power", HTTP_GET, [](AsyncWebServerRequest *request) {
      //     if (request->hasParam("value")) {
      //         String powerValue = request->getParam("value")->value();
      //         int power = powerValue.toInt();
      //         motor_duty_set(MotorA, power);
      //         motor_duty_set(MotorB, power);
      //         motor_duty_set(MotorC, power);
      //         motor_duty_set(MotorD, power);
      //         Serial.println(power);
      //         request->send(200, "text/plain", "OK");
      //     }
      // });

      server.on("/propulsionSpeed", HTTP_GET, [](AsyncWebServerRequest *request) {
        if (request->hasParam("value")) {
            String speedValue = request->getParam("value")->value();
            int speed = speedValue.toInt();
            motor_function(0, speed);  // 控制推进器1
            motor_function(1, speed);  // 控制推进器2
            Serial.println("推进器速度: " + speedValue);
            request->send(200, "text/plain", "OK");
        } else {
            request->send(400, "text/plain", "参数缺失");
        }
    });
      server.on("/powerLeft", HTTP_GET, [](AsyncWebServerRequest *request) {
        if (request->hasParam("value")) {
          String powerValue = request->getParam("value")->value();
          int powerLeft = powerValue.toInt();
          motor_duty_set(MotorB, powerLeft); // 假设MotorB和MotorD是右轮电机
          motor_duty_set(MotorD, powerLeft);
          Serial.println("左轮速度: " + powerValue);
          request->send(200, "text/plain", "OK");
        } else {
          request->send(400, "text/plain", "参数缺失");
        }
      });
      
      server.on("/powerRight", HTTP_GET, [](AsyncWebServerRequest *request) {
        if (request->hasParam("value")) {
          String powerValue = request->getParam("value")->value();
          int powerRight = powerValue.toInt();

          motor_duty_set(MotorA, powerRight); // 假设MotorA和MotorC是左轮电机
          motor_duty_set(MotorC, powerRight);
          Serial.println("右轮速度: " + powerValue);
          request->send(200, "text/plain", "OK");
        } else {
          request->send(400, "text/plain", "参数缺失");
        }
      });

      
      // 新增传感器数据路由
      server.on("/sensor", HTTP_GET, [](AsyncWebServerRequest *request) {
        // 预分配足够大的缓冲区
        static char response[128]; 
        
        // 直接读取最新数据，不检查有效性
        char rmc[BUF_SIZE];
        gps_read_rmc(rmc);
        float currentLat = Get_lat(rmc);
        float currentLon = Get_lon(rmc);
        float currentYaw = H30_Data();
        
        // 直接格式化字符串，避免String对象
        snprintf(response, sizeof(response), 
            "{\"latitude\":%.6f,\"longitude\":%.6f,\"yaw\":%.2f}",
            currentLat, currentLon, currentYaw);
        
        request->send(200, "application/json", response);
    });


     server.on("/setnav", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (request->hasParam("lat") && request->hasParam("lon")) {
        HAL::targetLat = request->getParam("lat")->value().toFloat();
        HAL::targetLon = request->getParam("lon")->value().toFloat();
        Serial.println("导航点设置为:");
        Serial.println("纬度: " + String(HAL::targetLat, 6));
        Serial.println("经度: " + String(HAL::targetLon, 6));
        request->send(200, "text/plain", "导航点设置成功");
    } else {
        request->send(400, "text/plain", "参数缺失");
    }
});
    server.on("/startnav", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (HAL::navigationStarted) {
      
        HAL::navigationStarted = false;

        Serial.println("导航关闭");
        request->send(200, "text/plain", "导航关闭");
    } else {
        HAL::navigationStarted = true;

        Serial.println("导航启动");
        request->send(200, "text/plain", "导航启动");
    }
});

    server.addHandler(&HAL::ws); 
    HAL::ws.onEvent(HAL::onWsEvent);

      server.begin();
    }

}

#endif
