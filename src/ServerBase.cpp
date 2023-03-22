/**************************************************************
 * https://arduino.github.io/arduino-cli/0.29/library-specification/
 * 
 * 
 * shared array between threads: https://stackoverflow.com/questions/10360394/declaring-a-variable-length-array-as-a-global-variable-in-c
 * global array between files: https://cplusplus.com/forum/beginner/168961/
 * 
 **************************************************************/

#include "ServerBase.h"

//-------------------------------------------------------------------
const int SERVERBASE_RX_LED = 5;
const int SERVERBASE_TX_LED = 17;
const int SERVERBASE_RELAY_PIN = 32;
const int SERVERBASE_I2C_Freq = 100000;
const int SERVERBASE_SDA_0 = 15;       //I2C data pin
const int SERVERBASE_SCL_0 = 14;       //I2C clock pin
char ntpServer1[16] = "pool.ntp.org";
char *ptrNtpServer1 = ntpServer1;
char ntpServer2[16] = "time.nist.gov";
char *ptrNtpServer2 = ntpServer2;
char time_zone[24] = "PST8PDT,M3.2.0,M11.1.0";  // TimeZone rule for TZ_America_Los_Angeles including daylight adjustment rules
char *ptrTime_zone = time_zone;

//-------------------------------------------------------------------

bool isVerbose = true;

//-------------------------------------------------------------------

int xMainTaskPriority = 20;           //Priority of tasks
TaskHandle_t xHandleTaskCore0 = NULL; //Handle in case the task needs to be managed
TaskHandle_t xHandleTaskCore1 = NULL; //Handle in case the task needs to be managed

//###################################################################
//-------------------------------------------------------------------
//Use this thread to run code on core 0 - typically communications processess
void xTaskCore0(void *params) {
  mqtt_setup();
  websocket_setup();
  webserver_setup();
  while(1){
    rtc_update();     // check if the system time is up to date
    mqtt_update();    // 
    system_update();  // publish system status
    vTaskDelay( 1 );  // delay for only 1 tick (but DO delay to release the focus)
  }
}

//-------------------------------------------------------------------
//Use this thread to run code on core 1 - typically sensors and attached hardware
void xTaskCore1(void *params) {
  while(1){
    coreUtilizationCalculation();   //calculate idle rates
    scheduleCheck();                // check if it is time to turn off (or on) 

    vTaskDelay( 1 );  // delay for only 1 tick (but DO delay to release the focus)
  }
}

//-------------------------------------------------------------------
// the class constructor
ServerBase::ServerBase()
{
}

//-------------------------------------------------------------------
void ServerBase::begin(){
  system_setup();

  Serial.print(F("\nServerBase Library version: ")); Serial.println(ServerBase_LIBRARY_VERSION);
  
Serial.print("bootcount: "); Serial.println(ptrServerBaseParameter->getUInt("bootcount", 0));
Serial.print("ptrDnsName: "); Serial.print(ptrDnsName); Serial.print("  Size ptrDnsName: "); Serial.println(strlen(ptrDnsName));

  rtc_setup();
  lan8720_setup();
  schedule_setup();
  
  xTaskCreatePinnedToCore(xTaskCore0, "xTaskCore0", 4096, NULL, xMainTaskPriority, &xHandleTaskCore0, 0);  //. https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/freertos.html
  xTaskCreatePinnedToCore(xTaskCore1, "xTaskCore1", 4096, NULL, xMainTaskPriority, &xHandleTaskCore1, 1);
  configASSERT( xHandleTaskCore0 );  //trap errors during development https://www.freertos.org/a00110.html#configASSERT
  configASSERT( xHandleTaskCore1 );  //trap errors during development
  
  while(true){
    vTaskDelay( 100 );
  }
}
