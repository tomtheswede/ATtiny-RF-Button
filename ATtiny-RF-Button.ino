/*  
 *   For sending RF signals from battery operated ATtiny85
 *   Code by Thomas Friberg (https://github.com/tomtheswede)
 *   Updated 23/10/2016
 */

#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit)) //OR
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit)) //AND

//Interrupt variables
volatile bool triggered=false;
volatile bool buttonState=false;
volatile const unsigned int butPin = 3; //button pin
volatile unsigned long lastTrigger;

const unsigned int sendPin = 2; //RF pin
int butState1=LOW;
int butState2=HIGH;
int devID = 4; //Reads as "3" corresponds with BTN type
int sensorName1 = 3; //Reads as 2
long lastPress=1;
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
  butState2 = butState1;
  butState1 = digitalRead(butPin);

  if (triggered) {
    delay(10);
    encodeMessage();
    triggered=false;
    delay(30);
  }
  delay(0);
}

ISR(PCINT0_vect) {
  buttonState=!digitalRead(butPin);
  if (buttonState && !triggered && (millis()-lastTrigger>200 || millis()<lastTrigger)) { //includes 200ms debounce
    lastTrigger=millis();
    triggered=true;
  }
}

void encodeMessage() { //message should read 15 1 6 14 or 1111 0001 0110 1110
  //regular pulses for receiver to calibrate
  pulse(0);
  pulse(0);
  pulse(0);
  pulse(0);
  
  encodeNumber(15); //Start  1111 to initiate message
  encodeNumber(4); //number 4
  encodeNumber(11); //comma
  encodeNumber(9); //number 9
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
