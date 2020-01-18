void setup() {
  // put your setup code here, to run once:
  DDRD |= (1<<PD4)|(1<<PD7); // Set pins as output
  DDRB |= (1<<PB0)|(1<<PB1)|(PB2); // Set pins as output

  Serial.begin(9600);

}

void loop() {
  // put your main code here, to run repeatedly

  PORTD |= (1<<PD4); // Turn on LED
  while(1) // Redundant loop to by-pass Arduino overhead and increase runtime
  {

    uint16_t message_frame[12]={};
    char row_flag = 0x00; //Setup flags for what row is currently being toggled
    uint16_t col_flag; // and what col button might be pressed
    col_flag = 0x01;
    setframe(message_frame,row_flag++,col_flag);
    
   // "x" in the following comments is the effective Arduino pin that's mapped to appropriate pin 
   // on the chip. Lookup Arduino pinout : https://www.circuito.io/blog/arduino-uno-pinout/
   // Notice in the picture how pin 6 on the chip is actually digital pin 4 on the arduino and is BIT
   // 4 in the D register (PD4)
   
    PORTB |= (1<<PB0); // digitalWrite(x,HIGH) -> Use PORTX when writing to registers, PINX when reading and DDRX when setting the pinMode
    col_flag = CheckKeyPress(); // Check for key presses
    PORTB &= ~(1<<PB0); // digitalWrite(x,LOW)
    setframe(message_frame,row_flag++,col_flag);

    
    PORTB |= (1<<PB1);
    col_flag = CheckKeyPress();
    PORTB &= ~(1<<PB1);
    setframe(message_frame,row_flag++,col_flag);

    PORTB |= (1<<PB2);
    col_flag = CheckKeyPress();
    PORTB &= ~(1<<PB2);
    setframe(message_frame,row_flag++,col_flag);


    PORTD |= (1<<PD7);
    col_flag = CheckKeyPress();
    PORTB &= ~(1<<PD7);  
    setframe(message_frame,row_flag++,col_flag);

    
    col_flag = analogRead(A0);
    setframe(message_frame,row_flag++,col_flag);


    col_flag = analogRead(A1);
    setframe(message_frame,row_flag++,col_flag);

    col_flag = analogRead(A2);
    setframe(message_frame,row_flag++,col_flag);

    col_flag = analogRead(A3);
    setframe(message_frame,row_flag++,col_flag);

    col_flag = analogRead(A4);
    setframe(message_frame,row_flag++,col_flag);


    col_flag = analogRead(A5);
    setframe(message_frame,row_flag++,col_flag);

    col_flag = 0x04;
    setframe(message_frame,row_flag,col_flag);

    sendframe(message_frame);
    Serial.write(0x0D);
    delay(10);
    Serial.write(0x0A);
  }

  
}

char CheckKeyPress(){
  char dreg = PIND; // Read the whole input register -> registers are 8 bit. No point in using int (32 bits). Can also use uint_8t
  if(dreg & (1<<PD2)) // If the PD2 bit in the register is set, return 1
    return 0x0A;
  if(dreg & (1<<PD3))
    return 0x0B;
  if(dreg & (1<<PD5))
    return 0x0C;

  return 0x00;
} 

void setframe(uint16_t frame[], char index, uint16_t &col_flag)
{
    frame[index] = (col_flag);

    col_flag = 0x00; 
    delay(100);
}

void sendframe(uint16_t message_frame[])
{
  char i=0;
  do
  {
    Serial.write(message_frame[i]);
    delay(10);                            //UART is quirky. Can't be sending stuff too fast.
    Serial.write(message_frame[i]>>8); 
    delay(10);

  }
  while(message_frame[i++] != 0x04);

}
