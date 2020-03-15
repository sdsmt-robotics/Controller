/**
 * Code for rev3 of the controller. 
 * 
 * This controller does some unique stuff for reading button presses. The buttons are read 
 * as a grid. Four pins (R1, R2, R3, R4) send a pulse to a set of buttons. Then three input 
 * pins (C1, C2, C3) read the values from these buttons. This reduces the number of pins 
 * required to read the 12 buttons. The grid pattern is as shown below:
+----+--------------+-------------+-------------+-----------+
|    |      R1      |     R2      |     R3      |    R4     |
+----+--------------+-------------+-------------+-----------+
| C1 | Button Right | Button Left | Button Down | Button Up |
| C2 | Dpad Down    | Dpad Right  | Dpad Left   | Dpad Up   |
| C3 | Right Bump   | Left Bump   | Left Joy    | Right Joy |
+----+--------------+-------------+-------------+-----------+
 * This does have the side effct however, that when sending a pulse to the set of buttons it
 * takes time for things to settle down. Because of this, there is a delay of 10ms in the 
 * below code. Removing or lowering delay can cause mis-reads, no-reads, and/or possible 
 * heart failure.
 *
 */
 
#include "Controller.h"


//=====DEFINE PINS========================================
//---Joystick pins----
#define JOY_L_X     A3
#define JOY_L_Y     A5
#define JOY_R_X     A2
#define JOY_R_Y     A4

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

//LED
#define LED_PIN 4

//max and min values for the triggers
#define RIGHT_TRIG_MIN 494
#define RIGHT_TRIG_MAX 503
#define LEFT_TRIG_MIN 495
#define LEFT_TRIG_MAX 506

//range for joystick values
#define JOY_RANGE 800

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

  //LED
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);

  //initialize the communications
  controller.init();
}

//=====MAIN LOOP========================================
void loop() {
  //joystick values. Scale the analog values from 1023 down to 1.0.
  controller.setJoystick(LEFT, X, scaleJoy(analogRead(JOY_L_X)));
  controller.setJoystick(LEFT, Y, scaleJoy(analogRead(JOY_L_Y)));
  controller.setJoystick(RIGHT, X, scaleJoy(analogRead(JOY_R_X)));
  controller.setJoystick(RIGHT, Y, scaleJoy(analogRead(JOY_R_Y)));
  
  //trigger values. Scale the analog values from 1023 down to 1.0.
  controller.setTrigger(LEFT, scaleTrigger(analogRead(TRIG_LEFT), LEFT_TRIG_MIN, LEFT_TRIG_MAX));
  controller.setTrigger(RIGHT, scaleTrigger(analogRead(TRIG_RIGHT), RIGHT_TRIG_MIN, RIGHT_TRIG_MAX));

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

/**
 * Scale an analog trigger value to 0.0 to 1.0.
 * 
 * @param val - the trigger value
 * @return the scaled value.
 */
float scaleTrigger(int val, int minVal, int maxVal) {
  //constrain
  val = constrain(val, minVal, maxVal);

  //shift
  val = val - minVal;

  //scale
  return val / float(maxVal - minVal);
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
