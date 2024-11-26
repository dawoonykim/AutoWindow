#include <DHT.h>
#include <DHT_U.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <Stepper.h>
#define DHTPIN 8
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE); 
const int stepsPerRevolution=2048;
Stepper myStepper(stepsPerRevolution,9,11,10,12);
LiquidCrystal_I2C lcd(0x27,20,4);

    //byte W0[8]={B01010,B11110,B01011,B10110,B00000,B00100,B01010,B00100};
    //byte W1[8]={B01110,B01010,B01110,B00000,B11111,B00100,B10000,B11111};
    
    byte T2[8]={B01110,B10001,B01110,B00100,B11111,B00000,B10000,B11111};
    byte T3[8]={B11111,B10000,B10000,B11111,B00000,B00100,B00100,B11111};
    byte T4[8]={B00010,B00101,B00010,B00000,B00000,B00000,B00000,B00000};
    
    byte H5[8]={B01110,B10001,B00000,B11111,B01010,B01110,B01010,B01110};
    byte H6[8]={B11111,B10000,B10000,B11111,B00000,B00100,B00100,B11111};

    byte AG7[8]={B00000,B11010,B01011,B01010,B00000,B01110,B00010,B00000};
    byte AG8[8]={B00000,B01110,B01000,B01110,B00100,B00100,B01110,B00000};
    byte AG9[8]={B01000,B10100,B01000,B00000,B00000,B00000,B00000,B00000};

int CDS,VR;
int winVR,motVR,angleVR;
int Relaypin1=6;
int switchs=2,counts=0,sts=0; //sts:statestop
int switchc=3,countc=0,stc=0; //stc:statechange
int sun;
int waterVR,water;


void setup()
{
  Serial.begin(9600);
  dht.begin();
  lcd.init();
  lcd.backlight();
  lcd.begin(16,2);
  myStepper.setSpeed(16);
  pinMode(Relaypin1,OUTPUT); // 릴레이핀을 출력으로 설정
  pinMode(switchs,INPUT_PULLUP); //stop switch를 입력으로 설정
  pinMode(switchc,INPUT_PULLUP); //change switch를 입력으로 설정
  pinMode(13,OUTPUT); // watercensor LED // watercensor LED
}

void loop()
{
  
  VR=analogRead(0);
  winVR=map(VR,0,1023,0,100); //VR값을 0~100으로 변환
  motVR=map(VR,0,1023,0,360); //VR값을 0~360으로 변환
  
  waterVR=analogRead(2);
  water=map(waterVR,0,1023,0,100); //waterVR 값을 0~100으로 변환
  
  //lcd.createChar(0,W0); 
  lcd.createChar(7,AG7); // 글자 '각'
  lcd.setCursor(0,0); // 커서 설정
  //lcd.createChar(1,W1); 
  lcd.createChar(8,AG8); //글자 '도'
  lcd.setCursor(1,0);

  //lcd.write(byte(0));
  lcd.write(byte(7)); // '각' 생성
  //lcd.write(byte(1)); 
  lcd.write(byte(8)); // '도' 생성

  lcd.print("=");
  //Serial.print("창문 = ");
  Serial.print("각도 = ");
  //lcd.print(winVR);
  lcd.print(motVR);
  
  //lcd.print("%");

  if(motVR<10) // motVR의 크기가 10 미만이면
  {
    lcd.createChar(9,AG9);
    lcd.setCursor(5,0); // 숫자 바로 옆에 '도' 표시
    lcd.write(byte(9));
  }
  else if(motVR<100) // motVR의 크기가 10 이상 100 미만이면
  {
    lcd.createChar(9,AG9);
    lcd.setCursor(6,0); // 숫자 바로 옆에 '도' 표시
    lcd.write(byte(9));
  }
  else // motVR의 크기가 100초과이면
  {
    lcd.createChar(9,AG9);
    lcd.setCursor(7,0); // 숫자 바로 옆에 '도' 표시
    lcd.write(byte(9));
  
  }
  
  //Serial.print(winVR);
  //Serial.print("%");
  Serial.print(motVR);
  Serial.print("도");

  CDS=analogRead(1);
  sun=map(CDS,0,700,0,100); // CDS 값을 0~100으로 변환

  lcd.setCursor(9,0);
  if(sun>65) // 65 이상이면 '낮' 표시
  {
    lcd.print("Day");
  }
  else if(sun<35) // 35 미만이면 '밤' 표시
  {
    lcd.print("Night");
  }
  else // 그 외의 값은 '새벽' 표시
  {
    lcd.print("Sunset");
  }
    
  Serial.print(" 햇빛의 양=");
  Serial.print(sun);
  Serial.println("%");
  
  
  int hum,tem;
  tem=dht.readTemperature();
  hum=dht.readHumidity();

  
  lcd.createChar(2,T2);
  lcd.setCursor(0,1);
  lcd.createChar(3,T3);
  lcd.setCursor(1,1);
  
  lcd.write(byte(2));
  lcd.write(byte(3));
  lcd.print(":");
  Serial.print("온도 : ");
  lcd.print(tem);
  
  lcd.createChar(4,T4);
  lcd.setCursor(6,1);
  lcd.write(byte(4));
  
  lcd.print("C ");
  Serial.print(tem);
  
  lcd.createChar(5,H5);
  lcd.setCursor(8,1);
  lcd.createChar(6,H6);
  lcd.setCursor(9,1);
  
  lcd.write(byte(5));
  lcd.write(byte(6));
  lcd.print(":");
  Serial.print("습도 : ");
  lcd.print(hum);
  lcd.print("%");
  Serial.println(hum);
  delay(100);

  if(waterVR>100)
  {
    digitalWrite(13,HIGH);
  }

  else
  {
    digitalWrite(13,LOW);
  }
  Serial.print("물 감지 : ");
  Serial.print(water);
  Serial.println("%");
  delay(100);
  angleVR=map(motVR,0,360,0,2048);
  
  {
    
    {
      if(digitalRead(switchs)==LOW) // stop 스위치를 누르면
      {
        digitalWrite(Relaypin1,HIGH); //1채널 릴레이 ON
        counts+=1;
        delay(1);
      }
      else if(digitalRead(switchs)==HIGH) // stop 스위치를 누르지 않으면
      {
        digitalWrite(Relaypin1,LOW); //1채널 릴레이 OFF
        delay(1);
      }
    }
      
    {
      if(digitalRead(switchc)==HIGH) // 버튼이 클릭되지 않았을 때
        {
          if(stc==0) // 만약 전 상태가 0이었다면
          {
            delay(1); // 약간의 딜레이 후
            stc=1; // 상태가 1이 된다. 
          }
        }
         else if(digitalRead(switchc)==LOW) // 버튼이 클릭되는 순간
        {
          if(stc==1) // 전 상태가 1이었다면
          {
            countc+=1; // change count가 1 더해진다
            delay(1); // 약간의 딜레이 후
            stc=0; // 상태는 0이 된다.
          }
        }  
      }

    if(waterVR>=50)
    {
      myStepper.step(-angleVR);
    }
    else
    {
      if(counts%2==0)
      {
        myStepper.step(angleVR=0);
      }
    
      else
      {
        if(countc%2==1)
        {
          myStepper.step(angleVR);
        }
        else if(countc%2==0)
        {
          myStepper.step(-angleVR);
        }
      }
      
    }
      
  }
  Serial.println(motVR);  
  lcd.clear();

}
