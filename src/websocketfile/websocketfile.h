/****************************************************************************************************************************
 * 
 *
 *****************************************************************************************************************************/

#ifndef websocketfile_h
#define websocketfile_h

#include <Arduino.h>

#include <ArduinoJson.h>
#include <AsyncWebSocket.h>   //this is already in AsyncWebserver

extern bool isVerbose;                 //declared in ServerBase.h
extern const int SERVERBASE_RX_LED;    //declared in ServerBase.h
extern const int SERVERBASE_TX_LED;    //declared in ServerBase.h
extern bool isScheduleOn;              //declared in schedulefile.h

extern AsyncWebSocket *ptrWebsocket;   //set the pointer so other files can use this object


//###################################################################

extern String systemStatusJson();        //declared in system.h
extern String systemConfigJson();        //declared in system.h
extern String filesJson();               //declared in system.h
extern int ServerBaseParameterLookUp(char*, char*, int, const char*, bool); //declared in system.h
extern String scheduleJson();            //declared in schedulefile.h
extern void scheduleEnableUpdate(bool);  //declared in schedulefile.h
extern void scheduleUpdate(unsigned long,unsigned long); //declared in schedulefile.h
extern String mqttJson();                //declared in mqttfile.h
extern void setMQTT_BROKER(const char*);        //declared in mqttfile.h
extern void setMQTT_PORT(int);           //declared in mqttfile.h
extern void setMQTT_QOS(int);            //declared in mqttfile.h
extern void setMQTT_LOCATION(const char*);      //declared in mqttfile.h


extern void onWsEvent(AsyncWebSocket *, AsyncWebSocketClient *, AwsEventType, void *, uint8_t *, size_t); 
extern void websocket_setup();

//-------------------------------------------------------------------

#endif      //websocketfile_h
