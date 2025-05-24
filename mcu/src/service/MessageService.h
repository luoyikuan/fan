#ifndef _MessageService_H
#define _MessageService_H

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>

class MessageService
{
private:
    WiFiClient *_wifiClient;
    const char *_url;
    uint16_t _port;
    const char *_username;
    const char *_password;
    PubSubClient _mqttClient;
    void onMessageListener(char *topic, byte *payload, unsigned int length);
    void connect();
public:
    MessageService(WiFiClient *wifiClient, const char *url, uint16_t port, const char *username, const char *password);
    void setup();
    void loop();
    bool send(const char *content);
};

void onMessage(JsonDocument msg);

#endif