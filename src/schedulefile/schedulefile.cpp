/****************************************************************************************************************************
 * creeate a simple daily timer to turn stuff on/off
 *****************************************************************************************************************************/
#include "schedulefile.h"

//###################################################################

#define SCHEDULE_ONSECOND_Parameter "slOnSec"
#define SCHEDULE_ONDURATION_Parameter "slOnDrtn"
#define SCHEDULE_ISON_Parameter "slIsOn"
#define SCHEDULE_ISENABLED_Parameter "slIsEn"

bool isScheduleOn = false;  // is the accessory supposed to be running
bool isScheduleEnabled = true; // is the schedule supposed to be controlling the accessories
unsigned long scheduleOnSecond = 22*60*60; // Which second of the day to turn on
unsigned long scheduleOnDuration = 60*60;   //6*60*60;  // =3600=1hour. elapsed number of seconds for the accessory to remain on
time_t scheduleOnTime = scheduleOnSecond;  // when to turn on the accessory
time_t scheduleOffTime = scheduleOnSecond+scheduleOnDuration;  // when to turn off the accessory.  This is calculated: OnTime + Duration = OffTime
struct tm timeBuffer;   // reserve space to avoid dynamically creating a tm structure (which is not thread safe)
int scheduleCheckInterval = 10000;   //30000;     // once an hour
unsigned long scheduleCheckLastUpdate;   // when was the local clock checked for update needs

//###################################################################

//return schedule as a JSON string
String scheduleJson() {
  if (isVerbose) {Serial.print("scheduleJson: Executing on core: "); Serial.print(xPortGetCoreID()); Serial.print(";  priority: "); Serial.println(uxTaskPriorityGet(NULL));}

  StaticJsonDocument<450> thisJsonDoc;
  thisJsonDoc["SL"]["OnSecond"] = scheduleOnSecond;
  thisJsonDoc["SL"]["OnDuration"] = scheduleOnDuration;
  thisJsonDoc["SL"]["OnTime"] = scheduleOnTime;
  thisJsonDoc["SL"]["OffTime"] = scheduleOffTime;
  thisJsonDoc["SL"]["isOn"] = isScheduleOn;
  thisJsonDoc["SL"]["isEnabled"] = isScheduleEnabled;
  String outJson;
  serializeJson(thisJsonDoc, outJson);
  if (isVerbose) {Serial.print(F("SCHEDULE JSON: ")); Serial.println(outJson);}
//Serial.print(F("SCHEDULE JSON: ")); Serial.println(outJson);
  return outJson;
}

//-------------------------------------------------------------------
//do I need a FUNCTION for this? or just set the variable?
// void setScheduleStatus (bool newStatus){
  // if (isScheduleOn != newStatus) {
    // isScheduleOn = newStatus;
  // }
// }

//-------------------------------------------------------------------
// check if it is time to turn off (or on) 
// assumes a daily schedule - each day turn on at OnTime and turn off Duration later
void scheduleCheck(void) {
  if(!isScheduleEnabled){
    // schedule has been manually paused
    return;   // nothing to do, so exit
  }
  if((millis() - scheduleCheckLastUpdate) > scheduleCheckInterval) // time to check if update is needed
  {
    bool initialOnSetting = isScheduleOn;
    time_t initialOnTime = scheduleOnTime;
    time_t initialOffTime = scheduleOffTime;
    if(getLocalTime(&timeBuffer)){
      time_t timeNow = 0; 
      timeNow = mktime(&timeBuffer);
      timeBuffer.tm_hour  = 0;
      timeBuffer.tm_min  = 0;
      timeBuffer.tm_sec  = 0;
      time_t todayMidnight = 0; 
      todayMidnight = mktime(&timeBuffer);
      if (scheduleOnTime > timeNow) {
        // on is future
        if (scheduleOffTime > timeNow) {
          // on is future, off is future
          // accessory should be off
          isScheduleOn = false;
        } else {
          // on is future, off is past
          // error this shouldn't happen. Erase current times
          scheduleOnTime = 0+scheduleOnSecond;
          scheduleOffTime = scheduleOnTime + scheduleOnDuration;
        }
      } else {
        // on is past
        if (scheduleOffTime > timeNow) {
          // on is past, off is future
          // accessory should be on
          //isScheduleOn = true;
          isScheduleOn = true;
        } else {
          // on is past, off is past
          // accessory should be off
          //isScheduleOn = false;
          isScheduleOn = false;
          // reset schedule times
          scheduleOnTime = todayMidnight+scheduleOnSecond;
          scheduleOffTime = scheduleOnTime + scheduleOnDuration;
        }
      } 
    } else {
      // time is not yet available
      Serial.println(F("Unable to obtain time. Schedule NOT updated!"));
    }
    if((initialOnSetting != isScheduleOn) || (initialOnTime != scheduleOnTime) || (initialOffTime != scheduleOffTime)){
      //something is changed send an update
      ptrWebsocket->printfAll("%s", scheduleJson().c_str());   // update all connected clients.
    }
  }
}

//-------------------------------------------------------------------
// sets the variables when the user updates the webpage
void scheduleUpdate (unsigned long  startSecond,unsigned long durationSecond){
  unsigned long result;
  result = ptrServerBaseParameter->putULong(SCHEDULE_ONSECOND_Parameter, startSecond);
  result = ptrServerBaseParameter->putULong(SCHEDULE_ONDURATION_Parameter, durationSecond);
  scheduleOnSecond =   ptrServerBaseParameter->getULong(SCHEDULE_ONSECOND_Parameter,   scheduleOnSecond);
  scheduleOnDuration = ptrServerBaseParameter->getULong(SCHEDULE_ONDURATION_Parameter, scheduleOnDuration);
  scheduleCheck();
}

//-------------------------------------------------------------------
// a simple fuction to set the override parameter
void scheduleEnableUpdate(bool newSetting){
  bool result;
  result = ptrServerBaseParameter->putBool(SCHEDULE_ISENABLED_Parameter, newSetting);
  isScheduleEnabled =  ptrServerBaseParameter->getBool (SCHEDULE_ISENABLED_Parameter,  isScheduleEnabled);
  if (!isScheduleEnabled) {
    // in override mode need to also set/retrieve the on-state
    result = ptrServerBaseParameter->putBool(SCHEDULE_ISON_Parameter, isScheduleOn);
    isScheduleOn = ptrServerBaseParameter->getBool (SCHEDULE_ISON_Parameter, isScheduleOn);
  }
}

//###################################################################
void schedule_setup(){
  //retrieve storge parameters
  scheduleOnSecond =   ptrServerBaseParameter->getULong(SCHEDULE_ONSECOND_Parameter,   scheduleOnSecond);
  scheduleOnDuration = ptrServerBaseParameter->getULong(SCHEDULE_ONDURATION_Parameter, scheduleOnDuration);
  isScheduleEnabled =  ptrServerBaseParameter->getBool (SCHEDULE_ISENABLED_Parameter,  isScheduleEnabled);
  scheduleUpdate (scheduleOnSecond,scheduleOnDuration);
  if(isVerbose){Serial.print(F("scheduleOnTime: ")); Serial.print(scheduleOnTime); Serial.print(F("   scheduleOffTime: ")); Serial.println(scheduleOffTime);}
  if (!isScheduleEnabled) {
    // in override mode need to also retrieve the on-state
    bool result = ptrServerBaseParameter->getBool (SCHEDULE_ISON_Parameter, isScheduleOn);
    isScheduleOn = result;   //ensure the accessories match the manual set state
  }
}

//-------------------------------------------------------------------
void schedule_loop() {

}

