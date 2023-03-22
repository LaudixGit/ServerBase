/****************************************************************************************************************************
 * 
 *
 *****************************************************************************************************************************/

#ifndef schedulefile_h
#define schedulefile_h

#include <Arduino.h>

#include <Preferences.h>
#include <ArduinoJson.h>
#include <AsyncWebSocket.h>   //this is already in AsyncWebserver

extern bool isVerbose;                 //declared in ServerBase.h
extern Preferences *ptrServerBaseParameter;   //declared in ServerBase.h
extern AsyncWebSocket *ptrWebsocket;          //declared in websocketfile.h

extern bool isScheduleOn;              // is the accessory supposed to be running

//###################################################################

extern String scheduleJson();     //return schedule as a JSON string
extern void scheduleEnableUpdate(bool);  // a simple fuction to set the override parameter
extern void scheduleUpdate (unsigned long,unsigned long); // sets the variables when the user updates the webpage

extern void scheduleCheck(void);  // check if it is time to turn off (or on) 
extern void schedule_setup();

#endif      //schedulefile_h
