#include <DHT.h>
#include <DHT_U.h>
#include <LiquidCrystal_I2C.h>
#define DHTPIN 5
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

LiquidCrystal_I2C lcd(0x27, 20, 4);
byte t[8] = {B10111, B10100, B00100, B00100, B00100, B00100, B00100, B00111};
byte unit[8] = {B11100, B00100, B11100, B00100, B11100, B00000, B00000, B00000};

//긴급제동 스위치 변수
int mode1=0, mode2=0, mode3=0;
int sw = 3, csw=18, osw=19;
int led = 2;
int stop1=14;

boolean current1 = LOW, current2 = LOW, current3 = LOW;
boolean last1 = LOW, last2 = LOW, last3 = LOW;

//motㅐr pin 번호 설정
int PUL = 48; //define Pulse pin
int DIR = 50; //define Direction pin
int ENA = 52; //define Enable Pin

//각종 센서 변수 선언
int rain, CDS, sun, ranger;

//미세먼지센서 변수 선언
int Dust = 0;
char str1[10];
int Vo = 0, V_LED = 7;
float Vo_value = 0, Voltage = 0;
int g = 11, y = 12, r = 13;
int limit0 = 30, limit1 = 32, limit2 = 34, limit3 = 36, limit4 = 38;
int ranged;

void setup()
{
  Serial.begin(9600);
  dht.begin();
  lcd.init();
  lcd.backlight();
  lcd.begin(20, 4);

  pinMode (PUL, OUTPUT);
  pinMode (DIR, OUTPUT);
  pinMode (ENA, OUTPUT);

  pinMode(V_LED, OUTPUT);
  pinMode(Vo, INPUT);

  pinMode(g, OUTPUT);
  pinMode(y, OUTPUT);
  pinMode(r, OUTPUT);

  pinMode(limit0, INPUT);
  pinMode(limit1, INPUT);
  pinMode(limit2, INPUT);
  pinMode(limit3, INPUT);
  pinMode(limit4, INPUT);

  pinMode(led, OUTPUT);
  pinMode(sw, INPUT);
  pinMode(stop1,OUTPUT);
 
  attachInterrupt(digitalPinToInterrupt(sw),movestop,HIGH);

  pinMode(osw,INPUT);
  pinMode(csw,INPUT);
  attachInterrupt(digitalPinToInterrupt(osw),Open,HIGH);
  attachInterrupt(digitalPinToInterrupt(csw),Close,HIGH);
  
  
}



void loop()
{
  work();
  activate();
  Open();
  Close();
}

void work()
{
  // 비가 오는 정도 LCD 출력
  rain = analogRead(2);
  ranger = map(rain, 0, 1024, 0, 3);
  lcd.setCursor(0, 0);
  lcd.print("Rain = ");
  switch (ranger)
  {
    case 0:
      lcd.print("Flood          ");
      break;
    case 1:
      lcd.print("Rain Warning");
      break;
    case 2:
      lcd.print("Not Raining    ");
      break;

  }
  delay(1);
  Serial.println(rain);

  // 햇빛량 LCD 출력
  CDS = analogRead(4);
  int sun = map(CDS, 0, 500, 0, 1024);
  lcd.setCursor(0, 1);
  lcd.print("Sunlight = ");

  if (sun < 700 && sun >= 200)
    lcd.print("DAY     ");

  else if (sun < 200 && sun >= 100)
    lcd.print("SUNSET");

  else if (sun < 100)
    lcd.print("NIGHT   ");

  Serial.print(" 햇빛량=");

  Serial.println(sun);

  // 온도와 습도 LCD 출력
  int hum, tem;
  tem = dht.readTemperature();
  hum = dht.readHumidity();

  lcd.setCursor(0, 2);
  lcd.print("Temp= ");
  lcd.print(tem);

  lcd.createChar(2, t);
  lcd.setCursor(8, 2);
  lcd.write(byte(2));

  Serial.print("온도 : ");
  Serial.println(tem);

  lcd.setCursor(10, 2);
  lcd.print("Humi= ");
  lcd.print(hum);

  Serial.print("습도 : ");
  lcd.print("%");
  Serial.println(hum);

  // 미세먼지센서 코드
  digitalWrite(V_LED, LOW);
  delayMicroseconds(280);
  Vo_value = analogRead(Vo);
  delayMicroseconds(40);
  digitalWrite(V_LED, HIGH);
  delayMicroseconds(9680);

  Voltage = Vo_value * 5.0 / 1024.0;

  Serial.print("Voltage:");
  Serial.println(Voltage);

  if (Voltage < 0.1)
  {
    Dust = 0;
  }
  else if (Voltage < 0.8)
  {
    Dust = 10 * ((Voltage - 0.1) * 5);
  }

  else if (Voltage < 1)
  {
    Dust = 10 + 15 * ((Voltage - 0.8) * 10);
  }

  else if (Voltage > 1)
  {
    Dust = 40 + 16 * ((Voltage - 1) * 10);
  }
  ranged = map(Dust, 0, 460, 0, 100);
  Serial.print("Dust:");
  Serial.println(Dust);
  Serial.println("");
  Serial.print("Dust Range :");
  Serial.println(ranged);
  Serial.println("");

  delay(500);

  lcd.setCursor(0, 3);
  lcd.print("Dust: ");
  sprintf(str1, "%03d", Dust);
  lcd.print(str1);


  lcd.print("g/m");


  lcd.createChar(1, unit);
  lcd.setCursor(12, 3);
  lcd.write(byte(1));


  if (ranged < 16)// 좋음
  {
    digitalWrite(g, HIGH);
    digitalWrite(y, LOW);
    digitalWrite(r, LOW);
    lcd.setCursor(14, 3);
    lcd.print("Good  ");
  }
  else if (ranged >= 16 && ranged < 36)// 보통
  {
    digitalWrite(g, LOW);
    digitalWrite(y, HIGH);
    digitalWrite(r, LOW);
    lcd.setCursor(14, 3);
    lcd.print("Nomal  ");
  }
  else if (ranged >= 36 && ranged < 76) // 나쁨
  {
    digitalWrite(g, LOW);
    digitalWrite(y, HIGH);
    digitalWrite(r, LOW);
    delay(100);
    digitalWrite(y, LOW);
    lcd.setCursor(14, 3);
    lcd.print("Bad   ");
  }

  else if (ranged >= 76)// 매우나쁨
  {
    digitalWrite(g, LOW);
    digitalWrite(y, LOW);
    digitalWrite(r, HIGH);
    lcd.setCursor(14, 3);
    lcd.print("TooBad");
  }
  delay(100);
}

void activate() // 각종 수치를 비교하여 모터 동작
{
  digitalWrite(stop1,mode1); 
   
  if (mode1 == 1)
  {
    digitalWrite(ENA, LOW);
    digitalWrite(PUL, LOW);
    digitalWrite(led, HIGH);
  }

  else if (mode1 == 0)
  {
    digitalWrite(led,LOW);
    if (ranger == 2) // 비 아예 안오는 상황
    {
      if (ranged < 75) //창문열기
      {
        if (ranged < 25)
        {
          for (int i = 0; i < 6400; i++)
          {
            if (digitalRead(limit4) == HIGH)
            {
              digitalWrite(DIR, HIGH);
              digitalWrite(ENA, HIGH);
              digitalWrite(PUL, HIGH);
              delayMicroseconds(7);
              digitalWrite(PUL, LOW);
              delayMicroseconds(7);
            }
            else if (digitalRead(limit4) == LOW)
            {
              digitalWrite(ENA, LOW);
              digitalWrite(PUL, LOW);
            }
          }
        }
        else if (ranged >= 25 && ranged < 50)
        {
          for (int i = 0; i < 6400; i++)
          {
            if (digitalRead(limit3) == HIGH)
            {
              digitalWrite(DIR, HIGH);
              digitalWrite(ENA, HIGH);
              digitalWrite(PUL, HIGH);
              delayMicroseconds(7);
              digitalWrite(PUL, LOW);
              delayMicroseconds(7);
            }
            else if (digitalRead(limit3) == LOW)
            {
              digitalWrite(ENA, LOW);
              digitalWrite(PUL, LOW);
            }
          }
        }
        else if (ranged >= 50 && ranged < 75)
        {
          for (int i = 0; i < 6400; i++)
          {
            if (digitalRead(limit2) == HIGH)
            {
              digitalWrite(DIR, HIGH);
              digitalWrite(ENA, HIGH);
              digitalWrite(PUL, HIGH);
              delayMicroseconds(7);
              digitalWrite(PUL, LOW);
              delayMicroseconds(7);
            }
            else if (digitalRead(limit2) == LOW)
            {
              digitalWrite(ENA, LOW);
              digitalWrite(PUL, LOW);
            }
          }
        }
      }

      else if (ranged >= 75)
      {
        for (int i = 0; i < 6400; i++)
        {
          if (digitalRead(limit0) == HIGH)
          {
            digitalWrite(DIR, LOW);
            digitalWrite(ENA, HIGH);
            digitalWrite(PUL, HIGH);
            delayMicroseconds(7);
            digitalWrite(PUL, LOW);
            delayMicroseconds(7);
          }
          else if (digitalRead(limit0) == LOW)
          {
            digitalWrite(ENA, LOW);
            digitalWrite(PUL, LOW);
          }
        }
      }
    }

    else if (ranger <= 1) //창문닫기
    {
      if (ranger == 1) //비가 조금 내릴 때
      {
        for (int i = 0; i < 6400; i++)
        {
          if (digitalRead(limit1) == HIGH)
          {
            digitalWrite(DIR, LOW);
            digitalWrite(ENA, HIGH);
            digitalWrite(PUL, HIGH);
            delayMicroseconds(7);
            digitalWrite(PUL, LOW);
            delayMicroseconds(7);
          }

          else if (digitalRead(limit1) == LOW)
          {
            digitalWrite(ENA, LOW);
            digitalWrite(PUL, LOW);
          }
        }
      }
      else if (ranger == 0) //비가 많이 내릴 때
      {
        for (int i = 0; i < 6400; i++)
        {
          if (digitalRead(limit0) == HIGH)
          {
            digitalWrite(DIR, LOW);
            digitalWrite(ENA, HIGH);
            digitalWrite(PUL, HIGH);
            delayMicroseconds(7);
            digitalWrite(PUL, LOW);
            delayMicroseconds(7);
          }

          else if (digitalRead(limit0) == LOW)
          {
            digitalWrite(ENA, LOW);
            digitalWrite(PUL, LOW);
          }
        }
      }
    }

  }
}

void movestop() // 모터 동작 일시정지 스위치
{
  current1=digitalRead(sw);
  if(last1==!current1)
  {
    mode1 = !mode1;
    mode2=0;
    mode3=0;
  }
 
  delay(100); 
}

void Open() //창문열림 스위치
{
  current2=digitalRead(osw);
  if(last2==!current2)
  {
    mode2 = !mode2;
    mode1=2;
    mode3=0;
  }

  if(mode2==1)
  {
    for (int i = 0; i < 6400; i++)
          {
            if (digitalRead(limit4) == HIGH)
            {
              digitalWrite(DIR, HIGH);
              digitalWrite(ENA, HIGH);
              digitalWrite(PUL, HIGH);
              delayMicroseconds(7);
              digitalWrite(PUL, LOW);
              delayMicroseconds(7);
            }
            else if (digitalRead(limit4) == LOW)
            {
              digitalWrite(ENA, LOW);
              digitalWrite(PUL, LOW);
            }
          }
       
  }
  else if(mode2==0)
  {
    digitalWrite(ENA,LOW);
    digitalWrite(PUL,LOW);
   
  } 
}

void Close() //창문닫힘 스위치
{
  current3=digitalRead(csw);
  if(last3==!current3)
  {
    mode3 = !mode3;
    mode1=2;
    mode2=0;
  }

  if(mode3==1)
  {
    for (int i = 0; i < 6400; i++)
    {
          if (digitalRead(limit0) == HIGH)
          {
            digitalWrite(DIR, LOW);
            digitalWrite(ENA, HIGH);
            digitalWrite(PUL, HIGH);
            delayMicroseconds(7);
            digitalWrite(PUL, LOW);
            delayMicroseconds(7);
          }

          else if (digitalRead(limit0) == LOW)
          {
            digitalWrite(ENA, LOW);
            digitalWrite(PUL, LOW);
          }
          
    }
  }
  else if(mode3==0)
  {
     digitalWrite(ENA,LOW);
     digitalWrite(PUL,LOW);
    
  }
  
}
