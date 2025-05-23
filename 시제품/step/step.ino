int PUL=5; //define Pulse pin
int DIR=6; //define Direction pin
int ENA=7; //define Enable Pin
void setup() {
  pinMode (PUL, OUTPUT);
  pinMode (DIR, OUTPUT);
  pinMode (ENA, OUTPUT);

}

void loop() {

  for (int i=0; i<6400; i++)    //Forward 5000 steps
  {
    digitalWrite(DIR,LOW);
    digitalWrite(ENA,HIGH);
    digitalWrite(PUL,HIGH);
    delayMicroseconds(10);
    digitalWrite(PUL,LOW);
    delayMicroseconds(10);
  }

 delay(1000);
  for (int i=0; i<6400; i++)   //Backward 5000 steps
  {
    digitalWrite(DIR,HIGH);
    digitalWrite(ENA,HIGH);
    digitalWrite(PUL,HIGH);
    delayMicroseconds(10);
    digitalWrite(PUL,LOW);
    delayMicroseconds(10);
  }
  delay(1000);
}
