/**
 * Demo class for the controller receive code.
 * 
 * Uncomment a function in loop() to test out some functionality.
 * 
 * This uses an xbee connected to Serial1. Other serial ports may 
 * be used without issue.
 */
 
#include "Controller.h"


//Create the communications object. Use Serial for the communications.
Controller controller(Serial1);

//Have the class handle the data whenever it arrives
//NOTE: this function is stupid. It is not actually an interrupt. It is only 
//      called when once per loop(). DO NOT USE delay() OR YOU WILL LOOSE STUFF!
void serialEvent1() {
  controller.receiveData();
}

//=====SETUP=============================================
void setup() {
  Serial.begin(115200);
  
  //initialize the communications
  controller.init();
  Serial.println("Waiting for connection...");
  while (!controller.connected()) { delay(10); }
  Serial.println("Connected...");
}

//=====MAIN LOOP=============================================
void loop() {
  //Uncomment a demo mode to run it.

  //Print all values to the serial monitor
  //printEverything();

  //only print button changes (clicks)
  printButtonChanges();
}

/**
 * Display when one of the buttons get pressed (clicked).
 */
void printButtonChanges() {
  //right side buttons
  if (controller.buttonClick(UP)) {
    Serial.println("btn:UP");
  }
  if (controller.buttonClick(DOWN)) {
    Serial.println("btn:DOWN");
  }
  if (controller.buttonClick(LEFT)) {
    Serial.println("btn:LEFT");
  }
  if (controller.buttonClick(RIGHT)) {
    Serial.println("btn:RIGHT");
  }

  //Dpad
  if (controller.dpadClick(UP)) {
    Serial.println("dpad:UP");
  }
  if (controller.dpadClick(DOWN)) {
    Serial.println("dpad:DOWN");
  }
  if (controller.dpadClick(LEFT)) {
    Serial.println("dpad:LEFT");
  }
  if (controller.dpadClick(RIGHT)) {
    Serial.println("dpad:RIGHT");
  }
  
  //bumpers
  if (controller.bumperClick(LEFT)) {
    Serial.println("bumper:LEFT");
  }
  if (controller.bumperClick(RIGHT)) {
    Serial.println("bumper:RIGHT");
  }
}

/**
 * Spam all of the controller values unto the screen.
 */
void printEverything() {
  static unsigned long lastPrint = millis();
  
  if (millis() - lastPrint > 100) {
    //Format:
    //dpad:[lrud],but:[lrud],bump:[lr],trig:[l,r],joyL:[l,r],joyR:[l,r]
    
    //Dpad
    Serial.print("dpad:[");
    Serial.print(controller.dpad(LEFT));
    Serial.print(controller.dpad(RIGHT));
    Serial.print(controller.dpad(UP));
    Serial.print(controller.dpad(DOWN));

    //buttons
    Serial.print("],but:[");
    Serial.print(controller.button(LEFT));
    Serial.print(controller.button(RIGHT));
    Serial.print(controller.button(UP));
    Serial.print(controller.button(DOWN));

    //bumpers
    Serial.print("],bump:[");
    Serial.print(controller.bumper(LEFT));
    Serial.print(controller.bumper(RIGHT));

    //triggers
    Serial.print("],trig:[");
    Serial.print(controller.trigger(LEFT));
    Serial.print(",");
    Serial.print(controller.trigger(RIGHT));

    //left joystick
    Serial.print("],joyL:[");
    Serial.print(controller.joystick(LEFT,X));
    Serial.print(",");
    Serial.print(controller.joystick(LEFT,Y));

    //right joystick
    Serial.print("],joyR:[");
    Serial.print(controller.joystick(RIGHT,X));
    Serial.print(",");
    Serial.print(controller.joystick(RIGHT,Y));
    Serial.println("]");
    
    lastPrint = millis();
  }
}
