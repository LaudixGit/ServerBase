/**************************************************************
 * 
 **************************************************************/
#include <Arduino.h>
#include "ServerBase.h"

// the class constructor
ServerBase::ServerBase()
{
}


void ServerBase::begin(){
  Serial.print(F("\nServerBase Library version: ")); Serial.println(ServerBase_LIBRARY_VERSION);
}

