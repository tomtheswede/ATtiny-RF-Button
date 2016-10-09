
//#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit)) //OR
//#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit)) //AND

const unsigned int butPin = 3;
volatile bool triggered=false;

int sendPin = 2;
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

  //Start interupts
  //sbi(GIMSK,PCIE); //Turn on interrupt
  //sbi(PCMSK,PCINT1); //set pin affected by interupt
}

void loop() {
  butState2 = butState1;
  butState1 = digitalRead(butPin);

  if (butState2==HIGH && butState1==LOW) {
    encodeMessage();
    delay(30);
  }
  delay(0);
}

//ISR(PCINT0_vect) {
//  if (digitalRead(butPin)==LOW && !triggered) {
//    triggered=true;
//  }
//}


void encodeMessage() { //message should read 3,2,f5
  encodeNumber(2); //A signal to level the receiver/prep for start signal
  encodeNumber(15); //Start
  encodeNumber(devID); //code 4, reads as 3
  encodeNumber(1); //comma
  encodeNumber(sensorName1); //code 3 reads as 2
  encodeNumber(1); //comma
  //encodeNumber(4); //3
  //encodeNumber(12);//full stop
  encodeNumber(13);
  encodeNumber(6); //5
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
