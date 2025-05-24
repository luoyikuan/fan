#include "MessageService.h"

MessageService::MessageService(WiFiClient *wifiClient, const char *url, uint16_t port, const char *username, const char *password)
{
    _wifiClient = wifiClient;
    _url = url;
    _port = port;
    _username = username;
    _password = password;
}

void MessageService::setup()
{
    _mqttClient.setClient(*_wifiClient);
    _mqttClient.setServer(_url, _port);

    using std::placeholders::_1;
    using std::placeholders::_2;
    using std::placeholders::_3;
    _mqttClient.setCallback(std::bind(&MessageService::onMessageListener, this, _1, _2, _3));

    connect();
}

void MessageService::loop()
{
    if (_mqttClient.connected())
    {
        // 保持心跳
        _mqttClient.loop();
    }
    else
    {
        // 重新连接
        connect();
    }
}

void MessageService::connect()
{
    // 根据ESP8266的MAC地址生成客户端ID（避免与其它ESP8266的客户端ID重名）
    String clientId = "esp8266-" + WiFi.macAddress();

    // 连接MQTT服务器
    if (_mqttClient.connect(clientId.c_str(), _username, _password))
    {
        Serial.println("MQTT Server Connected.");

        // 订阅
        String topic = "fan/ctrl/" + WiFi.macAddress();
        _mqttClient.subscribe(topic.c_str());
        // bc:ff:4d:40:ba:5b
    }
    else
    {
        Serial.print("MQTT Server Connect Failed. Client State:");
        Serial.println(_mqttClient.state());
        delay(3000);
    }
}

bool MessageService::send(const char *content)
{
    if (_mqttClient.connected())
    {
        String topic = "fan/data/" + WiFi.macAddress();
        return _mqttClient.publish(topic.c_str(), content);
    }
    else
    {
        return false;
    }
}

void MessageService::onMessageListener(char *topic, byte *payload, unsigned int length)
{
    JsonDocument doc;
    deserializeJson(doc, payload, length);
    onMessage(doc);
}
