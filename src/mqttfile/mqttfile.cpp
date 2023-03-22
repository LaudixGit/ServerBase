/****************************************************************************************************************************
 *https://ftp.gnu.org/old-gnu/Manuals/glibc-2.2.3/html_chapter/libc_21.html
 * 
 *****************************************************************************************************************************/
#include "mqttfile.h"

#define MQTT_BUFFER_SIZE  (50)
const bool mqttRetain = false;   // should the broker keep a copy of the sent message
//#define MQTT_BROKER_Parameter "mqttBroker"
char MQTT_BROKER_Parameter[] = "mqttBroker";
char *ptrMQTT_BROKER_Parameter = MQTT_BROKER_Parameter;
#define MQTT_BROKER_Size 30
char mqttBroker[MQTT_BROKER_Size] = "test.mosquitto.org";
#define MQTT_PORT_Parameter "mqttport"
int mqttPort = 1883;
#define MQTT_QOS_Parameter "mqttqos"
int mqttQosLevel = 1;  //https://www.hivemq.com/blog/mqtt-essentials-part-6-mqtt-quality-of-service-levels/

#define MQTT_LOCATION_Parameter "mqttlocation"
const int MQTT_LOCATION_Size = 16;
char mqttLocation[MQTT_LOCATION_Size] = "here";
const int MQTT_TOPICROOT_Size = 34;   //MQTT_LOCATION_Size + DnsNameSize + 2
char mqttTopicRoot[MQTT_TOPICROOT_Size];  // the first part of the path sent to the mqtt broker.  e.g., location/hostname/<measurand>
char *ptrMqttTopicRoot = mqttTopicRoot;   //set the pointer so other files can use this array

unsigned long mqttUpdateLastUpdate = 0;  // when was mqtt status last sent
int mqttUpdateLastInterval = 5000;       // once every 5 seconds

#include <AsyncMqtt_Generic.h>   //compile errors if this is in mqttfile.h

AsyncMqttClient mqttClient;
//AsyncMqttClient *ptrMqttClient = &mqttClient;   //set the pointer so other files can use this object
//extern AsyncMqttClient *ptrMqttClient;       //set the pointer so other files can use this object
TimerHandle_t mqttReconnectTimer;
TimerHandle_t *ptrMqttReconnectTimer = &mqttReconnectTimer;

//###################################################################

//return MQTT as a JSON string
String mqttJson() {
  if (isVerbose) {Serial.print("scheduleJson: Executing on core: "); Serial.print(xPortGetCoreID()); Serial.print(";  priority: "); Serial.println(uxTaskPriorityGet(NULL));}

  StaticJsonDocument<450> thisJsonDoc;
  thisJsonDoc["MQTT"]["Broker"] = mqttBroker;
  thisJsonDoc["MQTT"]["Port"] = mqttPort;
  thisJsonDoc["MQTT"]["QoS"] = mqttQosLevel;
  thisJsonDoc["MQTT"]["Location"] = mqttLocation;
  String outJson;
  serializeJson(thisJsonDoc, outJson);
  if (isVerbose) {Serial.print(F("MQTT JSON: ")); Serial.println(outJson);}
  return outJson;
}

//-------------------------------------------------------------------
void setMQTT_BROKER(const char* newValue){
  int rSize = 0;   //to catch the return value (could be used to validate correct info looked up)
  rSize = ServerBaseParameterLookUp(MQTT_BROKER_Parameter,   mqttBroker,   sizeof(mqttBroker), newValue,   true);     //add true as last parameter to update stored value
}

//-------------------------------------------------------------------
void setMQTT_PORT(int newValue){
  int rSize = 0;   //to catch the return value (could be used to validate correct info looked up)
  rSize = ptrServerBaseParameter->putUInt(MQTT_PORT_Parameter, newValue);
  mqttPort = ptrServerBaseParameter->getUInt(MQTT_PORT_Parameter, newValue);
}

//-------------------------------------------------------------------
void setMQTT_QOS(int newValue){
  int rSize = 0;   //to catch the return value (could be used to validate correct info looked up)
  rSize = ptrServerBaseParameter->putUInt(MQTT_QOS_Parameter, newValue);
  mqttQosLevel = ptrServerBaseParameter->getUInt(MQTT_QOS_Parameter, newValue);
}

//-------------------------------------------------------------------
void setMQTT_LOCATION(const char* newValue){
  int rSize = 0;   //to catch the return value (could be used to validate correct info looked up)
  rSize = ServerBaseParameterLookUp(MQTT_LOCATION_Parameter, mqttLocation, sizeof(mqttLocation), newValue, true);
}

//-------------------------------------------------------------------
void connectToMqtt()
{
  if (isVerbose) {Serial.print(F("Connecting to MQTT broker: ")); Serial.print(mqttBroker); Serial.print(F("  port: ")); Serial.println(mqttPort);}
  mqttClient.connect();
}

//-------------------------------------------------------------------
void onMqttConnect(bool sessionPresent)
{
  if (isVerbose) {Serial.print(F("onMqttConnect: Executing on core: ")); Serial.print(xPortGetCoreID()); Serial.print(F(";  priority: ")); Serial.println(uxTaskPriorityGet(NULL));}
  Serial.print("Connected to MQTT broker: "); Serial.print(mqttBroker);
  Serial.print(", port: "); Serial.println(mqttPort);
  Serial.print("mqttTopicRoot: "); Serial.println(mqttTopicRoot);

//  printSeparationLine();
  Serial.print("Session present: "); Serial.println(sessionPresent);

  int maxMsg = 35;
  char tmpMsg[maxMsg];
  int tmpSize=snprintf  (tmpMsg, maxMsg, "%s%s", mqttTopicRoot, "commands");
  if (isVerbose) {Serial.print(F("Subsribed Topic: ")); Serial.println(tmpMsg);}
  uint16_t packetIdSub = mqttClient.subscribe(tmpMsg, 2);
  if (isVerbose) {Serial.print("Subscribing at QoS 2, packetId: "); Serial.println(packetIdSub);}

//  uint16_t packetIdPub2 = mqttClient.publish(mqttPubTopic, 2, true, "test 3a");
//  Serial.print("Publishing at QoS 2, packetId: "); Serial.println(packetIdPub2);

}

//-------------------------------------------------------------------
void onMqttDisconnect(AsyncMqttClientDisconnectReason reason)
{
  (void) reason;
  Serial.println("Disconnected from MQTT.");
  if (wt32EthConnected)
  {
    xTimerStart(mqttReconnectTimer, 0);
  }
}

//-------------------------------------------------------------------
void onMqttSubscribe(const uint16_t& packetId, const uint8_t& qos)
{
  Serial.println("Subscribe acknowledged.");
  Serial.print("  packetId: "); Serial.println(packetId);
  Serial.print("  qos: ");      Serial.println(qos);
}

//-------------------------------------------------------------------
void onMqttUnsubscribe(const uint16_t& packetId)
{
  Serial.println("Unsubscribe acknowledged.");
  Serial.print("  packetId: "); Serial.println(packetId);
}

//-------------------------------------------------------------------
void onMqttMessage(char* topic, char* payload, const AsyncMqttClientMessageProperties& properties,
                   const size_t& len, const size_t& index, const size_t& total)
{
  (void) payload;

  Serial.print("MQTT received:"); Serial.print("  topic:  ");  Serial.print(topic);
  Serial.printf("  payload: %.*s\n", len, payload); //len here refers to # of characters   //https://embeddedartistry.com/blog/2017/07/05/printf-a-limited-number-of-characters-from-a-string/
//  Serial.print("  payload: ");  Serial.println(payload, (int)len);
  if (isVerbose) {
    Serial.print("  qos: ");    Serial.print(properties.qos);
    Serial.print("  dup: ");    Serial.print(properties.dup);
    Serial.print("  retain: "); Serial.print(properties.retain);
    Serial.print("  len: ");    Serial.print(len);
    Serial.print("  index: ");  Serial.print(index);
    Serial.print("  total: ");  Serial.println(total);
  }
}

//-------------------------------------------------------------------
void onMqttPublish(const uint16_t& packetId)
{
    // this is only called if using Qos level 1 or 2
  if (isVerbose) {Serial.print("onMqttPublish: Executing on core: "); Serial.print(xPortGetCoreID()); Serial.print(";  priority: "); Serial.println(uxTaskPriorityGet(NULL));}

  //Serial.print("Publish acknowledged."); Serial.print("  packetId: "); Serial.println(packetId);
}

//-------------------------------------------------------------------
void mqtt_update() {
  if((millis() - mqttUpdateLastUpdate) > mqttUpdateLastInterval) // time to check if update is needed
  {
    int tmpSize;
    int maxTopicSize = 45;
    int maxValueSize = 7;
    char tmpTopic[maxTopicSize];
    char tmpValue[maxValueSize];
    tmpSize=snprintf  (tmpTopic, maxTopicSize, "%s%s", ptrMqttTopicRoot, "core0PercentIdle");
    tmpSize=snprintf  (tmpValue, maxValueSize, "%.2f", core0PercentIdle);
    if (isVerbose) {Serial.print(tmpTopic); Serial.print(": "); Serial.print(tmpValue); }
    mqttClient.publish(tmpTopic, mqttQosLevel, mqttRetain, tmpValue);
    tmpSize=snprintf  (tmpTopic, maxTopicSize, "%s%s", ptrMqttTopicRoot, "core1PercentIdle");
    tmpSize=snprintf  (tmpValue, maxValueSize, "%.2f", core1PercentIdle);
    if (isVerbose) {Serial.print("  "); Serial.print(tmpTopic); Serial.print(": "); Serial.println(tmpValue); }
    mqttClient.publish(tmpTopic, mqttQosLevel, mqttRetain, tmpValue);
    mqttUpdateLastUpdate = millis();
  }
}

//###################################################################
void mqtt_setup(){
  int rSize = 0;   //to catch the return value (could be used to validate correct info looked up)
  mqttPort = ptrServerBaseParameter->getUInt(MQTT_PORT_Parameter, mqttPort);
  mqttQosLevel = ptrServerBaseParameter->getUInt(MQTT_QOS_Parameter, mqttQosLevel);
//Serial.print("mqttBroker: "); Serial.print(mqttBroker); Serial.print("   mqttLocation: "); Serial.println(mqttLocation); 
  rSize = ServerBaseParameterLookUp(MQTT_BROKER_Parameter,   mqttBroker,   sizeof(mqttBroker),   mqttBroker,   false);     //add true as last parameter to update stored value
  rSize = ServerBaseParameterLookUp(MQTT_LOCATION_Parameter, mqttLocation, sizeof(mqttLocation), mqttLocation, false);     //add true as last parameter to update stored value
  rSize=snprintf(mqttTopicRoot, sizeof(mqttTopicRoot), "%s/%s/", mqttLocation, ptrDnsName);
  
  mqttReconnectTimer = xTimerCreate("mqttTimer", pdMS_TO_TICKS(2000), pdFALSE, (void*)0, reinterpret_cast<TimerCallbackFunction_t>(connectToMqtt));

  mqttClient.onConnect(onMqttConnect);
  mqttClient.onDisconnect(onMqttDisconnect);
  mqttClient.onSubscribe(onMqttSubscribe);
  mqttClient.onUnsubscribe(onMqttUnsubscribe);
  mqttClient.onMessage(onMqttMessage);
  mqttClient.onPublish(onMqttPublish);

  mqttClient.setServer(mqttBroker, mqttPort);
}
