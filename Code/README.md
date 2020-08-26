# Controller Code  
This is all of the code for the controllers. All of the controllers run using the same communication protocol. This allows any of the controllers to be used interchangeably to control a robot by simply swapping out the XBee. A summary of the folders in this directory is as follows.
  
Folders with general classes:  
 - **receive** - class for receiving communications from the controller.   
 - **send** - class for the sending communications from the controller.   

Folders with controller code:
 - **rev1** - code to run on rev1 of the board.   
 - **rev2** - code to run on rev2 of the board.   
 - **rev3** - code to run on rev3 of the board.   
  
Folders with example code and documentation:
 - **sketch_dec04a** - documentation and example code for rev3.   

# Communication Protocol  
Each transmission is composed of the header and the data.  The header is one byte long and specifies the data that follows. Not all of the controller data is sent with each transmission. The bits in the header specify what data is sent.  

The transmission bytes are as follows:  
|   0    |   1   |   2   |   3   |   4   |    5     |     6     |    7    |    8     |
|--------|-------|-------|-------|-------|----------|-----------|---------|----------|
| header | joyLx | joyLy | joyRx | joyRy | trigleft | trigright | butleft | butright |
  
The header bits are as follows:  
|  |  |
|--|--|
| 0 | Joy Left      |
| 1 | Joy Right     |
| 2 | Trig Left     |
| 3 | Trig Right    |
| 4 | Buttons Left  |
| 5 | Buttons Right |  
  
The buttons are split into left and right. The buttons for a side are indicated by bits as follows:
|  |  |
|--|--|
| 0 | Left Button  |
| 1 | Right Button |
| 2 | Up Button    |
| 3 | Down Button  |
| 4 | Joy Button   |
| 5 | Bumper       |


The sending device is strategic about what it will send and when it will send it. It will do a full send of all data at a fixed interval to keep the connection active and to gaurd against values being missed. Between the full sends, it will send only values that update. There are also minimum intervals defined for sending analog and digital values. The exact logic is as follows:
- *time since last full send > max interval:* send all data
- *non-analog value changed and time > min interval:* send changed values
- *any value changed and time > analog interval:* send changed values
- *else:* wait for more time to pass



# Sending Code  
The main purpose of this class is to handle the transmission of data from the controller. The reading of buttons and joysticks is mostly dependent on the version of controller, so it is largely ommitted from this class (although, there some helpful bits to make life easier). General usage is as follows.  

**Setup**  
Pass in the serial strea to use for communications.

    Controller controller(Serial);  

Initialize the class in the Arduino init() function.

    init() {
	    controller.init();
    }

Call the update function in the Arduino loop() or wherever values are changed. This will prompt a send.

    loop() {
	    controller.update();
    }


**Updating Values**  
The rest of the functions are used for updating values. Most of these functions use a Dir or a Axis to specify to which button/joystick/trigger we are referring. These are enum values.  Options are as follows:  

 - **Dir:** LEFT, RIGHT, UP, DOWN
 - **Axis:** X, Y

Whenever an update function receives a new value, it compares it to the old value. If the new value is different, it will mark the value as needing to be sent in the next transmission.

The following two functions are for analog values. They expect a value in the range 0.0 to 1.0.  

    void setJoystick(Dir side, Axis axis, float value);
    void setTrigger(Dir side, float value);

The rest of these functions are for digital values and expect a true/false.

    void setJoyButton(Dir side, bool pressed);
    void setButton(Dir dir, bool pressed);
    void setDpad(Dir dir, bool pressed);
    void setBumper(Dir side, bool pressed);



**Other Notes**  
There is a constant defined in the .cpp file called BAUDRATE which controls the baudrate. Bumping this up may improve performance.  

There is another constant defined in the .cpp file called DEBUG_MODE. Uncommenting this will print the output in human-readable form instead of binary.   

# Receiving Code  

**Setup**  
In the constructor, pass in the serial stream to user. Any of the hardware serial streams will do.

    Controller controller(Serial);  

Initialize the class in the Arduino init() function. Can also wait for the connection if desired.

    init() {
	    controller.init();
		while (!controller.connected()) { delay(10); }
    }

The class must be told when to try to read incoming data. This can be placed in the Arduino serialEvent() function since it gets called whenever a serial value is available. (Not really. See note below.)

	void serialEvent() {
		controller.receiveData();
	}

This class also contains a helper function to set the deadzone for the joysticks. Values with a magnitude smaller than this will be returned as 0.0.

	controller.setJoyDeadzone(floatVal);



**Checking if Connected**  
The controller connection will time out if nothing is received or over 1 second. This function will check the connection status.

    bool connected();

**Joystick Vals**  
This function will return the curent value for a joystick along a particular axis in the range -128 to 128. 

    int8_t joystick(Dir side, Axis axis);

**Trigger Vals**  
This function will return the current value of the trigger in the range 0 to 255.

    uint8_t trigger(Dir side);

**Button Vals**  
The button, Dpad, and bumper function all work similarly. They return true or false depending on whether the button is pressed.  

    bool joyButton(Dir side);
    bool button(Dir dir);
    bool dpad(Dir dir);
    bool bumper(Dir side);
    
There are also functions for getting button clicks. These are useful in instances where we only care about press events. The click functions will return true *once* when the button is pressed and false until the button is pressed again.

    bool joyButtonClick(Dir side);
    bool buttonClick(Dir dir);
    bool dpadClick(Dir dir);
    bool bumperClick(Dir side);
    

**Other Notes**  
*On handling incoming serial data:*  
It seems natural to put the incoming data handler in the Arduino  serialEvent() function since it supposedly gets called whenever serial data is available. But guess what: IT DOESN'T! It only gets called *at the end of an Arduino loop()* if serial data is available. Since a new transmission can be sent once per 20ms, if loop() takes longer than this you will loose data!  
  
*On serial buffer size:*  
A full transmission is 9 bytes. By default, the Arduino serial buffer is 8 bytes. In limited testing, this never caused an issue. When the controller does a full send, it actually breaks it into left and right halves. A true full send will likely never happen. If this should become an issue, the serial buffer can be increased by editing the Arduino source files.  

# Version Specific Notes
**Rev 1**  
Nothing perticular to note here. The controller does not have triggers, bumpers, or button connections to the joysticks. It also does not have a dpad, so those functions refer to the left set of butttons.

**Rev 2**  
This controller splits buttons into three sets and reads analog values from a set to determine which button is pressed. The buttons are wired such that each button will have a distinct analog value associated with it when pressed. Each set of buttons is also connected to an interrupt which will trigger when any of the buttons are pressed.

Also of note: the triggers on this controller are actually buttons. They are only ever on or off. If they are pressed, the controller will send the max value for the trigger.  

**Rev 3**  
This controller does some unique stuff for reading button presses. The buttons are read as a grid. Four pins (R1, R2, R3, R4) send a pulse to a set of buttons. Then three input pins (C1, C2, C3) read the values from these buttons. This reduces the number of pins required to read the 12 buttons. The grid pattern is as shown below:

|        |      R1      |     R2      |     R3      |    R4     |
|--------|--------------|-------------|-------------|-----------|
| **C1** | Button Right | Button Left | Button Down | Button Up |
| **C2** | Dpad Down    | Dpad Right  | Dpad Left   | Dpad Up   |
| **C3** | Right Bump   | Left Bump   | Left Joy    | Right Joy |

This does have the side effct however, that when sending a pulse to the set of buttons it takes time for things to settle down. Because of this, there is a delay of 10ms in the code. Removing or lowering the delay can cause mis-reads, no-reads, and/or possible heart failure.
