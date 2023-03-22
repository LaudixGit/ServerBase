/****************************************************************************************************************************
 * Stuff used for the network device/shield integrated onto the board
 * https://www.waveshare.com/LAN8720-ETH-Board.htm
 * 
 * Useful info from the original library, but not actually use (since the defaults are sufficient):
#define ETH_PHY_TYPE   ETH_PHY_LAN8720    // Type of typedef enum { ETH_PHY_LAN8720, ETH_PHY_TLK110, ETH_PHY_RTL8201, ETH_PHY_DP83848, ETH_PHY_DM9051, ETH_PHY_KSZ8081, ETH_PHY_MAX } eth_phy_type_t;
#define ETH_PHY_MDC    23    // Pin# of the I²C clock signal for the Ethernet PHY
#define ETH_PHY_MDIO   18    // Pin# of the I²C IO signal for the Ethernet PHY
#define ETH_CLK_MODE   ETH_CLOCK_GPIO0_IN  //  ETH_CLOCK_GPIO17_OUT
#define SHIELD_TYPE   "ETH_PHY_LAN8720" 
#define ETH_PHY_ADDR   1    // I²C-address of Ethernet PHY (0 or 1 for LAN8720)
#define ETH_PHY_POWER 16    // Pin# of the enable signal for the external crystal oscillator (-1 to disable for internal APLL source)
 *
 * Time zone config: https://github.com/esp8266/Arduino/blob/master/cores/esp8266/TZ.h
 * example: https://forum.arduino.cc/t/time-library-functions-with-esp32-core/515397/17
 * SimpleTime: https://github.com/espressif/arduino-esp32/blob/master/libraries/ESP32/examples/Time/SimpleTime/SimpleTime.ino
 * API:  https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/system_time.html
 * Structure: https://ftp.gnu.org/old-gnu/Manuals/glibc-2.2.3/html_chapter/libc_21.html
 *
 *****************************************************************************************************************************/

#ifndef lan8720_h
#define lan8720_h

#include <Arduino.h>

#include <ETH.h>        //CANNOT declare here; declare before any functions are defined
#include <WiFi.h>
#include <ESPmDNS.h>
#include "sntp.h"
#include "RTClib.h"

//-------------------------------------------------------------------
extern bool isVerbose;                 //declared in ServerBase.h
extern char *ptrNtpServer1;            //declared in ServerBase.h
extern char *ptrNtpServer2;            //declared in ServerBase.h
extern char *ptrTime_zone;             //declared in ServerBase.h
extern char *ptrDnsName;               //declared in system.h
extern bool isRtcExists;               //declared in rtcfile.h
extern struct tm *ptrTimeinfo;         //declared in rtcfile.h
extern RTC_DS3231 *ptrRtcTime;         //declared in rtcfile.h
extern TimerHandle_t *ptrMqttReconnectTimer; //declared in mqttfile.h

extern bool wt32EthConnected;          //declared in lan8720.h
extern time_t *ptrNtpTimeLastUpdate;   //when did ntp last sync the time

//###################################################################
extern void connectToMqtt();        //declared in mqttfile.h

extern void lan8720_setup();

//-------------------------------------------------------------------

#endif      //lan8720_h
