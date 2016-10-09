/*  
 *   For sending RF signals from battery operated ATtiny85
 *   Code by Thomas Friberg (https://github.com/tomtheswede)
 *   Updated 9/10/2016
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
    triggered=true;
  }
}


void encodeMessage() { //message should read 3,2,f5
  encodeNumber(2); //A signal to level the receiver/prep for start signal
  encodeNumber(15); //Start
  encodeNumber(devID); //code 4, reads as 3
  encodeNumber(1); //comma
  encodeNumber(sensorName1); //code 3 reads as 2
  //encodeNumber(1); //comma
  //encodeNumber(4); //3
  //encodeNumber(12);//full stop
  //encodeNumber(13);
  //encodeNumber(6); //5
  encodeNumber(14);//End
}

void encodeNumber(int num) {
  for (int i=0; i<num; i++) {
    digitalWrite(sendPin,HIGH);
    delayMicroseconds(430);
    digitalWrite(sendPin,LOW);
    delayMicroseconds(400);
  }
  delayMicroseconds(900);
}
