/****************************************************************************************************************************
 *https://ftp.gnu.org/old-gnu/Manuals/glibc-2.2.3/html_chapter/libc_21.html
 * 
 *****************************************************************************************************************************/
#include "lan8720.h"

//-------------------------------------------------------------------
bool wt32EthConnected = false;                     // is the cable connected to the RJ45 jack
time_t ntpTimeLastUpdate;                          // when did ntp last sync the time
time_t *ptrNtpTimeLastUpdate = &ntpTimeLastUpdate; //set the pointer so other files can use this variables

//###################################################################

//-------------------------------------------------------------------
void ETH_event_handler(WiFiEvent_t event){
  if (isVerbose) {Serial.print("ETH_event_handler: Executing on core: "); Serial.print(xPortGetCoreID()); Serial.print(";  priority: "); Serial.println(uxTaskPriorityGet(NULL));}

  switch (event)
  {
    case ARDUINO_EVENT_ETH_START:
      if (isVerbose) {Serial.println(F("ETH starting"));}
      ETH.setHostname(ptrDnsName);
      break;
    case ARDUINO_EVENT_ETH_CONNECTED:
      if (isVerbose) {Serial.println(F("ETH connected"));}
      wt32EthConnected = true;
      break;
    case ARDUINO_EVENT_ETH_GOT_IP:
      if (isVerbose) {Serial.println(F("ETH got IP"));}
      Serial.print(F("URL: http://")); Serial.print(ETH.getHostname()); Serial.println(F(".local"));
      Serial.print(F("ETH MAC: "));    Serial.print(ETH.macAddress());
      Serial.print(F(", IPv4: "));     Serial.print(ETH.localIP());
      if (ETH.fullDuplex())
      {
        Serial.print(", FULL_DUPLEX");
      }
      Serial.print(", ");
      Serial.print(ETH.linkSpeed());
      Serial.println("Mbps");
      connectToMqtt();
      break;
    case ARDUINO_EVENT_ETH_DISCONNECTED:
      Serial.println(F("ETH lost connection"));
      wt32EthConnected = false;
      xTimerStop(*ptrMqttReconnectTimer, 0);   // ensure we don't reconnect to MQTT when no ETH
      break;
    case ARDUINO_EVENT_ETH_STOP:
      Serial.println(F("ETH stops"));
      wt32EthConnected = false;
      xTimerStop(*ptrMqttReconnectTimer, 0);   // ensure we don't reconnect to MQTT when no ETH
      break;

    default:
      break;
  }
}

//-------------------------------------------------------------------
// Callback function (get's called when time adjusts via NTP)
void timeavailable(struct timeval *t)
{
  if (isVerbose){Serial.print(F("Got time adjustment from NTP!  ")); Serial.println(ptrTimeinfo, "%A, %B %d %Y %H:%M:%S");}
  *ptrNtpTimeLastUpdate = time (NULL);

  if (isRtcExists) {
    // update the attached clock
    ptrRtcTime->adjust(t->tv_sec);  //setting RTC to UTC time
    if(isVerbose){Serial.print(F("Compare new to to set time.  NTP: ")); Serial.print(t->tv_sec); Serial.print(F(".  RTC: ")); Serial.println(ptrRtcTime->now().unixtime());}
  }
}

//-------------------------------------------------------------------
void lan8720_setup(){

    // set notification call-back function
  sntp_set_time_sync_notification_cb( timeavailable );
  
  // NTP server address aquired via DHCP,
  // NOTE: This call should be made BEFORE esp32 aquires IP address via DHCP,
  //   otherwise SNTP option 42 would be rejected by default.
  sntp_servermode_dhcp(1);    // (optional)

  // handle TimeZones with daylightOffset. specify a environmnet variable with TimeZone definition including daylight adjustmnet rules.
  // A list of rules for your zone could be obtained from https://github.com/esp8266/Arduino/blob/master/cores/esp8266/TZ.h
  configTzTime(ptrTime_zone, ptrNtpServer1, ptrNtpServer2);
  
  // WT32_ETH01_onEvent();  // cannot uses OEM function; need to add MQTT to case statement  https://github.com/khoih-prog/AsyncWebServer_WT32_ETH01/blob/main/src/AsyncWebServer_WT32_ETH01.cpp
  // To be called before ETH.begin()
  WiFi.onEvent(ETH_event_handler);

  //bool begin(uint8_t phy_addr=ETH_PHY_ADDR, int power=ETH_PHY_POWER, int mdc=ETH_PHY_MDC, int mdio=ETH_PHY_MDIO, 
  //           eth_phy_type_t type=ETH_PHY_TYPE, eth_clock_mode_t clk_mode=ETH_CLK_MODE);
  //ETH.begin(ETH_PHY_ADDR, ETH_PHY_POWER, ETH_PHY_MDC, ETH_PHY_MDIO, ETH_PHY_TYPE, ETH_CLK_MODE);
  ETH.begin(ETH_PHY_ADDR, ETH_PHY_POWER);

  // Set up mDNS responder:
  // - first argument is the domain name, in this example
  //   the fully-qualified domain name is "esp32.local"
  // - second argument is the IP address to advertise
  //   we send our IP address on the WiFi network
  if (MDNS.begin(ptrDnsName)) {
    Serial.println(F("mDNS responder started"));
  } else {
    Serial.println(F("Error setting up mDNS responder!"));
  }

}
