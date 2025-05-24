#ifndef _WiFiService_H
#define _WiFiService_H

#include <Arduino.h>
#include <ESP8266WiFi.h>

class WiFiService
{
private:
    const char *_ssid;
    const char *_password;

public:
    WiFiService(const char *ssid, const char *password);
    void connect();
};

#endif