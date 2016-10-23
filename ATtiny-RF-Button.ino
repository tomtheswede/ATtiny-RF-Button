/*  
 *   For sending RF signals from battery operated ATtiny85
 *   Code by Thomas Friberg (https://github.com/tomtheswede)
 *   Updated 23/10/2016
 */

#include <avr/sleep.h>

//Device parameters
const unsigned int channel = 1; //So the message can be picked up by the right receiver
const unsigned int devType = 1; //Reads as "1" corresponding with BTN type
const unsigned int sensorName1 = 4; //The number of this device - needs to be unique.

//Interrupt variables
volatile bool triggered=false;
volatile bool longPressPrimer=false;
volatile bool longerPressPrimer=false;
volatile bool longestPressPrimer=false;
volatile bool buttonState=false;
volatile const unsigned int butPin = 3; //button pin
volatile unsigned long lastTrigger=millis();

//General variables
const unsigned int sendPin = 2; //RF pin
bool sendByte[4];
long lastActionTime=millis();

void setup() {
  // put your setup code here, to run once:
  pinMode(butPin,INPUT_PULLUP);
  pinMode(sendPin,OUTPUT);
  delay(20);
  //lastTrigger=millis();

  //Power controls
  ADCSRA &= ~(1<<ADEN);                   //Disable ADC, saves ~230uA

  //Start interupts
  GIMSK |= _BV(PCIE);                     // Enable Pin Change Interrupts
  PCMSK |= _BV(PCINT3);                   // Use PB3 as interrupt pin
}

void loop() {
  //delay(3);
  if (triggered) {
    encodeMessage(1);
    triggered=false;
    longPressPrimer=true;
    longerPressPrimer=true;
    longestPressPrimer=true;
    lastActionTime=millis();
  }
  else if (buttonState && longPressPrimer && millis()-lastTrigger>700) {
    encodeMessage(2);
    longPressPrimer=false;
  }
  else if (buttonState && longerPressPrimer && millis()-lastTrigger>1900) {
    encodeMessage(3);
    longerPressPrimer=false;
  }
  else if (buttonState && longestPressPrimer && millis()-lastTrigger>4000) {
    encodeMessage(4);
    longestPressPrimer=false;
  }
  if (millis()-lastActionTime>200 && !buttonState) {
    //encodeMessage(5); //Signal to say it's sleeping
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    sleep_mode(); //Actually go to sleep here
  }
}

ISR(PCINT0_vect) {
  buttonState=!digitalRead(butPin);
  if (buttonState && !triggered && (millis()-lastActionTime>250 || millis()<lastTrigger)) { //includes 200ms debounce
    lastTrigger=millis();
    triggered=true;
  }
  else if (!buttonState && !triggered) {
    lastTrigger=millis();
  }
}

void encodeMessage(int msgType) { //message should read 15 1 6 14 or 1111 0001 0110 1110
  //regular pulses for receiver to calibrate
  for (int i=0; i <= 12; i++){
    pulse(0);
  }
  
  encodeNumber(15); //Start  1111 to initiate message
  encodeNumber(channel); //number 4
  encodeNumber(11); //comma
  encodeNumber(devType); //number 9
  encodeNumber(11); //comma
  encodeNumber(sensorName1); //number 9
  encodeNumber(11); //comma
  encodeNumber(msgType); //number 9
  encodeNumber(13);//End  1101 - WIP on parity byte
  encodeNumber(12);//End  1100 - WIP on parity byte
}

void pulse(bool logic) {
  if (logic) {
    digitalWrite(sendPin,HIGH);
    delayMicroseconds(575); 
    digitalWrite(sendPin,LOW);
    delayMicroseconds(250);
  }
  else {
    digitalWrite(sendPin,HIGH);
    delayMicroseconds(250);
    digitalWrite(sendPin,LOW);
    delayMicroseconds(600);
  }
}

void encodeNumber(int num) {
  
  for (int i=0; i<4; i++) {
    if(bitRead(num,3-i)) { //a one
      pulse(1);
    }
    else{ //a zero
      pulse(0);
    }
  }
}
