/******************************************************************
 * ORIGINAL: "\\10.0.1.187\jetson\code\ESP32ETH01-Server_Base-multifile"
 * 
 * for Preferences: https://www.tutorialspoint.com/esp32_for_iot/esp32_for_iot_preferences.htm
 * max size of Preferences is 500k  https://github.com/espressif/arduino-esp32/blob/master/libraries/Preferences/src/Preferences.h
 * 
******************************************************************/

#ifndef system_h
#define system_h

#include <Arduino.h>
#include <Preferences.h>
#include <Wire.h>
#include <ArduinoJson.h>
#include <SPIFFS.h>
#include <AsyncWebSocket.h>   //this is already in AsyncWebserver

extern const int SERVERBASE_RX_LED;    //declared in ServerBase.h
extern const int SERVERBASE_TX_LED;    //declared in ServerBase.h
extern const int SERVERBASE_RELAY_PIN; //declared in ServerBase.h
extern const int SERVERBASE_I2C_Freq;  //declared in ServerBase.h
extern const int SERVERBASE_SDA_0;     //declared in ServerBase.h
extern const int SERVERBASE_SCL_0;     //declared in ServerBase.h

extern bool isVerbose;                 //declared in ServerBase.h

//-------------------------------------------------------------------
extern struct tm *ptrTimeinfo;                //declared in rtcfile.h
extern char *ptrMqttTopicRoot;                //declared in mqttfile.h
extern bool isScheduleOn;                     //declared in schedulefile.h
extern AsyncWebSocket *ptrWebsocket;          //declared in websocketfile.h

extern char *ptrDnsName;                      //root of the URL e.g., https://<DNS_ALIAS_NAME>.loca
extern const int DnsNameSize;                 //size of the array
extern byte *ptrI2cAddresses;                 //set the pointer so other files can use this array
extern int restartCount;                      // increase the count of reboots
extern Preferences *ptrServerBaseParameter;   //Access to the ESP32 slice of storage
extern TwoWire *ptrI2C_0;                     //Access to one of the I2C channels
extern float core0PercentIdle;                // store the calculated utilization
extern float core1PercentIdle;                // store the calculated utilization

//###################################################################

extern String systemStatusJson();    //declared in system.h
extern String systemConfigJson();    //declared in system.h
extern String filesJson();           //declared in system.h
extern int ServerBaseParameterLookUp(char*, char*, int, const char*, bool); // retrieve a char array
extern byte i2cAddressError(byte);   // check if a single I2c address is responding
extern void coreUtilizationCalculation();  // calculate the rates for both cores and store each
extern void espRestart();            // force the device to reboot
extern void system_update();         //publish system status
extern void system_setup();

#endif      //system_h
