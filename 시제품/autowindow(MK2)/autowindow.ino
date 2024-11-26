#include <DHT.h>
#include <DHT_U.h>
#include <LiquidCrystal_I2C.h>
#define DHTPIN 7
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE); 

LiquidCrystal_I2C lcd(0x27,20,4);
byte t[8]={B10111,B10100,B00100,B00100,B00100,B00100,B00100,B00111};

//moter pin 번호 설정
int PUL=48; //define Pulse pin
int DIR=50; //define Direction pin
int ENA=52; //define Enable Pin

//각종 센서 변수 선언
int rain;
int CDS;
int sun;

//미세먼지센서 변수 선언
int Dust=0;
char str1[10];
int Vo=4;
int V_LED=6;
float Vo_value=0;
float Voltage=0;
int g=4;
int y=3;
int r=2;
int limit0=30;
int limit1=32;
int limit2=34;
int limit3=36;
int limit4=38;

void setup()
{
  Serial.begin(9600);
  dht.begin();
  lcd.init();
  lcd.backlight();
  lcd.begin(20,4);

  pinMode (PUL, OUTPUT);
  pinMode (DIR, OUTPUT);
  pinMode (ENA, OUTPUT);
  
  pinMode(V_LED,OUTPUT);
  pinMode(Vo,INPUT);

  pinMode(g,OUTPUT);
  pinMode(y,OUTPUT);
  pinMode(r,OUTPUT);

  pinMode(limit0,INPUT);
  pinMode(limit1,INPUT);
  pinMode(limit2,INPUT);
  pinMode(limit3,INPUT);
  pinMode(limit4,INPUT);
 

  
}


void loop()
{
  // 비가 오는 정도 LCD 출력
  rain=analogRead(0);
  int ranger=map(rain,0,1024,0,3);
  lcd.setCursor(0,0);
  lcd.print("Rain = ");
  switch(ranger)
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
  CDS=analogRead(2);
  int sun=map(CDS,0,500,0,1024);
  lcd.setCursor(0,1);
  lcd.print("Sunlight = ");

    if(sun<700&&sun>=500)
      lcd.print("DAY     ");
     
    else if(sun<500&&sun>=300)
      lcd.print("SUNSET");
      
    else if(sun<300)
      lcd.print("NIGHT   ");
      
  Serial.print(" 햇빛량=");
  
  Serial.println(sun);
  
  // 온도와 습도 LCD 출력
  int hum,tem;
  tem=dht.readTemperature();
  hum=dht.readHumidity();
  
  lcd.setCursor(0,2);
  lcd.print("Temp= ");
  lcd.print(tem);
  
  lcd.createChar(2,t);
  lcd.setCursor(8,2);
  lcd.write(byte(2));
  
  Serial.print("온도 : ");
  Serial.println(tem);

  lcd.setCursor(10,2);
  lcd.print("Humi= ");
  lcd.print(hum);
  
  Serial.print("습도 : ");
  lcd.print("%");
  Serial.println(hum);
 
  // 미세먼지센서 코드
  digitalWrite(V_LED,LOW);
  delayMicroseconds(280);
  Vo_value=analogRead(Vo);
  delayMicroseconds(40);
  digitalWrite(V_LED,HIGH);
  delayMicroseconds(9680);

  Voltage=Vo_value*5.0/1024.0;

  Serial.print("Voltage:");
  Serial.println(Voltage);

  if(Voltage<0.1)
  {
    Dust=0;
  }
  else if(Voltage<0.8)
  {
    Dust=10*((Voltage-0.1)*5);
  }

  else if(Voltage<1)
  {
    Dust=10+15*((Voltage-0.8)*10);
  }
  
  else if(Voltage>1)
  {
    Dust=40+16*((Voltage-1)*10);
  }
  int ranged=map(Dust,0,460,0,100);
  Serial.print("Dust:");
  Serial.println(Dust);
  Serial.println("");
  Serial.print("Dust Range :");
  Serial.println(ranged);
  Serial.println("");
  
  delay(500);

  lcd.setCursor(0,3);
  lcd.print("Dust : ");
  sprintf(str1,"%03d",Dust);
  lcd.print(str1);

  lcd.write(0);
  lcd.print("g/m");
  lcd.write(1);

  if (ranged < 25)// 좋음
    {
      digitalWrite(g,HIGH);
      digitalWrite(y,LOW);
      digitalWrite(r,LOW);
    }
  if (ranger >= 25 && ranged < 50)// 중간
    {
      digitalWrite(g,LOW);
      digitalWrite(y,HIGH);
      digitalWrite(r,LOW);
    }
  if(ranged>=50&&ranged<75)// 나쁨
  {
      digitalWrite(g,LOW);
      digitalWrite(y,HIGH);
      digitalWrite(r,LOW);
      delay(100);
      digitalWrite(y,LOW);
  }

  if (ranged >= 75)// 매우나쁨
    {    
      digitalWrite(g,LOW);
      digitalWrite(y,LOW);
      digitalWrite(r,HIGH);
    }
  delay(100);

  

  if(ranger==2)
  {
       if(ranged<75)//창문열기
       {
          
            for (int i=0; i<6400; i++)   
            {
              if(digitalRead(limit4)==HIGH)
              {
                digitalWrite(DIR,HIGH);
                digitalWrite(ENA,HIGH);
                digitalWrite(PUL,HIGH);
                delayMicroseconds(7);
                digitalWrite(PUL,LOW);
                delayMicroseconds(7);
              }
              else if(digitalRead(limit4)==LOW)
              {
                digitalWrite(ENA,LOW);
                digitalWrite(PUL,LOW);
              } 
            }  
       } 

       else if(ranged>=75)
       {
          for (int i=0; i<6400; i++)
          {
            if(digitalRead(limit0)==HIGH)
            {
              digitalWrite(DIR,LOW);
              digitalWrite(ENA,HIGH);
              digitalWrite(PUL,HIGH);
              delayMicroseconds(7);
              digitalWrite(PUL,LOW);
              delayMicroseconds(7);
            }
            else if(digitalRead(limit0)==LOW)
            {
              digitalWrite(ENA,LOW);
              digitalWrite(PUL,LOW);
            }
          }  
        }
   }

  
   else if(ranger==1)
  {
    for (int i=0; i<6400; i++)
    {
      if(digitalRead(limit0)==HIGH) 
      {
        digitalWrite(DIR,LOW);
        digitalWrite(ENA,HIGH);
        digitalWrite(PUL,HIGH);
        delayMicroseconds(7);
        digitalWrite(PUL,LOW);
        delayMicroseconds(7);
      }
      
      else if(digitalRead(limit0)==LOW)
      {
        digitalWrite(ENA,LOW);
        digitalWrite(PUL,LOW);
      }
    
    }
  }
}
