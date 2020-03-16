/**
 * Code for rev2 of the controller. 
 * 
 * This controller splits buttons into three sets and reads analog values from a 
 * set to determine which button is pressed. The buttons are wired such that each 
 * button will have a distinct analog value associated with it when pressed. Each 
 * set of buttons is also connected to an interrupt which will trigger when any 
 * of the buttons are pressed.
 * 
 * Also of note: the triggers on this controller are digital. If they are pressed 
 * the controller will send the max value for the trigger.
 *
 */

 
#include "Controller.h"


//=====DEFINE PINS========================================
//---Joystick pins----
#define JOY_L_X     A1
#define JOY_L_Y     A0
#define JOY_R_X     A2
#define JOY_R_Y     A3

//---button pins-----
#define RIGHT_BUTTONS A5
#define LEFT_BUTTONS  A4
#define OTH_BUTTONS   7

//button interrupt
#define RIGHT_BUTTONS_INT INT0
#define LEFT_BUTTONS_INT  INT1
#define OTH_BUTTONS_INT   INT4

//Button voltage threshholds
#define VARIANCE  25
#define DPAD_UP_THRESH 940  - VARIANCE
#define DPAD_DN_THRESH 772  - VARIANCE
#define DPAD_LE_THRESH 855  - VARIANCE
#define DPAD_RI_THRESH 1023 - VARIANCE

#define JOY_THRESH 690 - VARIANCE

#define BTN_UP_THRESH 940  - VARIANCE
#define BTN_DN_THRESH 1023 - VARIANCE
#define BTN_LE_THRESH 855  - VARIANCE
#define BTN_RI_THRESH 772  - VARIANCE

#define BMP_LE_THRESH   945  - VARIANCE
#define BMP_RI_THRESH   1023 - VARIANCE
#define TRIG_LE_THRESH  865  - VARIANCE
#define TRIG_RI_THRESH  500  - VARIANCE

//range for joystick values
#define JOY_RANGE 1023

//Create the communications object. Use Serial for the communications.
Controller controller(Serial1);


//=====SETUP========================================
void setup() {
  //joystick pins
  pinMode(JOY_L_X, INPUT); //joystick
  pinMode(JOY_L_Y, INPUT); //joystick
  pinMode(JOY_R_X, INPUT); //joystick
  pinMode(JOY_R_Y, INPUT); //joystick

  //button pins
  pinMode(RIGHT_BUTTONS, INPUT); //buttons
  pinMode(LEFT_BUTTONS, INPUT); //buttons
  pinMode(OTH_BUTTONS, INPUT); //buttons

  //button interrupts
  pinMode(RIGHT_BUTTONS_INT, INPUT);
  pinMode(LEFT_BUTTONS_INT, INPUT);
  pinMode(OTH_BUTTONS_INT, INPUT);
  
  attachInterrupt(RIGHT_BUTTONS_INT, handleRightBtn, CHANGE);
  attachInterrupt(LEFT_BUTTONS_INT, handleLeftBtn, CHANGE);
  attachInterrupt(OTH_BUTTONS_INT, handleOthBtn, CHANGE);

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

    //do an update
    controller.update();

    delay(1);
    //TODO: maybe need to have some delay after a button press before reading to remove jitter.
    //TODO: add a -0.14 shift to the right joystick x axis. It is being stupid.
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

/**
 * Handle a button press on the left set of buttons. Read the analog value from 
 * the set to determine which button is pressed.
 */
void handleLeftBtn() {
    //Track which button is pressed
    bool dpadUp = false;
    bool dpadDown = false;
    bool dpadLeft = false;
    bool dpadRight = false;
    bool joyBtn = false;

    //read the value from the set of buttons
    int val = analogRead(LEFT_BUTTONS);

    //figure out which button is pressed
    if (val > DPAD_RI_THRESH) {
        dpadRight = true;
    } else if (val > DPAD_UP_THRESH) {
        dpadUp = true;
    } else if (val > DPAD_LE_THRESH) {
        dpadLeft = true;
    } else if (val > DPAD_DN_THRESH) {
        dpadDown = true;
    } else if (val > JOY_THRESH) {
        joyBtn = true;
    }

    //set the button to pressed and the rest to not pressed
    controller.setDpad(UP, dpadUp);
    controller.setDpad(DOWN, dpadDown);
    controller.setDpad(LEFT, dpadLeft);
    controller.setDpad(RIGHT, dpadRight);
    controller.setJoyButton(LEFT, joyBtn);
}

/**
 * Handle a button press on the right set of buttons. Read the analog value from 
 * the set to determine which button is pressed.
 */
void handleRightBtn() {
    //Track which button is pressed
    bool btnUp = false;
    bool btnDown = false;
    bool btnLeft = false;
    bool btnRight = false;
    bool joyBtn = false;

    //read the value from the set of buttons
    int val = analogRead(RIGHT_BUTTONS);

    //figure out which button is pressed
    if (val > BTN_DN_THRESH) {
        btnDown = true;
    } else if (val > BTN_UP_THRESH) {
        btnUp = true;
    } else if (val > BTN_LE_THRESH) {
        btnLeft = true;
    } else if (val > BTN_RI_THRESH) {
        btnRight = true;
    } else if (val > JOY_THRESH) {
        joyBtn = true;
    }

    //set the button to pressed and the rest to not pressed
    controller.setButton(UP, btnUp);
    controller.setButton(DOWN, btnDown);
    controller.setButton(LEFT, btnLeft);
    controller.setButton(RIGHT, btnRight);
    controller.setJoyButton(RIGHT, joyBtn);
}

/**
 * Handle a button press on the other set of buttons (bumpers and triggers). Read 
 * the analog value from the set to determine which button is pressed.
 */
void handleOthBtn() {
    //Track which button is pressed
    bool rightBump = false;
    bool leftBump = false;
    bool rightTrig = false;
    bool leftTrig = false;

    //read the value from the set of buttons
    int val = analogRead(OTH_BUTTONS);

    //figure out which button is pressed
    if (val > BMP_RI_THRESH) {
        rightBump = true;
    } else if (val > BMP_LE_THRESH) {
        leftBump = true;
    } else if (val > TRIG_LE_THRESH) {
        leftTrig = true;
    } else if (val > TRIG_RI_THRESH) {
        rightTrig = true;
    }

    //set the button to pressed and the rest to not pressed
    controller.setBumper(LEFT, leftBump);
    controller.setBumper(RIGHT, rightBump);
    controller.setTrigger(LEFT, (leftTrig ? 1.0 : 0.0));
    controller.setTrigger(RIGHT, (rightTrig ? 1.0 : 0.0));
}
