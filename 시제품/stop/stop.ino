int mode1;
int mode2;
int sw = 9;
int led = 10;
int Stop1 = 14;
int Stop2 = 15;
int count;


int PUL = 48; //define Pulse pin
int DIR = 50; //define Direction pin
int ENA = 52; //define Enable Pin


void setup() {
  Serial.begin(9600);
  pinMode(sw, INPUT);
  pinMode(led, OUTPUT);

  pinMode (PUL, OUTPUT);
  pinMode (DIR, OUTPUT);
  pinMode (ENA, OUTPUT);
  pinMode(Stop1, OUTPUT);
  pinMode(Stop2, OUTPUT);

  mode1 = 0;
  mode2= 0;

}

void loop() {
  // put your main code here, to run repeatedly:
  if (digitalRead(sw) == HIGH)
  {
    mode1 = (mode1 + 1) % 2;
    mode2 = (mode2 + 1)%2;
    if (mode1 == 1)
    {
      digitalWrite(led, HIGH);
    }
    if (mode1 == 0)
    {
      digitalWrite(led, LOW);
    }

  }



  if (mode1 == 1)
  {
    digitalWrite(Stop1, mode1);
    for (int i = 0; i < 6400; i++)
    {
      digitalWrite(led, HIGH);
      digitalWrite(DIR, 0);
      digitalWrite(ENA, 0);
      digitalWrite(PUL, 0);
      delayMicroseconds(7);
      digitalWrite(PUL, 0);
      delayMicroseconds(7);
    }

  }

  else if (mode1 == 0)
  {
    digitalWrite(Stop2, mode2);
    for (int i = 0; i < 6400; i++)
    {
      digitalWrite(DIR, HIGH);
      digitalWrite(ENA, HIGH);
      digitalWrite(PUL, HIGH);
      delayMicroseconds(7);
      digitalWrite(PUL, LOW);
      delayMicroseconds(7);
    }
  }



  delay(1000);


}
