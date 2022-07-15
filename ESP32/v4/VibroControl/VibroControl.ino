/*
 *******************************************************
 * VibroControl µC Framework
 *
 * Version 4.0, 04.02.2019
 * Author: Felix Laufer
 * Contact: laufer@cs.uni-kl.de
 *
 * AG wearHEALTH
 * Department of Computer Science
 * TU Kaiserslautern (TUK), Germany
 *******************************************************
 */

/*
 ******************************************************* 
 * Config and event handling
 *******************************************************
 */

#include "core/Tools.h"
#include "sensors/Button.h"

Button button0 = Button(26, [](const ButtonEvent& e)
{
  Serial.println(e.timeStamp());
  Serial.println(e.type());
});

/*
 ******************************************************* 
 * Main routines
 *******************************************************
 */ 

void setup()
{
}

// Main loop
void loop()
{
}