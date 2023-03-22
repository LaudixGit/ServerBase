/******************************************************************
 * 
******************************************************************/

#include "rtcfile.h"

#define DS3231_I2C_ADDRESS 0x68   // note: this is hardcoded in the library

bool isRtcExists = false;           // was RTC found at startup
int rtcCheckInterval = 3600000;     // once an hour
unsigned long rtcCheckLastUpdate;   // when was the local clock checked for update needs
struct tm timeinfo;                 //holds 'current' time. only updated when the websocket refreshes
struct tm *ptrTimeinfo = &timeinfo; //set the pointer so other files can use this variables

RTC_DS3231 rtcTime;                 //avoid and object name "rtc" as that may conflict with other libraries
RTC_DS3231 *ptrRtcTime = &rtcTime;  //set the pointer so other files can use this object

//-------------------------------------------------------------------
// the steps to start RTC - use to retry if needed
bool rtcStart(){
  bool rtcResult = rtcTime.begin(ptrI2C_0);
  if (! rtcResult) {
//    isRtcExists = false;
    Serial.println(F("Couldn't find RTC"));
  } else {
    isRtcExists = true;
    rtcTime.disable32K();    //this sketch doesn't use the 32k signal so disable it 
    rtcTime.clearAlarm(0);   //on the unlikely chance an alarm exists, remove it
    rtcTime.clearAlarm(1);   //on the unlikely chance an alarm exists, remove it
    rtcTime.disableAlarm(0); //this sketch doesn't use the alarm so disable it 
    rtcTime.disableAlarm(1); //this sketch doesn't use the alarm so disable it 
  }
  return rtcResult;
}

//###################################################################
void rtc_setup(){
  bool rtcResult = rtcStart();

  if (isRtcExists){
    DateTime nowDT = rtcTime.now();

    Serial.print("RTC TIME:  ");
    Serial.print(nowDT.year(), DEC);
    Serial.print('/');
    Serial.print(nowDT.month(), DEC);
    Serial.print('/');
    Serial.print(nowDT.day(), DEC);
//    Serial.print(" (");
//    Serial.print(daysOfTheWeek[nowDT.dayOfTheWeek()]);
    Serial.print(" ");
    Serial.print(nowDT.hour(), DEC);
    Serial.print(':');
    Serial.print(nowDT.minute(), DEC);
    Serial.print(':');
    Serial.print(nowDT.second(), DEC);
    Serial.println();

    struct timeval nowTV = { .tv_sec = nowDT.unixtime() };
    settimeofday(&nowTV, NULL);   //set ESP32 internal clock. note: UTC base
    bool gotTime = getLocalTime(ptrTimeinfo);
    if(gotTime){
      Serial.print(F("rtc_setup Time: ")); Serial.println(ptrTimeinfo, "%A, %B %d %Y %H:%M:%S");
    } else {
      Serial.println(F("rtc_setup: Failed to obtain time"));
    }
  }
}

//-------------------------------------------------------------------
//originally the was called "rtc_loop" but now that tasks are used there is on rel 'loop'.
//this updates the system clock IF npt is NOT keeping the system time updated.
//this function is called very frequently, but runs rarely; rtcCheckInterval
void rtc_update() {
  if((millis() - rtcCheckLastUpdate) > rtcCheckInterval) // time to check if update is needed
  {
    if (isRtcExists){
      rtcCheckLastUpdate = millis();
      if (!i2cAddressError(DS3231_I2C_ADDRESS)) { //the device successfully responds (no error)
        if (ptrRtcTime->now().unixtime() > (*ptrNtpTimeLastUpdate+(rtcCheckInterval/1000))){
          // the ntp has NOT been updated since last time this check ran, 
          // so use RTC time to update the ESP32 clock (RTC is more accurate than ESP32)
          struct timeval nowTV = { .tv_sec = rtcTime.now().unixtime() };
          settimeofday(&nowTV, NULL);   //set ESP32 internal clock. note: UTC base
          if (isVerbose){Serial.print(F("Using RTC time since NTP server updates are delayed/missing:  ")); Serial.println(ptrTimeinfo, "%A, %B %d %Y %H:%M:%S");}
        }
      } else {
        //RTC failed to return content. the DS3231 might have gotten disconnected?
        isRtcExists = false;
        if (isVerbose){Serial.println(F("RTC, did exist; no longer responding."));}
      }
    } else {
      // the DS3231 was not detected originally, try again
      byte i2cError = i2cAddressError(DS3231_I2C_ADDRESS);
      if (!i2cError) {
        // the address responded. There MIGHT be an RTC connected now
          bool rtcResult = rtcStart();
          if (rtcResult) {
            if (isVerbose){Serial.println(F("RTC, was missing. Has been added."));}
          }
      }
    }
  }

}
