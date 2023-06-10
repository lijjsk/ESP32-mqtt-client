/**
  ******************************************************************************
  * @file    srvr.h
  * @author  Waveshare Team
  * @version V1.0.0
  * @date    23-January-2018
  * @brief   ESP8266 WiFi server.
  *          This file provides firmware functions:
  *           + Sending web page of the tool to a client's browser
  *           + Uploading images from client part by part
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
//#include <ESP8266WiFi.h>// ESP8266 and WiFi classes
#include <WiFi.h>
#include "buff.h" // POST request data accumulator
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <Arduino.h>
#include "DEV_Config.h"
#include "EPD.h"
#include "GUI_Paint.h"
/* SSID and password of your WiFi net ----------------------------------------*/
const char *ssid = "Li"; //"your ssid";
const char *password = "13106655625";   //"your password";
const char *mqtt_server = "192.168.149.200";  //mqtt服务器端ipv4地址
const char *TOPIC = "client2";
const char *client_id = "esp32";
/* Server and IP address ------------------------------------------------------*/
// WiFiServer server(80); // Wifi server exemplar using port 80
WiFiClient espclient;
PubSubClient client(espclient);
IPAddress myIP;        // IP address in your local wifi net
// PubSubClient client(server);
/* The 'index' page flag ------------------------------------------------------*/
bool isIndexPage = true; // true : GET  request, client needs 'index' page;
// false: POST request, server sends empty page.
/* Server initialization -------------------------------------------------------*/
//服务初始化，获得网址ip

void setup_wifi() {
 
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
 
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
 
  randomSeed(micros());
 
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}


void callback(char* topic, byte* payload, unsigned int length){
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  Serial.println();
  StaticJsonDocument<2000> doc;
  DeserializationError error = deserializeJson(doc, payload);
  if (error) {
    Serial.print(F("deserializeJson() failed: 解析JSON数据失败"));
    Serial.println(error.c_str());
    return;
  }
  const char* name = doc["name"];
  const char* price = doc["price"];
  const char* stock = doc["stock"];
  Serial.print("name: ");
  Serial.println(name);
  Serial.print("price: ");
  Serial.println(price);
  Serial.print("stock: ");
  Serial.println(stock);
  Serial.println();
  //创建一个新图片缓存
	UBYTE *BlackImage;
	UWORD Imagesize = ((EPD_2in13_V3_WIDTH % 8 == 0)? (EPD_2in13_V3_WIDTH / 8 ): (EPD_2in13_V3_WIDTH / 8 + 1)) * EPD_2in13_V3_HEIGHT;
	if((BlackImage = (UBYTE *)malloc(Imagesize)) == NULL) 
	{
		printf("Failed to apply for black memory...\r\n");
		while (1);
	}
	Paint_NewImage(BlackImage, EPD_2in13_V3_WIDTH, EPD_2in13_V3_HEIGHT, 90, WHITE);
	Paint_Clear(WHITE);
	Debug("Drawing\r\n");
	Paint_SelectImage(BlackImage);
  Paint_DrawString_EN(10, 10, "Name: ", &Font24, WHITE, BLACK);
  Paint_DrawString_EN(10, 50, "Price: ", &Font24, WHITE, BLACK);
  Paint_DrawString_EN(10, 90, "Stock: ", &Font24, WHITE, BLACK);
  Paint_DrawString_EN(120, 10, name, &Font24, WHITE, BLACK);
  Paint_DrawString_EN(120, 50, price, &Font24, WHITE, BLACK);
  Paint_DrawString_EN(120, 90, stock, &Font24, WHITE, BLACK);
  EPD_2in13_V3_Display_Base(BlackImage);
}


void reconnect(){
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // 创建一个随机数后缀的客户端名字
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);
    //    boolean connect(const char* id, const char* user, const char* pass, const char* willTopic, uint8_t willQos, boolean willRetain, const char* willMessage);
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // 一旦连接上了，就发布一个主题，这个主题是自定义的，您可以在自己的MQTT服务器进行查看
      client.publish("test-connect", "hello emqx");
      // subscribe是订阅的意思，MQTT通信的核心就是“订阅+发布”
      client.subscribe(TOPIC);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}


void setup_mqtt(){
  setup_wifi(); 
  client.setServer(mqtt_server,1883);
  client.setCallback(callback);
  
}


void loop_mqtt(){
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}