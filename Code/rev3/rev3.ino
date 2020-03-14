/**
 * Code for rev3 of the controller. 
 * 
 */
 
#include "Controller.h"


//=====DEFINE PINS========================================
//---Joystick pins----
#define JOY_L_X     A5
#define JOY_L_Y     A3
#define JOY_R_X     A4
#define JOY_R_Y     A2

//---Trigger pins-----
#define TRIG_LEFT    A0
#define TRIG_RIGHT   A1

//---buttons----------
//read pins
#define C1  2
#define C2  3
#define C3  5

//pulse pins
#define R1  8
#define R2  9
#define R3  10
#define R4  7

#define BR_DD_RB  R1  //pulse pin for: button left, dpad right, & left bumper
#define BL_DR_LB  R2  //pulse pin for: button down, dpad left, & left joystick
#define BD_DL_LJ  R3  //pulse pin for: button up, dpad up, & right joystick
#define BU_DU_RJ  R4  //pulse pin for: button right, dpad down, & right bumper

//standard buttons (left-side)
#define BTN_UP    C1
#define BTN_RIGHT C1
#define BTN_LEFT  C1
#define BTN_DOWN  C1

//dpad (right-side buttons)
#define DPAD_UP    C2
#define DPAD_RIGHT C2
#define DPAD_LEFT  C2
#define DPAD_DOWN  C2

//other buttons
#define JOY_R_BTN  C3
#define JOY_L_BTN  C3
#define BUMP_L     C3
#define BUMP_R     C3

//interval between readin banks of buttons. 
//When we send power to the bank, it takes time for things to settle...
#define READ_SPACING 10  

//Create the communications object. Use Serial for the communications.
Controller controller(Serial);


//=====SETUP========================================
void setup() {
  //joystick pins
  pinMode(JOY_L_X, INPUT); //joystick
  pinMode(JOY_L_Y, INPUT); //joystick
  pinMode(JOY_R_X, INPUT); //joystick
  pinMode(JOY_R_Y, INPUT); //joystick

  //trigger pins
  pinMode(TRIG_LEFT, INPUT); //trigger
  pinMode(TRIG_RIGHT, INPUT); //trigger
  
  //button grid
  pinMode(C1, INPUT);
  pinMode(C2, INPUT);
  pinMode(C3, INPUT);

  pinMode(R1, OUTPUT);
  pinMode(R2, OUTPUT);
  pinMode(R3, OUTPUT);
  pinMode(R4, OUTPUT);

  digitalWrite(R1, LOW);
  digitalWrite(R2, LOW);
  digitalWrite(R3, LOW);
  digitalWrite(R4, LOW);


  //initialize the communications
  controller.init();
}

//=====MAIN LOOP========================================
void loop() {
  //joystick values. Scale the analog values from 1023 down to 1.0.
  controller.setJoystick(LEFT, X, analogRead(JOY_L_X) / 1023.0);
  controller.setJoystick(LEFT, Y, analogRead(JOY_L_Y) / 1023.0);
  controller.setJoystick(RIGHT, X, analogRead(JOY_R_X) / 1023.0);
  controller.setJoystick(RIGHT, Y, analogRead(JOY_R_Y) / 1023.0);
  
  //trigger values. Scale the analog values from 1023 down to 1.0.
  controller.setTrigger(LEFT, analogRead(TRIG_LEFT) / 1023.0);
  controller.setTrigger(RIGHT, analogRead(TRIG_RIGHT) / 1023.0);

  //First bank of buttons
  digitalWrite(BR_DD_RB, HIGH);
  controller.setBumper(RIGHT, digitalRead(BUMP_R));
  controller.setButton(RIGHT, digitalRead(BTN_RIGHT));
  controller.setDpad(DOWN, digitalRead(DPAD_DOWN));
  digitalWrite(BR_DD_RB, LOW);
  delay(READ_SPACING);

  //Second bank of buttons
  digitalWrite(BL_DR_LB, HIGH);
  controller.setBumper(LEFT, digitalRead(BUMP_L));
  controller.setButton(LEFT, digitalRead(BTN_LEFT));
  controller.setDpad(RIGHT, digitalRead(DPAD_RIGHT));
  digitalWrite(BL_DR_LB, LOW);
  delay(READ_SPACING);
  
  //Third bank of buttons
  digitalWrite(BD_DL_LJ, HIGH);
  controller.setJoyButton(LEFT, digitalRead(JOY_L_BTN));
  controller.setButton(DOWN, digitalRead(BTN_DOWN));
  controller.setDpad(LEFT, digitalRead(DPAD_LEFT));
  digitalWrite(BD_DL_LJ, LOW);
  delay(READ_SPACING);
  
  //Fourth bank of buttons
  digitalWrite(BU_DU_RJ, HIGH);
  controller.setJoyButton(RIGHT, digitalRead(JOY_R_BTN));
  controller.setButton(UP, digitalRead(BTN_UP));
  controller.setDpad(UP, digitalRead(DPAD_UP));
  digitalWrite(BU_DU_RJ, LOW);
  delay(READ_SPACING);

  //do an update
  controller.update();
}
