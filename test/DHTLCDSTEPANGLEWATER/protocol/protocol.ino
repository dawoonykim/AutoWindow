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
int Relaypin=6;
int switchs=2,counts=0,sts=0; //sts:statestop
int switchc=3,countc=0,stc=0; //stc:statechange
int bss,bsc; //bss:buttonstatestop, bsc:buttonstatechange
int cw,ccw;
int sun;
int waterVR,rain;


void setup()
{
  Serial.begin(9600);
  dht.begin();
  lcd.init();
  lcd.backlight();
  lcd.begin(16,2);
  myStepper.setSpeed(16);
  pinMode(Relaypin,OUTPUT); // 릴레이핀을 출력으로 설정
  pinMode(switchs,INPUT_PULLUP); //stop switch를 입력으로 설정
  pinMode(switchc,INPUT_PULLUP); //change switch를 입력으로 설정
  pinMode(13,OUTPUT); // watercensor LED // watercensor LED
  attachInterrupt(0,stopswitch,RISING);
  attachInterrupt(1,changeswitch,RISING);
  
}

void stopswitch()
{
  motVR=map(VR,0,1023,0,360); //VR값을 0~360으로 변환
  angleVR=map(motVR,0,360,0,2048);
  

  
  
 
}

void changeswitch()
{
  motVR=map(VR,0,1023,0,360); //VR값을 0~360으로 변환
  angleVR=map(motVR,0,360,0,2048);
}


void loop()
{
  
  VR=analogRead(0);
  winVR=map(VR,0,1023,0,100); //VR값을 0~100으로 변환
  motVR=map(VR,0,1023,0,360); //VR값을 0~360으로 변환
  
  waterVR=analogRead(2);
  rain=map(waterVR,0,1023,0,100);
  
  //lcd.createChar(0,W0);
  lcd.createChar(7,AG7);
  lcd.setCursor(0,0);
  //lcd.createChar(1,W1);
  lcd.createChar(8,AG8);
  lcd.setCursor(1,0);

  //lcd.write(byte(0));
  lcd.write(byte(7));
  //lcd.write(byte(1));
  lcd.write(byte(8));

  lcd.print("=");
  //Serial.print("창문 = ");
  Serial.print("각도 = ");
  //lcd.print(winVR);
  lcd.print(motVR);
  
  //lcd.print("%");

  if(motVR<10)
  {
    lcd.createChar(9,AG9);
    lcd.setCursor(5,0);
    lcd.write(byte(9));
  }
  else if(motVR<100)
  {
    lcd.createChar(9,AG9);
    lcd.setCursor(6,0);
    lcd.write(byte(9));
  }
  else
  {
    lcd.createChar(9,AG9);
    lcd.setCursor(7,0);
    lcd.write(byte(9));
  
  }
  
  //Serial.print(winVR);
  //Serial.print("%");
  Serial.print(motVR);
  Serial.print("도");

  CDS=analogRead(1);
  sun=map(CDS,0,700,0,100);

  lcd.setCursor(9,0);
  if(sun>65)
  {
    lcd.print("Day");
  }
  else if(sun<35)
  {
    lcd.print("Night");
  }
  else
  {
    lcd.print("Sunset");
  }
    
  Serial.print(" 햇빛량=");
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
  Serial.print("강수확률 : ");
  Serial.print(rain);
  Serial.println("%");
  delay(100);

  
  
  angleVR=map(motVR,0,360,0,2048);
  {
    if(digitalRead(switchs)==LOW) // stop 스위치를 누르면
    {
      digitalWrite(Relaypin,HIGH); //1채널 릴레이 ON
      counts+=1;
      delay(1);
    }
    else if(digitalRead(switchs)==HIGH) // stop 스위치를 누르지 않으면
    {
      digitalWrite(Relaypin,LOW); //1채널 릴레이 OFF
      delay(1);
    }
    
    if(rain>=50) //watercensor 50% 감지되면 -angleVR(각도)로 회전
    {
      ccw=-angleVR;
      myStepper.step(ccw/2);
    }
    else if(counts%2==0) 
    {
      cw=angleVR=0;
      myStepper.step(cw);
    }
  }
    
  
   bsc=digitalRead(switchc); // 버튼의 상태 입력
  {
    if(bsc==HIGH) // 버튼이 클릭되지 않았을 때
    {
      if(stc==0) // 만약 전 상태가 0이었다면
      {
        delay(1); // 약간의 딜레이 후
        stc=1; // 상태가 1이 된다.
      }
    }
    if(bsc==LOW) // 버튼이 클릭되는 순간
    {
      if(stc==1) // 전 상태가 1이었다면
      {
        countc+=1; // change count가 1 더해진다
        delay(1); // 약간의 딜레이 후
        stc=0; // 상태는 0이 된다.
      }
    }
   
   
    if(countc%2==1) //change count가 1이면 창문 열림
    {
      if(rain>=50)
      {
        angleVR=0;
      } 
      cw=angleVR;
      myStepper.step(cw);
    }
    else if(countc%2==0) //change count가 0이면 창문 닫힘
    {
      myStepper.step(-angleVR); 
    }
    
    
  }
  Serial.println(motVR);
  
  lcd.clear();

}
