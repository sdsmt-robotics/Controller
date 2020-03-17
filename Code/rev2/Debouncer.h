/* 
 * Header for the button debouncer class.
 */

#ifndef DEBOUNCER_H
#define DEBOUNCER_H

#include "Arduino.h"


class Debouncer {
public:
    Debouncer(unsigned thresholds[], uint8_t numButtons, uint8_t inputPin);
    
    int8_t getPressed();
    
    //signify that no button is pressed
    const int NONE = -1;
  
private:
    int8_t readState();    
    

    uint8_t inputPin;
    uint8_t numButtons;
    unsigned* thresholds; //last steady read value 
    int8_t lastSteadyState = NONE; //last steady read value 
    int8_t lastState = NONE; //last read value
    unsigned long int lastStateChange = 0;  //last time the read value changed
};

#endif
