/*  
 *   For sending RF signals from battery operated ATtiny85
 *   Code by Thomas Friberg (https://github.com/tomtheswede)
 *   Updated 23/10/2016
 */

#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit)) //OR
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit)) //AND

//Device parameters
const unsigned int channel = 1; //So the message can be picked up by the right receiver
const unsigned int devType = 1; //Reads as "1" corresponding with BTN type
const unsigned int sensorName1 = 3; //The number of this device - needs to be unique.

//Interrupt variables
volatile bool triggered=false;
volatile bool longPressPrimer=false;
volatile bool longerPressPrimer=false;
volatile bool longestPressPrimer=false;
volatile bool buttonState=false;
volatile const unsigned int butPin = 3; //button pin
volatile unsigned long lastTrigger;

//General variables
const unsigned int sendPin = 2; //RF pin
bool sendByte[4];

void setup() {
  // put your setup code here, to run once:
  pinMode(butPin,INPUT_PULLUP);
  pinMode(sendPin,OUTPUT);
  delay(20);
  lastTrigger=millis();

  //Start interupts
  sbi(GIMSK,PCIE); //Turn on interrupt
  sbi(PCMSK,PCINT3); //set pin affected by interupt
}

void loop() {
  if (triggered) {
    delay(0);
    encodeMessage(1);
    triggered=false;
  }
  else if (buttonState && longPressPrimer && millis()-lastTrigger>700) {
    delay(0);
    encodeMessage(2);
    longPressPrimer=false;
  }
  else if (buttonState && longerPressPrimer && millis()-lastTrigger>1700) {
    delay(0);
    encodeMessage(3);
    longerPressPrimer=false;
  }
  else if (buttonState && longestPressPrimer && millis()-lastTrigger>4000) {
    delay(0);
    encodeMessage(4);
    longestPressPrimer=false;
  }
  delay(0);
}

ISR(PCINT0_vect) {
  buttonState=!digitalRead(butPin);
  if (buttonState && !triggered && (millis()-lastTrigger>200 || millis()<lastTrigger)) { //includes 200ms debounce
    lastTrigger=millis();
    triggered=true;
    longPressPrimer=true;
    longerPressPrimer=true;
    longestPressPrimer=true;
  }
  else if (!buttonState && !triggered) {
    lastTrigger=millis();
  }
}

void encodeMessage(int msgType) { //message should read 15 1 6 14 or 1111 0001 0110 1110
  //regular pulses for receiver to calibrate
  pulse(0);
  pulse(0);
  pulse(0);
  pulse(0);
  pulse(0);
  pulse(0);
  pulse(0);
  pulse(0);
  pulse(0);
  pulse(0);
  
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
