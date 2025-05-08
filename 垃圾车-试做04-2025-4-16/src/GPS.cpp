#include "GPS.h"

// 初始化 GPS 模块
void gps_init() {
    // 配置 Serial2 使用 D34 (RX) 和 D35 (TX)
    GPS_SERIAL.begin(9600, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN);
}

// 读取 RMC 数据
bool gps_read_rmc(char *rmc) {
    static char buffer[BUF_SIZE];
    static int index = 0;

    while (GPS_SERIAL.available()) {
        char c = GPS_SERIAL.read();
        if (c == '\n') {
            buffer[index] = '\0'; // 结束字符串
            if (strstr(buffer, "$GNRMC") != NULL) {
                // 检查RMC数据是否有效
                if (is_valid_rmc(buffer)) {
                    strncpy(rmc, buffer, BUF_SIZE); // 复制 RMC 数据
                    index = 0; // 重置索引
                    return true;
                }
            }
            index = 0; // 重置索引
        } else {
            if (index < BUF_SIZE - 1) {
                buffer[index++] = c; // 存储字符
            }
        }
    }
    return false;
}

// 检查RMC数据是否有效
bool is_valid_rmc(const char *rmc) {
    char rmc_copy[BUF_SIZE];
    strncpy(rmc_copy, rmc, BUF_SIZE);

    char *token = strtok(rmc_copy, ",");
    int field = 0;
    double lat = 0.0;
    double lon = 0.0;

    while (token != NULL) {
        if (field == 3) { // 纬度值
            lat = atof(token);
        }
        if (field == 5) { // 经度值
            lon = atof(token);
        }
        token = strtok(NULL, ",");
        field++;
    }

    // 如果经纬度为0，则认为数据无效
    return (lat != 0.0 && lon != 0.0);
}

// 获取经度
double Get_lon(const char *rmc) {
    char rmc_copy[BUF_SIZE];
    strncpy(rmc_copy, rmc, BUF_SIZE);

    char *token = strtok(rmc_copy, ",");
    int field = 0;
    double lon = 0.0;
    char lon_dir = 'E';

    while (token != NULL) {
        if (field == 5) { // 经度值
            lon = atof(token);
        }
        if (field == 6) { // 经度方向 (E/W)
            lon_dir = token[0];
        }
        token = strtok(NULL, ",");
        field++;
    }

    lon = (lon / 100.0) + (lon - (lon / 100.0) * 100) / 60.0; // 转换为十进制
    if (lon_dir == 'W') {
        lon = -lon; // 如果是西经，转换为负数
    }

    return lon;
}

// 获取纬度
double Get_lat(const char *rmc) {
    char rmc_copy[BUF_SIZE];
    strncpy(rmc_copy, rmc, BUF_SIZE);

    char *token = strtok(rmc_copy, ",");
    int field = 0;
    double lat = 0.0;
    char lat_dir = 'N';

    while (token != NULL) {
        if (field == 3) { // 纬度值
            lat = atof(token);
        }
        if (field == 4) { // 纬度方向 (N/S)
            lat_dir = token[0];
        }
        token = strtok(NULL, ",");
        field++;
    }

    lat = (lat / 100.0) + (lat - (lat / 100.0) * 100) / 60.0; // 转换为十进制
    if (lat_dir == 'S') {
        lat = -lat; // 如果是南纬，转换为负数
    }

    return lat;
}