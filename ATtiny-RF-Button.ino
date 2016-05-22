int code[14] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14};
String out[14] = {",","1","2","3","4","5","6","7","8","9","0",".","e","s"};
int sendPin = 0;

void setup() {
  // put your setup code here, to run once:
  pinMode(sendPin,OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  encodeMessage();
  encodeMessage();
  encodeMessage();
  encodeMessage();
  delay(15);
}

void encodeMessage() {
  encodeNumber(14); //Start
  encodeNumber(3); //2
  encodeNumber(1); //comma
  encodeNumber(2); //1
  encodeNumber(1); //comma
  encodeNumber(4); //3
  encodeNumber(12);//full stop
  encodeNumber(6); //5
  encodeNumber(13);//End
    
  delayMicroseconds(500);
}

void encodeNumber(int num) {
  for (int i=0; i<num; i++) {
    digitalWrite(sendPin,HIGH);
    delayMicroseconds(10);
    digitalWrite(sendPin,LOW);
    delayMicroseconds(10);
  }
  delayMicroseconds(300);
}
