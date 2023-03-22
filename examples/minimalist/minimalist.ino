/**************************************************************
 * goal: run a IoT server as a library
 * 
 * Source: https://github.com/LaudixGit/ServerBase
 * 
 * This file, .ino, initilizes a library called "ServerBase"
 * only 3 lines are needed:
 *   -include
 *   -declare instance
 *   -begin (in setup)
 *  
 * See ServerBase.h for instructions.  https://github.com/LaudixGit/ServerBase
 * 
 * (based on "\\10.0.1.187\jetson\code\classTask-library")
 * 
 * 
 **************************************************************/
#include <ServerBase.h>

ServerBase iotServer;  //declare an instance of the class

void setup() {
  iotServer.begin();
}

void loop() {
  vTaskDelay( 100 );
}
