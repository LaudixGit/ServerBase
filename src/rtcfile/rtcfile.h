/******************************************************************
 * https://github.com/adafruit/RTClib
 * https://pcbartists.com/firmware/esp32-firmware/esp32-ntp-and-ds3231-rtc/
 *   Only update system time (from NTP) once in a month or less often if the RTC time is valid during boot. 
 *     You can store the “last good update” timestamp in EEPROM.
 *   Sync the ESP32 system time to the RTC once in every 6 to 24 hours depending on the temperature of the operating environment. 
 *     Large temperature swings or extreme temperature change will mean larger drifts in the ESP32 RTC time.
 *   Rely on the internal ESP32 system time (struct timeval.tv_usec) for millisecond-accuracy timestamps.
 * 
******************************************************************/

#ifndef rtcfile_h
#define rtcfile_h

#include <Arduino.h>

#include "RTClib.h"

//-------------------------------------------------------------------
extern bool isVerbose;               //declared in ServerBase.h
extern TwoWire *ptrI2C_0;            //declared in ServerBase.h
extern byte i2cAddressError(byte);   //declared in ServerBase.h
extern time_t *ptrNtpTimeLastUpdate; //declared in lan8720.h

extern bool isRtcExists;             //declared in rtcfile.h
extern struct tm *ptrTimeinfo;       //declared in rtcfile.h
extern RTC_DS3231 *ptrRtcTime;       //declared in rtcfile.h

//###################################################################
extern void rtc_setup();
extern void rtc_update();

#endif      //rtcfile_h
