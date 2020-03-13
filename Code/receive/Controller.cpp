/*
 * Class for receiving data from the controller.
 * 
 * Functions:
 * init() - initilize receiving. Call in setup();
 * connected() - check if the controller is currently connected. 
 * receiveData() - read any data that has been sent to the receiver. Call this often or you will loose stuff.
 *
 * joystick(side, axis) - get the joystick for the given side and axis
 * trigger(side) - get the trigger value on the given side
 *
 * joyButton(side) - check if a joyButton is pressed
 * button(dir) - check if a normal button is pressed
 * dpad(dir) - check if a dpad button is pressed
 * bumper(side) - check if a bumper is pressed
 *
 * joyButtonClick(side) - check if a joyButton has been clicked
 * buttonClick(dir) - check if a normal button has been clicked
 * dpadClick(dir) - check if a dpad button has been clicked
 * bumperClick(side) - check if a bumpre has been clicked
 *
 */
 
 /*TODO:
  - Both xbees are 200kbps (=200,000 baud). Can definitely bump serial baud to at least 38,400.
 */
 
#include "Controller.h"

#define BAUDRATE 9600
#define CONNECTION_TIMEOUT 1000
#define PACKET_TIMEOUT 2  //max amount of time a byte should ever take to send

//Data header bitmasks
#define LEFT_JOY_X    0b00000001
#define LEFT_JOY_Y    0b00000001
#define RIGHT_JOY_X   0b00000010
#define RIGHT_JOY_Y   0b00000010
#define LEFT_TRIGGER  0b00000100
#define RIGHT_TRIGGER 0b00001000
#define LEFT_BUTTONS  0b00010000
#define RIGHT_BUTTONS 0b00100000

//masks to use with the header to figure out what data is coming
const uint8_t headerMasks[] = {
  LEFT_JOY_X,
  LEFT_JOY_Y,
  RIGHT_JOY_X,
  RIGHT_JOY_Y,
  LEFT_TRIGGER,
  RIGHT_TRIGGER,
  LEFT_BUTTONS,
  RIGHT_BUTTONS
};

//Define offsets for buttons (the rest are in enums).
#define JOY_BUTTON   4
#define BUMPER       5

/**
 * Constructor for the class.
*/
Controller::Controller(HardwareSerial &xbeeSerial) : xbeeSerial(xbeeSerial) {
  for (int i = 0; i < 8; i++) {
    dataTargets[i] = 0;  
  }

  //We haven't received anything yet, so make difference greater than the limit
  lastReceive = -CONNECTION_TIMEOUT;

}

/**
 * Initialize communications.
*/
void Controller::init() {
    xbeeSerial.begin(BAUDRATE);
}


/**
 * @brief Get the value for the given joystick and axis. Will give a value in 
 * the range -128 to 127.
 * 
 * @param side side of the joystick. [LEFT or RIGHT].
 * @param axis axis to return. [X or Y].
 * @return the joystick value on the axis. 
 */
int8_t Controller::joystick(Dir side, Axis axis) {
    return joy[side][axis] - 128;
}

/**
 * @brief Get the state of the joystick button.
 * 
 * @param side side of the joystick button.
 * @return true if pressed, false otherwise.
 */
bool Controller::joyButton(Dir side) {
    return getButtonState(side, JOY_BUTTON);
}


/**
* Get the value of one of the colored buttons (right side of the controller).
*
* @param dir - Direction of the button. (UP, DOWN, LEFT, RIGHT).
* @return true if pressed, false otherwise.
*/
bool Controller::button(Dir dir) {
    return getButtonState(Dir::RIGHT, dir);
}

/**
* Get the value of one of the dpad buttons (or the left side colored buttons on older controler model).
*
* @param dir - Direction of the Dpad button. (UP, DOWN, LEFT, RIGHT).
* @return true if pressed, false otherwise.
*/
bool Controller::dpad(Dir dir) {
    return getButtonState(Dir::LEFT, dir);
}

/**
* Get the value of a bumper.
*
* @param side - Side of the bumper. (LEFT or RIGHT).
* @return true if pressed, false otherwise.
*/
bool Controller::bumper(Dir side) {
    return getButtonState(side, BUMPER);
}

/**
* Get the value of a trigger. Will be value in the range 0 to 255.
*
* @param side - Side of the bumper. (LEFT or RIGHT).
* @return Value of the trigger.
*/
uint8_t Controller::trigger(Dir side) {
    return triggers[side];
}

/**
* Get the value of a button.
*
* @param side - Side of the button. (LEFT or RIGHT).
* @param button - Index of the button being pressed. (LEFT, RIGHT, UP, DOWN, JOY_BUTTON, BUMPER).
* @return true if pressed, false otherwise.
*/
bool Controller::getButtonState(Dir side, uint8_t button) {
    return buttons[side] & (1 << button);
}


/**
 * @brief Check if we have received data recently
 * 
 * @return true if we have received data recently, false otherwise.
 */
bool Controller::connected() {
    //try to receive any available data
    receiveData();

    //check if we have timed out
    return (millis() - lastReceive) < CONNECTION_TIMEOUT;
}

//Don't mind us. We are for debugging.
void printBinary(uint8_t val) {
  for (int i = 7; i >= 0; i--) {
    Serial.print((val >> i) & 1);
  }
}
void printBinary(int val) {
  for (int i = 7; i >= 0; i--) {
    Serial.print((val >> i) & 1);
  }
}

/**
* Receive data from the controller. This function should be called using the 
* serialEvent() function. 
* 
* How this works:
*  - if last transmission is complete or has timed out, read the first byte as 
*     the data header and use it to construct an array of targets for the upcoming data.
*  - Each time we get a new byte of data, save based on the target for that byte.
*  - Once we reach the end (curByte == numBytes), the transmission is done.
*  - Every time we receive any data, update the last receive time.
*/
void Controller::receiveData() {
    uint8_t dataHeader = 0;  //header for the packet of send data

    if (xbeeSerial.available()) {
        //reset if last send timed out
        if ((millis() - lastReceive) > PACKET_TIMEOUT) {
            curByte = 0;
            numBytes = 0;
        }
        
        //read all of the available data
        while (xbeeSerial.available()) {
            //check if we are starting a new transmission
            if (curByte == numBytes) {
                //read the header and set up the data targets
                dataHeader = xbeeSerial.read();
                
                //iterate through the header masks and build the targets list
                curByte = 0;
                for (int i = 0; i < 8; i++) {
                  //add the target to our list if we have a match
                  if (dataHeader & headerMasks[i]) {
                      dataTargets[curByte] = i;
                      curByte++;
                  }
                }

                //Get ready to receive the data
                numBytes = curByte;
                curByte = 0;
                
            } else {
                //read the data into the target for the current byte
                switch(dataTargets[curByte]) {
                  case 0:
                    joy[LEFT][X] = xbeeSerial.read();
                    break;
                  case 1:
                    joy[LEFT][Y] = xbeeSerial.read();
                    break;
                  case 2:
                    joy[RIGHT][X] = xbeeSerial.read();
                    break;
                  case 3:
                    joy[RIGHT][Y] = xbeeSerial.read();
                    break;
                  case 4:
                    triggers[LEFT] = xbeeSerial.read();
                    break;
                  case 5:
                    triggers[RIGHT] = xbeeSerial.read();
                    break;
                  case 6:
                    updateButtons(LEFT, xbeeSerial.read());
                    break;
                  case 7:
                    updateButtons(RIGHT, xbeeSerial.read());
                    break;
                }

                //go to the next byte
                curByte++;
            }
        }
        
        //update the time of last receiving data
        lastReceive = millis();
    }
}

/**
* Update the button values. Set the button's click bit if we hit a rising edge.
*
* @param side - Side of the button. (LEFT or RIGHT).
* @param newVal - New set of values for the buttons.
*/
void Controller::updateButtons(Dir side, uint8_t newVal) {
    buttonClicks[side] = newVal & ~buttons[side];  //set click to 1 if button went from 0 to 1
    buttons[side] = newVal;
}

/**
 * @brief Check if the joystick button has been clicked.
 * 
 * @param side side of the joystick button.
 * @return true if clicked, false otherwise.
 */
bool Controller::joyButtonClick(Dir side) {
    return getButtonClick(side, JOY_BUTTON);
}


/**
* Check if a colored button (right side of the controller) has been clicked.
*
* @param dir - Direction of the button. (UP, DOWN, LEFT, RIGHT).
* @return true if clicked, false otherwise.
*/
bool Controller::buttonClick(Dir dir) {
    return getButtonClick(Dir::RIGHT, dir);
}

/**
* Check if a dpad button (or the left side colored buttons on older controler model) has been clicked.
*
* @param dir - Direction of the Dpad button. (UP, DOWN, LEFT, RIGHT).
* @return true if clicked, false otherwise.
*/
bool Controller::dpadClick(Dir dir) {
    return getButtonClick(Dir::LEFT, dir);
}


/**
* Check if the bumper has been clicked.
*
* @param side - Side of the bumper. (LEFT or RIGHT).
* @return true if clicked, false otherwise.
*/
bool Controller::bumperClick(Dir side) {
    return getButtonClick(side, BUMPER);
}

/**
* Check if a button has been clicked (pressed down). If button is pressed, this 
* will return true and then reset to false.
*
* @param side - Side of the button. (LEFT or RIGHT).
* @param button - Index of the button. (LEFT, RIGHT, UP, DOWN, JOY_BUTTON, BUMPER).
* @return true if clicked, false otherwise.
*/
bool Controller::getButtonClick(Dir side, uint8_t button) {
    //check if clicked
    if (buttonClicks[side] & (1 << button)) {
      //reset
      buttonClicks[side] &= ~(1 << button);
      return true;
    }
    
    return false;
}
