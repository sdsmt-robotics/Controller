/* 
 * Class managing communications from the controller.
 * 
 * Any time a value is changed, a bit is set in the transmission header. When the update() 
 * function is called, we check if we should send. Sending logic:
 *   - time since full send > max interval: send all data
 *   - non-analog value changed and time > min interval: send data indicated by dataHeader
 *   - any value changed and time > analog interval: send data indicated by dataHeader
 *   - else: nope
 *   
 * The transmission is as follows:
 * +--------+-------+-------+-------+-------+----------+-----------+---------+----------+
 * |   0    |   1   |   2   |   3   |   4   |    5     |     6     |    7    |    8     |
 * +--------+-------+-------+-------+-------+----------+-----------+---------+----------+
 * | header | joyLx | joyLy | joyRx | joyRy | trigleft | trigright | butleft | butright |
 * +--------+-------+-------+-------+-------+----------+-----------+---------+----------+
 * 
 * Bits in the header indicate which bytes will be sent.
 * +---+---------------+
 * | 0 | Joy Left      |
 * | 1 | Joy Right     |
 * | 2 | Trig Left     |
 * | 3 | Trig Right    |
 * | 4 | Buttons Left  |
 * | 5 | Buttons Right |
 * | 6 | -             |
 * | 7 | -             |
 * +---+---------------+
 * 
 * The buttons are split into left and right. The buttons for a side are indicated by bits as follows:
 * +---+--------------+
 * | 0 | Left Button  |
 * | 1 | Right Button |
 * | 2 | Up Button    |
 * | 3 | Down Button  |
 * | 4 | Joy Button   |
 * | 5 | Bumper       |
 * | 6 | -            |
 * | 7 | -            |
 * +---+--------------+
 */
 /*TODO:
  - Both xbees are 200kbps (=200,000 baud). Can definitely bump serial baud to at least 38,400.
 */
#include "Controller.h"

//Uncomment this line to transmit as human readable stuff instead of in binary
//#define DEBUG_MODE

#define BAUDRATE 115200

//sending intervals in ms
#define MIN_INTERVAL 20
#define ANALOG_INTERVAL 50
#define MAX_INTERVAL 800

//Define bit offsets for the header. Left/right are specified using the Dir enum.
const uint8_t JOY        = 1 << 0;
const uint8_t TRIGGER    = 1 << 2;
const uint8_t BUTTONS    = 1 << 4;
const uint8_t NON_ANALOG = BUTTONS | (BUTTONS << 1);
const uint8_t ALL        = 0b00111111;
const uint8_t LEFT_HALF    = JOY | BUTTONS | TRIGGER;
const uint8_t RIGHT_HALF = (JOY << 1) | (BUTTONS << 1) | (TRIGGER << 1);


//Define offsets for buttons (the rest are in enums).
const uint8_t JOY_BUTTON  = 4;
const uint8_t BUMPER      = 5;

/** Constructor for the class.
*/
Controller::Controller(HardwareSerial &xbeeSerial) : xbeeSerial(xbeeSerial) { }

/** Initialize communications.
*/
void Controller::init() {
    xbeeSerial.begin(BAUDRATE);
}

/**
* Set the joystick value for the given side and axis.
*
* @param side - Joystick side. (LEFT or RIGHT).
* @param axis - Axis for the value. (X or Y).
* @param value - Value for the axis. (0.0 to 1.0).
*/
void Controller::setJoystick(Dir side, Axis axis, float value) {
    //Update the value and send the data if it is updated
    if (abs(joy[side][axis] - value) > 0.003) {
        joy[side][axis] = value;
        
        //Update the header to specify this item should send
        dataHeader |= (JOY << side);
    }
    
}

/**
* Set the joystick button value.
*
* @param side - Joystick side. (LEFT or RIGHT).
* @param isPressed - Set the state of the button. (true or false).
*/
void Controller::setJoyButton(Dir side, bool isPressed) {
    updateButtonState(side, JOY_BUTTON, isPressed);
}

/**
* Set the value of one of the colored buttons (right side of the controller).
*
* @param dir - Direction of the button. (UP, DOWN, LEFT, RIGHT).
* @param isPressed - Set the state of the button. (true or false).
*/
void Controller::setButton(Dir dir, bool isPressed) {
    updateButtonState(Dir::RIGHT, dir, isPressed);
}

/**
* Set the value of one of the dpad buttons (or the left side colored buttons on older controler model).
*
* @param dir - Direction of the button. (UP, DOWN, LEFT, RIGHT).
* @param isPressed - Set the state of the button. (true or false).
*/
void Controller::setDpad(Dir dir, bool isPressed) {
    updateButtonState(Dir::LEFT, dir, isPressed);
}

/**
* Set the value of a bumper.
*
* @param side - Side of the bumper. (LEFT or RIGHT).
* @param isPressed - Set the state of the button. (true or false).
*/
void Controller::setBumper(Dir side, bool isPressed) {
    updateButtonState(side, BUMPER, isPressed);
}

/**
* Set the value of a trigger.
*
* @param side - Side of the bumper. (LEFT or RIGHT).
* @param value - Set the value of the trigger. (0.0 to 1.0).
*/
void Controller::setTrigger(Dir side, float value) {
    //Update if new value
    if (abs(triggers[side] - value) > 0.003) {
        triggers[side] = value;
        
        //Update the header to specify this item should send
        dataHeader |= (TRIGGER << side);
    }
}

/**
* Set the value of a button.
*
* @param side - Side of the button. (LEFT or RIGHT).
* @param button - Index of the button being pressed. (LEFT, RIGHT, UP, DOWN, JOY_BUTTON, BUMPER).
* @param isPressed - Set the state of the button. (true or false).
*/
void Controller::updateButtonState(Dir side, uint8_t button, bool isPressed) {
    //get the current button state
    bool buttonState = (buttons[side] >> button) & 1; 
    
    //Check if we need to update anything
    if (buttonState != isPressed) {
        //toggle the button's bit
        //clear bit
        buttons[side] &= ~(1 << button);

        //set bit
        buttons[side] |= (isPressed << button);
        
        //Update the header to specify this item should send
        dataHeader |= (BUTTONS << side);
    }
}

//For debugging
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

/** Send the updated values.
* 
* Look at changed values and decide what to send. Logic:
*   - time since full send > max interval: send all data
*   - non-analog value changed and time > min interval: send data indicated by dataHeader
*   - any value changed and time > analog interval: send data indicated by dataHeader
*   - else: nope
*/
void Controller::update() {
    int timeDiff = millis() - lastSend;

    //only send if past the minimum send interval
    if (timeDiff > MIN_INTERVAL) {
      //send all values once every certain interval
      if (millis() - lastFullSend > MAX_INTERVAL) {
        fullSend();
      //send button press immediately or analog change once every interval
      } else if ((dataHeader & NON_ANALOG) || (dataHeader && timeDiff > ANALOG_INTERVAL)) {
        send();
      }
    }
}

/** Send all the data
*/
void Controller::fullSend() {
    //Update the header to specify that left half should send
    dataHeader |= LEFT_HALF;
    
    //Send the data
    send();
    lastFullSend = millis();

    //update the header to specify right half should send.
    //it will send as soon as the min interval has passed.
    dataHeader |= RIGHT_HALF;
}

/**
* Send data based off of bits in the data header.
* 
* Packet consists of header followed by the data. The bits in the header 
* describe which values will be sent.
*/
void Controller::send() {

#ifndef DEBUG_MODE  //transmit normally
    //send the header
    xbeeSerial.write((uint8_t*)&dataHeader, 1);
    
    //Decide what data to send and send it
    //left joystick
    if (dataHeader & (JOY << LEFT)) {
        xbeeSerial.write((uint8_t)((joy[LEFT][X] + 1.0) * 127.5));
        xbeeSerial.write((uint8_t)((joy[LEFT][Y] + 1.0) * 127.5));
    }
    
    //right joysticks
    if (dataHeader & (JOY << RIGHT)) {
        xbeeSerial.write((uint8_t)((joy[RIGHT][X] + 1.0) * 127.5));
        xbeeSerial.write((uint8_t)((joy[RIGHT][Y] + 1.0) * 127.5));
    }
    
    //left trigger
    if (dataHeader & (TRIGGER << LEFT)) {
        xbeeSerial.write((uint8_t)(triggers[LEFT] * 255));
    }
    
    //right trigger
    if (dataHeader & (TRIGGER << RIGHT)) {
        xbeeSerial.write((uint8_t)(triggers[RIGHT] * 255));
    }
    
    //left button set
    if (dataHeader & (BUTTONS << LEFT)) {
        xbeeSerial.write((uint8_t*)&buttons[LEFT], 1);
    }
    
    //right button set
    if (dataHeader & (BUTTONS << RIGHT)) {
        xbeeSerial.write((uint8_t*)&buttons[RIGHT], 1);
    }

#else  //send in human-readable text
    //send the header
    printBinary(dataHeader);
    
    //Decide what data to send and send it
    //left joystick
    if (dataHeader & (JOY << LEFT)) {
        xbeeSerial.print(",");
        xbeeSerial.print((uint8_t)((joy[LEFT][X] + 1.0) * 127.5));
        xbeeSerial.print(",");
        xbeeSerial.print((uint8_t)((joy[LEFT][Y] + 1.0) * 127.5));
    }
    
    //right joysticks
    if (dataHeader & (JOY << RIGHT)) {
        xbeeSerial.print(",");
        xbeeSerial.print((uint8_t)((joy[RIGHT][X] + 1.0) * 127.5));
        xbeeSerial.print(",");
        xbeeSerial.print((uint8_t)((joy[RIGHT][Y] + 1.0) * 127.5));
    }
    
    //left trigger
    if (dataHeader & (TRIGGER << LEFT)) {
        xbeeSerial.print(",");
        xbeeSerial.print((uint8_t)(triggers[LEFT] * 255));
    }
    
    //right trigger
    if (dataHeader & (TRIGGER << RIGHT)) {
        xbeeSerial.print(",");
        xbeeSerial.print((uint8_t)(triggers[RIGHT] * 255));
    }
    
    //left button set
    if (dataHeader & (BUTTONS << LEFT)) {
        xbeeSerial.print(",");
        printBinary(buttons[LEFT]);
    }
    
    //right button set
    if (dataHeader & (BUTTONS << RIGHT)) {
        xbeeSerial.print(",");
        printBinary(buttons[RIGHT]);
    }
    
    xbeeSerial.println("");
#endif

    //reset sending vars
    lastSend = millis();
    dataHeader = 0;
}
