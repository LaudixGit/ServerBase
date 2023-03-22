/****************************************************************************************************************************
 * 
 * https://github.com/khoih-prog/AsyncMQTT_Generic
 *
 *****************************************************************************************************************************/

#ifndef mqttfile_h
#define mqttfile_h


#include <Preferences.h>
#include <ArduinoJson.h>

//-------------------------------------------------------------------
extern bool isVerbose;                      //declared in ServerBase.h
extern char *ptrDnsName;                    //declared in system.h
extern float core0PercentIdle;              //declared in system.h
extern float core1PercentIdle;              //declared in system.h
extern Preferences *ptrServerBaseParameter; //declared in system.h
extern bool wt32EthConnected;               //declared in lan8720.h


// extern const bool mqttRetain;                //declared in mqttfile.h
// extern int mqttQosLevel;                     //declared in mqttfile.h
extern char *ptrMqttTopicRoot;               //declared in mqttfile.h
extern TimerHandle_t *ptrMqttReconnectTimer; //declared in mqttfile.h

//###################################################################
extern int ServerBaseParameterLookUp(char*, char*, int, const char*, bool); //declared in system.h

extern String mqttJson();           //declared in mqttfile.h
extern void setMQTT_BROKER(const char*);   //declared in mqttfile.h
extern void setMQTT_PORT(int);      //declared in mqttfile.h
extern void setMQTT_QOS(int);       //declared in mqttfile.h
extern void setMQTT_LOCATION(const char*); //declared in mqttfile.h
extern void connectToMqtt();        //declared in mqttfile.h
extern void mqtt_update();          //declared in mqttfile.h
extern void mqtt_setup();


#endif      //mqttfile_h
