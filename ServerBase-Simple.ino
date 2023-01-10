/**************************************************************
 * goal: run a IoT server as a library
 * 
 * https://www.circuitbasics.com/programming-with-classes-and-objects-on-the-arduino/
 * https://docs.arduino.cc/learn/contributions/arduino-creating-library-guide
 * https://roboticsbackend.com/arduino-object-oriented-programming-oop/
 * 
 * This file, .ino, initilizes a library called "ServerBase"
 * only 3 lines are needed:
 *   -include
 *   -declare instance
 *   -begin (in setup)
 *  
 * with default settings, toggles the RX LED every half second, and sends a messge to Serial
 * 
 * (based on "\\10.0.1.187\jetson\code\classTask-library")
 * 
 * 
 **************************************************************/
#include "ServerBase.h"

ServerBase iotServer();  //declare an instance of the class

void setup() {
  ledServer.begin();
}

void loop() {
  vTaskDelay( 100 );
}
