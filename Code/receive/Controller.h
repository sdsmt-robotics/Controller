/* 
 * Header for the xbee controller class.
 */

#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "Arduino.h"

enum Dir { LEFT, RIGHT, UP, DOWN };
enum Axis { X, Y };

class Controller {
public:
    Controller(HardwareSerial &xbeeSerial);
    
    void init();
    bool connected();
    
    int8_t joystick(Dir side, Axis axis);
    uint8_t trigger(Dir side);
    
    bool joyButton(Dir side);
    bool button(Dir dir);
    bool dpad(Dir dir);
    bool bumper(Dir side);
    
    bool joyButtonClick(Dir side);
    bool buttonClick(Dir dir);
    bool dpadClick(Dir dir);
    bool bumperClick(Dir side);
    
    void receiveData();  //read data from the serial stream
  
private:
    bool getButtonState(Dir side, uint8_t button);
    bool getButtonClick(Dir side, uint8_t button);
    void updateButtons(Dir side, uint8_t newVal);
    
    //controller data
    uint8_t joy[2][2];
    uint8_t triggers[2];
    uint8_t buttons[2];
    uint8_t buttonClicks[2];  //used for reading press events
    
    //serial
    HardwareSerial &xbeeSerial;

    //variables for receiving data
    uint8_t dataTargets[8];     //targets for the incoming data
    int8_t curByte = 0;         //current byte in the transmission
    int8_t numBytes = 0;         //number of bytes in the transmission
    uint32_t lastReceive = 0;    //track when the last transmission was received
};


#endif
