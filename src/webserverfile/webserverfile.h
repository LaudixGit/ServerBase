/****************************************************************************************************************************
 * 
 *
 *****************************************************************************************************************************/

#ifndef webserverfile_h
#define webserverfile_h

#include <Arduino.h>

#include <AsyncWebServer_WT32_ETH01.h>
#include <Update.h>  // https://github.com/espressif/arduino-esp32/blob/master/libraries/Update/src/Update.h
#include <SPIFFS.h>

#include "config_html.h"
#include "directory_html.h"

//-------------------------------------------------------------------
extern bool isVerbose;                 //declared in ServerBase.h
extern byte *ptrI2cAddresses;          //declared in system.h
extern int restartCount;               //declared in system.h
extern AsyncWebSocket *ptrWebsocket;   //set the pointer so other files can use this object

//###################################################################

extern void espRestart();              //declared in system.h
extern void onWsEvent(AsyncWebSocket *, AsyncWebSocketClient *, AwsEventType, void *, uint8_t *, size_t); //declared in websocketfile.h

extern void webserver_setup();

#endif      //webserverfile_h
