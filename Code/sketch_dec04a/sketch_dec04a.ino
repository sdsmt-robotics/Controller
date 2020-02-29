/* Authors: Pratik, Micah
 * 
 * About: 
 * This the current setup of how our amazing controller works.
 * Hopefully, it is documented well enough that future users 
 * can clearly understand what we did and how the can change it.
 * If you are not an adavnce embedded engineering student then I 
 * would not recommend touching anything. Is our code perfect? Nope, 
 * just as everything else in life is, but hopefully it's close.
 * We are going to start with anolog pin bleongs to what sensors and
 * then which button is located within each register, since that is 
 * what the orignal programmer decided to use for a slight speed up.
 * 
 * Pins to ---> physical devices
 * ----------------------------------------------------------------------------------------
 * Anolog pins: 
 * A0 --> Right Trigger 
 * A1 --> Left Trigger 
 * A2 --> Right Y-Axis Joystick Value 
 * A3 --> Left Y-Axis Joystick Value
 * A4 --> Right X-Axis Joystick Value 
 * A5 --> Left X-Axis Joystick Value
 * 
 * Digital Pin Buffer: 
 * IDE Pin 2 - Reads values from the right side buttons
 * IDE Pin 3 - Reads values from the left side buttons
 * IDE Pin 5 - Reads values from the bumpers
 * IDE Pin 7 - Pulses right and left yellow buttons
 * IDE Pin 8 - Pulses right red button, left green button, and right bumper
 * IDE Pin 9 - Pulses right blue button, left red button, and left bumper
 * IDE Pin 10 - Pulses right green button and left blue button
 * ----------------------------------------------------------------------------------------
 * Communication Choices: 
 * We decided to package all comunication down to two characters for button
 * presses as well as the Joystick Values
 * 
 * ----------------------------------------------------------------------------------------
 * Joystick Values are read on a 10-bit ADC, which means you have a range of 
 * 0 to 1023. Values from 0 255 are more widely accpeted values within motor
 * drivers. To accomplish this, we can left bit shift our analog value by 2 bits. 
 * This creates a very quick mapping from 1023 to 225. To help clear up any confusion
 * when receiving values, we will send another charcter that will represnt that we are 
 * receiving an analog value as we might receive the charcter representing 0, which is ' ' 
 * or 0000 0000 in binary. We have decided to use the charcaters 'L', 'l', 'R', 'r' for 
 * which joystick value is being recived. The upper case letter is for the x-axis and 
 * the lower case is for the y-axis. After the joystick signal value, comes the acutal
 * value of the read from the joystick. 
 * 
 * Example of the left-axis joystick value being sent: LA this would be 0x4c41 in hexdecimal
 * 
 * ----------------------------------------------------------------------------------------
 * Button Presses are read from digital pins and receive their voltage from a pulse by a
 * digital output pin. Button presses recieve thier pulse from the following 4 (IDE) pins:
 * Pin 7 - controls the pulse to the right and left yellow buttons
 * Pin 8 - controls the pusle to the right red button, left green button, and right bumper 
 * Pin 9 - controls the pulse to the right blue button, left red button, and left bumper
 * Pin 10 - controls pulse to the right green button and left blue button
 * NOTE: Due to the capcitors used to filter the button presses, the buttons will be read 
 * atleast twice, so adding in delays and staggering the button reads will hopefully fix the 
 * problem. 
 * To read the button presses the pins are read from the following 3 digital (IDE) pins: 
 * Pin 2 - controls the input read from the right side buttons
 * Pin 3 - controls the input read from the left side buttons
 * Pin 5 - controls the input read from the left and right bumpers
 * 
 * Example of a button vlaue being pulsed, recieved, and sent: 
 *   digitalWrite(RGLB, HIGH);
 *   if (digitalRead(Pull_Right) == HIGH)
 *   {
 *     Serial.write("?G");
 *   }
 *   digitalWrite(RGLB, LOW);
 * 
 * Example of transmitted values are: ?G or 0X3f47 in hexdecimal
 * 
 * ----------------------------------------------------------------------------------------
 * Tips: 
 * 1a). If you find that you press a button, bumper, tirgger, or move a joystick more often,
 * then call the function relating to it more often to keep it more up-to-date.
 * 1b). Another method of getting more response would be to change the fucntion calls in  
 * main as it has to check every value on the board. 
 * 2). Due to using a RC Lowpass filter for debouncing buttons, the input pins will receive
 * more than one button press from a single press. We bleive that having only the right or 
 * left side button fucntions have the intial pulse from the 
 * 
 * ----------------------------------------------------------------------------------------
 * Bugs: 
 * 1). Recieving more than one button presses. Adjusting the delay times may resolve this
 * problem. BEWARE every millisecond delayed means we are more likely to miss a value  
 * update and or increase the amount of lag proceived by the user. 
 * 
 * ----------------------------------------------------------------------------------------
 * To-Do's
 * 1). Adjust delay values until we no longer get double presses. Using the fucntion call:  
 * delayMicroseconds() may give better results from stopping multiple button presses
 * 2). Setup a way of sending standby transmit value to the robot to let it know that the 
 * contoller is still running. 
 * 3). Test Hall-effect sensors to make sure they will work with the triggers
 * 4). Move the list of fucntion calls to a header file or two.. or three
 * 
 * ----------------------------------------------------------------------------------------
 */
 
//switch name to which button colors and or side of buttons they are
const int RBLR = 9; //SW 5/7 5 = Right Blue and 7 = Left Red ->RBLR (and left bumper)
const int RRLG= 8; //SW 1/3 1 = Right Red and 3 = Left Green -> RRLG (and right bumper)
const int RGLB = 10; //SW 2/4 2 = Right Green and 4 = Left Blue -> RGLB
const int RYLY = 7; //SW 6/8 6 = Right Yellow and 8 = Left Yellow -> RYLY
const int Pull_Right = 2; //C1
const int Pull_Left = 3; //C2
const int Pull_Bumper = 5; //C3
static bool Is_Pressed = false; //used to see if a button is pressed and send a single value
void setup() {
  // put your setup code here, to run once:
  pinMode(Pull_Right, INPUT);
  pinMode(Pull_Left, INPUT);
  pinMode(Pull_Bumper, INPUT);
  pinMode(RBLR, OUTPUT);
  pinMode(RRLG, OUTPUT);
  pinMode(RGLB, OUTPUT);
  pinMode(RYLY, OUTPUT);

  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly

  PORTD |= (1<<PD4); // Turn on LED
  
  int previous_right_x_joystick = 0;
  int previous_right_y_joystick = 0;
  int previous_left_x_joystick = 0;
  int previous_left_y_joystick = 0;
  int previous_right_trigger = 0;
  int previous_left_trigger = 0;
  
  while(1) // Redundant loop to by-pass Arduino overhead and increase runtime
  { 
    
    //these two buttons are tied together when pulsed
    right_green_button();
    left_blue_button();
    delay(12);

    right_stick(previous_right_x_joystick, previous_right_y_joystick);
    left_stick(previous_left_x_joystick, previous_left_y_joystick);
    
    //these three are tied together when pulsed
    right_blue_button();
    left_red_button();
    left_bumper();
    delay(12);

    right_stick(previous_right_x_joystick, previous_right_y_joystick);
    left_stick(previous_left_x_joystick, previous_left_y_joystick);
    
    // These three are tied together
    right_red_button(); 
    left_green_button();
    right_bumper();
    delay(12);
    
    right_stick(previous_right_x_joystick, previous_right_y_joystick);
    left_stick(previous_left_x_joystick, previous_left_y_joystick);
    
    //tied together when pulsed
    right_yellow_button();
    left_yellow_button();
    delay(12);
    
    right_stick(previous_right_x_joystick, previous_right_y_joystick);
    left_stick(previous_left_x_joystick, previous_left_y_joystick);
    delay(30);
  }
}

void right_green_button()
{
  //checking if the right side green button was pressed
    digitalWrite(RGLB, HIGH);
    delay(2);
    if (digitalRead(Pull_Right) == HIGH)
    {
      Is_Pressed = true; 
    }
    digitalWrite(RGLB, LOW);
    if (Is_Pressed == true)
    {
      Serial.write("?G");
    }
    Is_Pressed = false;
}

void right_blue_button()
{
  //checking if the right side blue button was pressed
    digitalWrite(RBLR, HIGH);
    delay(2);
    if (digitalRead(Pull_Right) == HIGH)
    {
      Is_Pressed = true;
    }
    digitalWrite(RBLR, LOW);
    
    if (Is_Pressed == HIGH)
    {
      Serial.write("?B");
    }
    Is_Pressed = false;
}

void right_red_button()
{
    //checking if the right side red button was pressed
    digitalWrite(RRLG, HIGH);
    delay(2);
    if (digitalRead(Pull_Right) == HIGH )
    {
      Is_Pressed = true;
    }
    digitalWrite(RRLG, LOW);
    if (Is_Pressed == HIGH )
    {
      //Serial.write(" Red Pressed "); //testing only
      Serial.write("?R");
    }
    Is_Pressed = false; 
}

void right_yellow_button()
{
    //checking if the right side yellow button was pressed
    digitalWrite(RYLY, HIGH);
    delay(2);
    if (digitalRead(Pull_Right) == HIGH)
    {
      Is_Pressed = true;
    }
    if (Is_Pressed == HIGH)
    {
      Serial.write("?Y");
    }
    digitalWrite(RYLY, LOW);
    Is_Pressed = false;
}

void left_green_button()
{
    //checking if the left side green button is pressed
    //digitalWrite(RRLG, HIGH);
    if(digitalRead(Pull_Left) == HIGH)
    {
      Is_Pressed = true;
    }
    if(Is_Pressed == HIGH)
    {
      Serial.write("?g");
    }
    //digitalWrite(RRLG, LOW);
    Is_Pressed = false;
}

void left_red_button()
{
    //checking if the left side Red button is pressed
    //digitalWrite(RBLR, HIGH);
    if (digitalRead(Pull_Left) == HIGH)
    {
      Is_Pressed = true;    
    }
    if (Is_Pressed == HIGH)
    {
      Serial.write("?r");    
    }
    //digitalWrite(RRLG, LOW);
    Is_Pressed = false;
}

void left_blue_button()
{
    //checking if the left side blue button was pressed
    //digitalWrite(RGLB, HIGH);
    if (digitalRead(Pull_Left) == HIGH)
    {
      Is_Pressed = true;
    }
    if (Is_Pressed == HIGH)
    {
      Serial.write("?b");
    }
    //digitalWrite(RGLB, LOW);
    Is_Pressed = false;
}

void left_yellow_button()
{
    //checking if the right side yellow button was pressed
    //digitalWrite(RYLY, HIGH);
    if (digitalRead(Pull_Left) == HIGH)
    {
      Is_Pressed = true;
    }
    if (Is_Pressed == HIGH)
    {
      Serial.write("?y");
    }
    //digitalWrite(RYLY, LOW);
    Is_Pressed = false;
}

void right_bumper()
{
      //digitalWrite(RRLG, HIGH);
      if (digitalRead(Pull_Bumper) == HIGH)
      {
        Is_Pressed = true;
      }
      if (Is_Pressed == HIGH)
      {
        Serial.write("?r");
      }
      //digitalWrite(RRLG, LOW);
      Is_Pressed = false;
}

void left_bumper()
{
  //digitalWrite(RBLR, HIGH);
  if (digitalRead(Pull_Bumper) == HIGH)
  {
    Is_Pressed = true;
  }
  if (Is_Pressed == HIGH)
  {
    Serial.write("?l");
  }
  //digitalWrite(RBLR, LOW);
  Is_Pressed = false;
}

//Cehcks right joystick and sends the value
void right_stick(int &old_right_x_axis, int &old_right_y_axis)
{
    int current_right_x_axis = analogRead(A4) >> 2;
    
    //change the compare value for a more responsive joystick 
    if( abs(old_right_x_axis - current_right_x_axis) > 2)
    {
        old_right_x_axis = current_right_x_axis;
        
        //Right_trigger needs to be updated to bot
        Serial.write('R');
        Serial.write(old_right_x_axis);
    }

    int current_right_y_axis = analogRead(A2) >> 2;
    
    //change the compare value for a more responsive joystick
    if( abs(old_right_y_axis - current_right_y_axis) > 2)
    {
        old_right_y_axis = current_right_y_axis;
        //Left_trigger
        Serial.write('r');
        Serial.write(old_right_y_axis);
    }
}

//Checks left joystick change, if a great enough change has occured update value
void left_stick(int &old_left_x_axis, int &old_left_y_axis)
{
    int current_left_x_axis = analogRead(A5) >> 2;
    
    //change the compare value for a more responsive joystick 
    if( abs(old_left_x_axis - current_left_x_axis) > 2)
    {
        old_left_x_axis = current_left_x_axis;
        
        //Right_trigger needs to be updated to bot
        Serial.write('L');
        Serial.write(old_left_x_axis);
    }


    int current_left_y_axis = analogRead(A3) >> 2;
    
    //change the compare value for a more responsive joystick
    if( abs(old_left_y_axis - current_left_y_axis) > 2)
    {
        old_left_y_axis = current_left_y_axis;
        //Left_trigger
        Serial.write('l');
        Serial.write(old_left_y_axis);
    }
    
}

//checks the trigger values, if a great enough change has occured update value
void triggers(int &old_right_value, int &old_left_value)
{
    int current_right_trigger = analogRead(A0) >> 2;
    
    //change the compare value for a more responsive joystick
    if( abs(old_right_value - current_right_trigger) > 2)
    {
        old_right_value = current_right_trigger;
        
        //Right_trigger
        Serial.write('T');
        Serial.write(old_right_value);
    }

    
    int current_left_trigger = analogRead(A1) >> 2;
    
    //change the compare value for a more responsive joystick
    if( abs(old_left_value - current_left_trigger) > 2)
    {
        old_left_value = current_left_trigger;
        //Left_trigger
        Serial.write('t');
        Serial.write(old_left_value);
    }
    
}
