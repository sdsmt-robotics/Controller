/**
 * Code for rev2 of the controller. 
 * 
 * This controller splits buttons into three sets and reads analog values from a 
 * set to determine which button is pressed. The buttons are wired such that each 
 * button will have a distinct analog value associated with it when pressed. Each 
 * set of buttons is also connected to an interrupt which will trigger when any 
 * of the buttons are pressed.
 * 
 * Also of note: the triggers on this controller are actually buttons. They are 
 * only ever on or off. If they are pressed, the controller will send the max value 
 * for the trigger.
 *
 */

 
#include "Controller.h"
#include "Debouncer.h"

//=====DEFINE PINS========================================
//---Joystick pins----
#define JOY_L_X     A1
#define JOY_L_Y     A0
#define JOY_R_X     A2
#define JOY_R_Y     A3

//---button pins-----
#define RIGHT_BUTTONS A5
#define LEFT_BUTTONS  A4
#define OTHER_BUTTONS   7

//button interrupts
#define RIGHT_BUTTONS_INT INT0
#define LEFT_BUTTONS_INT  INT1
#define OTH_BUTTONS_INT   INT4

//Button analog variance for threshholds
#define VARIANCE  25

//range for joystick values
#define JOY_RANGE 1023

//button enums
//Left side buttons
enum {LEFT_JOY, DPAD_DOWN, DPAD_LEFT, DPAD_UP, DPAD_RIGHT};

//Right side buttons
enum {RIGHT_JOY, BTN_RIGHT, BTN_LEFT, BTN_UP, BTN_DOWN};

//Other buttons
enum {RIGHT_TRIG, LEFT_TRIG, LEFT_BUMP, RIGHT_BUMP};

//thresholds
unsigned leftThresholds[5] = {0};
unsigned rightThresholds[5] = {0};
unsigned otherThresholds[4] = {0};


//Create the communications object. Use Serial for the communications.
Controller controller(Serial1);

//Button set debouncers
Debouncer rightButtons(rightThresholds, 5, RIGHT_BUTTONS);
Debouncer leftButtons(leftThresholds, 5, LEFT_BUTTONS);
Debouncer otherButtons(otherThresholds, 4, OTHER_BUTTONS);


//=====SETUP========================================
void setup() {
    leftThresholds[LEFT_JOY]   = 690  - VARIANCE;
    leftThresholds[DPAD_DOWN]  = 772  - VARIANCE;
    leftThresholds[DPAD_LEFT]  = 855  - VARIANCE;
    leftThresholds[DPAD_UP]    = 940  - VARIANCE;
    leftThresholds[DPAD_RIGHT] = 1023 - VARIANCE;

    rightThresholds[RIGHT_JOY] = 690  - VARIANCE;
    rightThresholds[BTN_RIGHT] = 772  - VARIANCE;
    rightThresholds[BTN_LEFT]  = 855  - VARIANCE;
    rightThresholds[BTN_UP]    = 940  - VARIANCE;
    rightThresholds[BTN_DOWN]  = 1023 - VARIANCE;
    
    otherThresholds[RIGHT_TRIG] = 500  - VARIANCE;
    otherThresholds[LEFT_TRIG]  = 865  - VARIANCE;
    otherThresholds[LEFT_BUMP]  = 945  - VARIANCE;
    otherThresholds[RIGHT_BUMP] = 1023 - VARIANCE;

    //joystick pins
    pinMode(JOY_L_X, INPUT); //joystick
    pinMode(JOY_L_Y, INPUT); //joystick
    pinMode(JOY_R_X, INPUT); //joystick
    pinMode(JOY_R_Y, INPUT); //joystick
    
    //button interrupts (make these inputs just so they don't cause problems)
    pinMode(RIGHT_BUTTONS_INT, INPUT);
    pinMode(LEFT_BUTTONS_INT, INPUT);
    pinMode(OTH_BUTTONS_INT, INPUT);

  //initialize the communications
  controller.init();
}

//=====MAIN LOOP========================================
void loop() {
    //read the button sets
    int rightButtonPress = rightButtons.getPressed();
    int leftButtonPress = leftButtons.getPressed();
    int otherButtonPress = otherButtons.getPressed();

    //set button values
    controller.setDpad(UP, leftButtonPress == DPAD_UP);
    controller.setDpad(DOWN, leftButtonPress == DPAD_DOWN);
    controller.setDpad(LEFT, leftButtonPress == DPAD_LEFT);
    controller.setDpad(RIGHT, leftButtonPress == DPAD_RIGHT);
    controller.setJoyButton(LEFT, leftButtonPress == LEFT_JOY);

    controller.setButton(UP, rightButtonPress == BTN_UP);
    controller.setButton(DOWN, rightButtonPress == BTN_DOWN);
    controller.setButton(LEFT, rightButtonPress == BTN_LEFT);
    controller.setButton(RIGHT, rightButtonPress == BTN_RIGHT);
    controller.setJoyButton(RIGHT, rightButtonPress == RIGHT_JOY);
    
    controller.setBumper(LEFT, otherButtonPress == LEFT_BUMP);
    controller.setBumper(RIGHT, otherButtonPress == RIGHT_BUMP);
    controller.setTrigger(LEFT, (otherButtonPress == LEFT_TRIG ? 1.0 : 0.0));
    controller.setTrigger(RIGHT, (otherButtonPress == RIGHT_TRIG ? 1.0 : 0.0));


    //joystick values. Scale the analog values from 1023 down to 1.0.
    controller.setJoystick(LEFT, X, scaleJoy(analogRead(JOY_L_X)));
    controller.setJoystick(LEFT, Y, scaleJoy(analogRead(JOY_L_Y)));
    controller.setJoystick(RIGHT, X, scaleJoy(analogRead(JOY_R_X) - 40)); // this one is stupid for some reason
    controller.setJoystick(RIGHT, Y, scaleJoy(analogRead(JOY_R_Y)));


    //do an update
    controller.update();

    delay(1);
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
