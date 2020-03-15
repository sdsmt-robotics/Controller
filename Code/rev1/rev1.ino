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

//range for joystick values
#define JOY_RANGE 1022

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

  //joystick values.
  controller.setJoystick(LEFT, X, scaleJoy(analogRead(JOY_L_X_PIN)));
  controller.setJoystick(LEFT, Y, scaleJoy(analogRead(JOY_L_Y_PIN)));
  controller.setJoystick(RIGHT, X, scaleJoy(analogRead(JOY_R_X_PIN)));
  controller.setJoystick(RIGHT, Y, scaleJoy(analogRead(JOY_R_Y_PIN)));

  //do an update
  controller.update();
}

/**
 * Scale an analog joystick value to get something in the range -1.0 to 1.0.
 * 
 * @param val - the joystick value
 * @return the scaled value.
 */
float scaleJoy(int val) {
  const int middle = 1023 / 2;
  const float scaleFactor = 2.0 / float(JOY_RANGE);
  float newVal;

  //shift
  newVal = val - middle;

  //scale and constrain
  newVal = newVal * scaleFactor;

  //constrain
  return constrain(newVal, -1.0, 1.0);
}
