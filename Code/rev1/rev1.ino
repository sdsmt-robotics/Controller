/**
 * Code for rev1 of the controller. 
 * 
 * This controller does not have triggers, bumpers, or buttons on the joysticks, 
 * so those functions do nothing.
 */
 
#include "Controller.h"

//Define pins
#define JOY_L_Y_PIN     A1
#define JOY_L_X_PIN     A2
#define JOY_R_Y_PIN     A0
#define JOY_R_X_PIN     A3
#define JOY_L_BUT_PIN  -1 //unused
#define JOY_R_BUT_PIN  -1 //unused

#define BUT_L_U_PIN 2
#define BUT_L_D_PIN A5
#define BUT_L_L_PIN 6
#define BUT_L_R_PIN 3

#define BUT_R_U_PIN 5
#define BUT_R_D_PIN A4
#define BUT_R_L_PIN 7
#define BUT_R_R_PIN 4

//Create the communications object. Use Serial for the communications.
Controller controller(Serial);

void setup() {
  //initialize pins
  pinMode(JOY_L_Y_PIN, INPUT); //joystick
  pinMode(JOY_L_X_PIN, INPUT); //joystick
  pinMode(JOY_R_Y_PIN, INPUT); //joystick
  pinMode(JOY_R_X_PIN, INPUT); //joystick
  
  pinMode(BUT_R_L_PIN, INPUT);
  pinMode(BUT_R_R_PIN, INPUT);
  pinMode(BUT_R_U_PIN, INPUT);
  pinMode(BUT_R_D_PIN, INPUT);

  pinMode(BUT_L_L_PIN, INPUT);
  pinMode(BUT_L_R_PIN, INPUT);
  pinMode(BUT_L_U_PIN, INPUT);
  pinMode(BUT_L_D_PIN, INPUT);

  //initialize the communications
  controller.init();
}

void loop() {
  //Right set of buttons
  controller.setButton(LEFT,  digitalRead(BUT_R_L_PIN));
  controller.setButton(RIGHT, digitalRead(BUT_R_R_PIN));
  controller.setButton(UP,    digitalRead(BUT_R_U_PIN));
  controller.setButton(DOWN,  digitalRead(BUT_R_D_PIN));

  //left set of buttons (dpad, but not really)
  controller.setDpad(LEFT,  digitalRead(BUT_L_L_PIN));
  controller.setDpad(RIGHT, digitalRead(BUT_L_R_PIN));
  controller.setDpad(UP,    digitalRead(BUT_L_U_PIN));
  controller.setDpad(DOWN,  digitalRead(BUT_L_D_PIN));

  //joystick values. Scale the analog values from 1023 down to 1.0.
  controller.setJoystick(LEFT, X, analogRead(JOY_L_X_PIN) / 1023.0);
  controller.setJoystick(LEFT, Y, analogRead(JOY_L_Y_PIN) / 1023.0);
  controller.setJoystick(RIGHT, X, analogRead(JOY_R_X_PIN) / 1023.0);
  controller.setJoystick(RIGHT, Y, analogRead(JOY_R_Y_PIN) / 1023.0);

  //do an update
  controller.update();
}
