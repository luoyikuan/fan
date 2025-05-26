#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <Ticker.h>
#include <Service/WiFiService.h>
#include <Service/MessageService.h>

const char *ssid = "OpenWrt";       // WiFi名称
const char *password = "Luo123456"; // WiFi密码

const char *mqtt_broker = "";       // mqtt服务器
const int mqtt_port = 1883;         // mqtt端口
const char *mqtt_username = "username"; // mqtt用户名
const char *mqtt_password = "password"; // mqtt密码

#define DHTPIN D4
#define DHTTYPE DHT11

#define PWMPIN D0
#define FGPIN D7

Ticker ticker;
WiFiClient wifiClient;
WiFiService wiFiService(ssid, password);
MessageService messageService(&wifiClient, mqtt_broker, mqtt_port, mqtt_username, mqtt_password);
DHT_Unified dht(DHTPIN, DHTTYPE);

int pwm = 0;             // 风扇转速控制
volatile int rpm = 0;    // 风扇转速
float humidity = 0.0;    // 湿度
float temperature = 0.0; // 温度

volatile int fg = 0;
volatile int count_dht = 0;
volatile int count_send = 0;
void tickerCount();
void fgIntRoutine();

void setup()
{
  analogWriteRange(255);
  pinMode(PWMPIN, OUTPUT);
  analogWrite(PWMPIN, pwm);

  pinMode(FGPIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(FGPIN), fgIntRoutine, RISING);
  dht.begin();

  // 初始化串口
  Serial.begin(9600);
  delay(10);

  // 连接WiFi
  wiFiService.connect();

  // 启动信息服务
  messageService.setup();

  // Ticker定时对象
  ticker.attach_ms(1000, tickerCount);
}

void sendMessage();
void collectDht();

void loop()
{
  // 每隔3秒钟发布一次信息
  if (count_send >= 3)
  {
    sendMessage();
    count_send = 0;
  }

  // 每隔2秒钟采集一次温湿度
  if (count_dht > 2)
  {
    collectDht();
    count_dht = 0;
  }

  messageService.loop();
}

ICACHE_RAM_ATTR
void fgIntRoutine()
{
  ++fg;
}

void tickerCount()
{
  rpm = fg * 30;
  fg = 0;

  ++count_dht;
  ++count_send;
}

void collectDht()
{
  sensors_event_t event;
  dht.temperature().getEvent(&event);
  if (!isnan(event.temperature))
  {
    temperature = event.temperature;
  }

  dht.humidity().getEvent(&event);
  if (!isnan(event.relative_humidity))
  {
    humidity = event.relative_humidity;
  }
}

void sendMessage()
{
  JsonDocument doc;
  doc["pwm"] = pwm;
  doc["rpm"] = rpm;
  doc["humidity"] = humidity;
  doc["temperature"] = temperature;
  String msg;
  serializeJson(doc, msg);
  messageService.send(msg.c_str());
}

void onMessage(JsonDocument msg)
{
  pwm = msg["pwm"];
  analogWrite(PWMPIN, pwm);
}
