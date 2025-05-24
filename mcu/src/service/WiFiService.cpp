#include "WiFiService.h"

WiFiService::WiFiService(const char *ssid, const char *password)
{
    _ssid = ssid;
    _password = password;

    // 设置ESP8266工作模式为无线终端模式
    WiFi.mode(WIFI_STA);
}

/**
 * 连接WiFi
 */
void WiFiService::connect()
{
    WiFi.begin(_ssid, _password);

    // 等待WiFi连接,成功连接后输出成功信息
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(1000);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi Connected!");
    Serial.println("");
}