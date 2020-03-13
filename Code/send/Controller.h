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
    
    void setJoystick(Dir side, Axis axis, float value);
    void setJoyButton(Dir side, bool pressed);
    void setButton(Dir dir, bool pressed);
    void setDpad(Dir dir, bool pressed);
    void setBumper(Dir side, bool pressed);
    void setTrigger(Dir side, float value);
    
    void update();
  
private:
    void updateButtonState(Dir side, uint8_t button, bool pressed);
    
    void send();
    void fullSend();
    
    //controller data
    float joy[2][2];
    float triggers[2];
    uint8_t buttons[2];
    
    uint8_t dataHeader = 0;  //header for the packet of send data
    
    //serial
    HardwareSerial &xbeeSerial;
    uint32_t lastSend = 0;
    uint32_t lastFullSend = 0;
};

#endif
