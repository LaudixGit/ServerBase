/**************************************************************
 * I have been trying to convert an existing sketch into a library.
 * This has been unsuccessfull.
 * THIS file is an attempt to create a library from scratch
 * 
 * I2C addresses:
 *   x68: RTC DS3231
 * 
 * 20230205 empty, but valid, framework
 * 20230313 restore RTC and I2C_addresses functionality
 * 
 **************************************************************/
#ifndef SERVERBASE_H
#define SERVERBASE_H

#include <Arduino.h>
#include <Preferences.h>

#define ServerBase_LIBRARY_VERSION 20230312   //only used for information sharing

#if !( defined(ESP32) )
  #error This code is designed for WT32_ETH01 to run on ESP32 platform! Please check your Tools->Board setting.
#endif

#define INCLUDE_vTaskDelay  1   // make sure the compiler includes the vTaskDelay 

//-------------------------------------------------------------------

#include "system/system.h"
#include "mqttfile/mqttfile.h"
#include "rtcfile/rtcfile.h"
#include "lan8720/lan8720.h"
#include "schedulefile/schedulefile.h"
#include "websocketfile/websocketfile.h"
#include "webserverfile/webserverfile.h"

//###################################################################

extern const int SERVERBASE_RX_LED;    //LED3: Serial port indicator, RXD2(IO5) When there is data flow, the indicator light is on;
extern const int SERVERBASE_TX_LED;    //LED4: Serial port indicator, TXD2(IO17) When there is data flow, the indicator light is on;
extern const int SERVERBASE_RELAY_PIN; //Used in LED_Controller to enable the relay
extern const int SERVERBASE_I2C_Freq;  //I2C frequency
extern const int SERVERBASE_SDA_0;     //I2C data pin
extern const int SERVERBASE_SCL_0;     //I2C clock pin
extern char *ptrNtpServer1;            //set the pointer so other files can use this array
extern char *ptrNtpServer2;            //set the pointer so other files can use this array
extern char *ptrTime_zone;             //set the pointer so other files can use this array

//-------------------------------------------------------------------

extern bool isVerbose;               //increase amount of notifications sent to Serial

//###################################################################

class ServerBase
{
  public:
    ServerBase();
    void begin();
  private:
};

//-------------------------------------------------------------------


#endif // SERVERBASE_H