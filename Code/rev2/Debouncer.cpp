/* 
 * Button debouncer class.
 * 
 * This class is used with a button set to track changes and return the clean
 * steady state value. A set of thresholds are sent into the class. When the 
 * read function is called, the class will return the index of the threshold 
 * the current value matches.
 */

#include "Debouncer.h"

//Min time a button value must be constant
#define SETTLE_TIME 10

/**
 * @brief Constructer for the class.
 * 
 * @param thresholds - threshold values for button presses
 * @param numButtons - number of buttons in this set
 * @param inputPin - analog input pin to read for this set of buttons
 */
Debouncer::Debouncer(unsigned thresholds[], uint8_t numButtons, uint8_t inputPin)
    :thresholds(thresholds), numButtons(numButtons), inputPin(inputPin) {

}

/**
 * @brief Get which button is currently pressed.
 * 
 * @return index of the currently pressed button (-1 if none pressed).
 */
int8_t Debouncer::getPressed() {
    //get the current state
    int8_t curState = readState();

    if (curState != lastState) { //change since last read
        lastState = curState;
        lastStateChange = millis();
    //value has changed and value is "steady"
    } else if (curState != lastSteadyState && (millis() - lastStateChange) > SETTLE_TIME) {
        lastSteadyState = curState;
    }

    return lastSteadyState;
}

/**
 * @brief Cycle through threshold values to find the current state of the button set.
 * 
 * @return the current state of the button set (-1 if none pressed).
 */
int8_t Debouncer::readState() {
    //read the current value
    int curVal = analogRead(inputPin);

    //cycle through until we find a threshold we are under
    for (int i = 0; i < numButtons; i++) {
        if (curVal < thresholds[i]) {
            return i - 1;
        }
    }

    //if not under any of those, must be at the top one
    return numButtons - 1;
}    
