#include <common.h>
#include <FirebaseFS.h>
#include <Firebase_ESP_Client.h>
#include <Utils.h>
#include <Wire.h>
#include "BlueDot_BME280.h"

FirebaseData firebase_data_object;
FirebaseAuth firebase_auth;
FirebaseConfig firebase_config;

BlueDot_BME280 bme;

struct WiFiData{
  String wifi_ssid;
  String  wifi_password;
  };

struct FirebaseData{
  String project_id;
  String api_key;
  String user_email;
  String user_password;
  };

uint8_t menu_option = 0;
WiFiData wifiData;
FirebaseData firebaseData;

void setup() {
  
  Serial.begin(9600);
}

void loop() {
  if (menu_option == 0) {
    print_menu();
    if (Serial.available() > 0) {
      read_menu_input(char(Serial.read()));
    }
  }
  else {
     char[] inputData;
     uint8_t i = 0;
     while (Serial.available() > 0) {
      inputData[i] = char(Serial.read());
      i++;
    }
    setData(menu_option, inputData);
    }
}

bool sensorInitialisation(BlueDot_BME280 bmeToSet){
  bmeToSet.parameter.communication = 0; //I2C communication for Sensor
  bmeToSet.parameter.I2CAddress = 0x77;   //0x76
  //0b00:     In sleep mode no measurements are performed, but power consumption is at a minimum
  //0b01:     In forced mode a single measured is performed and the device returns automatically to sleep mode
  //0b11:     In normal mode the sensor measures continually (default value)
  bmeToSet.parameter.sensorMode = 0b11; 
  bmeToSet.parameter.IIRfilter = 0b100;                     //16 bit IIR Filter for Sensor
  bmeToSet.parameter.humidOversampling = 0b101;             //16 bit Humidity Oversampling for Sensor
  bmeToSet.parameter.tempOversampling = 0b101;              //16 bit Temperature Oversampling
  bmeToSet.parameter.pressOversampling = 0b101;             //16 bit Pressure Oversampling
  bmeToSet.parameter.pressureSeaLevel = 1013.25;            //default value of 1013.25 hPa
  bmeToSet.parameter.tempOutsideCelsius = 15;               //default value of 15°C
  return bmeToSet.init() != 0x60;
  }

void setData(uint8_t option, char[] dataToSet){
  switch(option){
    case 0: break;
    case 1: 
      wifiData.wifi_ssid = dataToSet;
      break;
    case 2:
      wifiData.wifi_password = dataToSet;
      break;
    case 3:
      firebaseData.project_id = dataToSet;
      break;
    case 4:
      firebaseData.api_key = dataToSet;
      break;
    case 5:
      firebaseData.user_email = dataToSet;
      break;
    case 6:
      firebaseData.user_password = dataToSet;
      break;
    default: break;
    }
  }

bool connect_wifi(WiFiData wifiData) {
  WiFi.begin(wifiData.wifi_ssid, wifiData.wifi_password);
  Serial.print("Подключаюсь к сети");
  uint32_t timer = millis();
  while (WiFi.status() != WL_CONNECTED || timer - millis() < 10000)
  {
    Serial.print(".");
    delay(300);
  }
  return WiFi.status() == WL_CONNECTED;
}

void firebase_init( FirebaseData firebase) {
  firebase_config.api_key = firebase.firebase_api_key;
  firebase_auth.user.email = firebase.firebase_user_email;
  firebase_auth.user.password = firebase.firebase_user_password;
  Firebase.begin(&firebase_config, &firebase_auth);
  Firebase.reconnectWiFi(true);
}

void print_menu() {
  Serial.println(F("Добро пожловать в настройки усройства"));
  Serial.println(F("1. Установка WiFi SSID"));
  Serial.println(F("2. Установка WiFi Password"));
  Serial.println(F("3. Установка Firebase Project ID"));
  Serial.println(F("4. Установка Firebase API Key"));
  Serial.println(F("5. Установка Firebase User Email"));
  Serial.println(F("6. Установка Firebase User Password"));
}

void read_menu_input(uint8_t input_option) {
  switch (input_option) {
    case 1:
      Serial.println(F("Введите WiFi SSID"));
      menu_option = 1;
      break;
    case 2:
      Serial.println(F("Введите WiFi Password"));
      menu_option = 2;
      break;
    case 3:
      Serial.println(F("Введите Firebase Project ID"));
      menu_option = 3;
      break;
    case 4:
      Serial.println(F("Введите Firebase API Key"));
      menu_option = 4;
      break;
    case 5:
      Serial.println(F("Введите Firebase User Email"));
      menu_option = 5;
      break;
    case 6:
      Serial.println(F("Введите Firebase User Password"));
      menu_option = 6;
      break;
    default:
      menu_option = 0;
      Serial.println(F("Неверный пункт меню"));
      break;
  }
}
