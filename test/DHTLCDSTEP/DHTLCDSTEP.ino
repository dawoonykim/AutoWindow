#include <DHT.h>
#include <DHT_U.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <Stepper.h>
#define DHTPIN 8
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE); 
const int stepsPerRevolution=2048;
Stepper myStepper(stepsPerRevolution,8,10,9,11);
LiquidCrystal_I2C lcd(0x27,20,4);

    //byte W0[8]={B01010,B11110,B01011,B10110,B00000,B00100,B01010,B00100};
    //byte W1[8]={B01110,B01010,B01110,B00000,B11111,B00100,B10000,B11111};
    
    byte T2[8]={B01110,B10001,B01110,B00100,B11111,B00000,B10000,B11111};
    byte T3[8]={B01111,B01000,B01000,B01111,B00000,B00010,B00010,B11111};
    byte T4[8]={B00010,B00101,B00010,B00000,B00000,B00000,B00000,B00000};
    
    byte H5[8]={B00100,B01010,B00000,B11111,B01010,B01110,B01010,B01110};
    byte H6[8]={B01111,B01000,B01000,B01111,B00000,B00010,B00010,B11111};

    byte AG7[8]={B00000,B11010,B001011,B01010,B00000,B01110,B00010,B00000};
    byte AG8[8]={B00000,B01110,B01000,B01110,B00100,B00100,B01110,B00000};

int CDS,VR;
int winVR,motVR,angleVR;
int Relaypin=6;
int switchs=4,counts=0,sts=0; //sts:statestop
int switchc=5,countc=0,stc=0; //stc:statechange
int bss,bsc; //bss:buttonstatestop, bsc:buttonstatechange
int sun;
void setup()
{
  Serial.begin(9600);
  dht.begin();
  lcd.init();
  lcd.backlight();
  lcd.begin(16,2);
  myStepper.setSpeed(15);
  pinMode(Relaypin,INPUT_PULLUP); // 릴레이를 출력으로 설정
  pinMode(7,OUTPUT); //릴레이 시그널 출력 핀
  pinMode(switchs,INPUT_PULLUP); //stop switch를 입력으로 설정
  pinMode(switchc,INPUT_PULLUP); //change switch를 입력으로 설정
  
}


void loop()
{
  
  VR=analogRead(0); // 아날로그 핀 0
  winVR=map(VR,0,1023,0,100); //VR값을 0~100으로 변환
  motVR=map(VR,0,1023,0,360); //VR값을 0~360으로 변환
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
  //lcd.print(motVR);
  
  //lcd.print("%");

  lcd.createChar(4,T4);
  lcd.setCursor(5,0);
  lcd.write(byte(4));
  
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
  
  angleVR=map(motVR,0,360,0,2048);
  {
    if(digitalRead(Relaypin)==LOW) // stop 스위치를 누르면
    {
      digitalWrite(7,HIGH); //1채널 릴레이 ON
      counts+=1; // stop count 증가
      delay(1); // 약간 딜레이 후
    }
    else
    {
      digitalWrite(7,LOW); // stop 스위치를 누르지 않으면
      delay(1); // 약간 딜레이 후
    }

    if(counts%2==1)
    {
      angleVR=0;
      myStepper.step(angleVR);  
    }  
  }
    
  bsc=digitalRead(switchc); //버튼의 상태 입력
  {
    if(bsc==HIGH) // 버튼이 클릭되지 않았을 때
    {
      if(stc==0) // 전 상태가 0이었다면
      {
        delay(1); // 약간 딜레이 후
        stc=1; //상태가 1이 된다
      }
    }
    if(bsc==LOW) // 버튼이 클릭되는 순간
    {
      if(stc==1) // 전 상태가 1이었다면
      {
        countc+=1; // 카운트가 더해진다.
        delay(1); // 약간 딜레이 후
        stc=0; // 상태는 0이 된다
      }
    }
    if(countc%2==1) // 만약 1번 눌러져서 카운트가 1이 되면
    {
      myStepper.step(angleVR);  //angleVR 값만큼 회전한다.
    }
    else
    {
      myStepper.step(-angleVR);
    }
  }
  Serial.println(motVR); 


  delay(100);
  lcd.clear();
  
  
 
}
