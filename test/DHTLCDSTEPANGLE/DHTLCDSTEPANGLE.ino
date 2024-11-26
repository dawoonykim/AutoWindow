#include <DHT.h>
#include <DHT_U.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <Stepper.h>
#define DHTPIN 7
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE); 
const int stepsPerRevolution=2048;
Stepper myStepper(stepsPerRevolution,9,11,10,12);
LiquidCrystal_I2C lcd(0x27,16,2);

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
int Relaypin=4;
int switchs=2,counts=0,sts=0; //sts:statestop
int switchc=3,countc=0,stc=0; //stc:statechange
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
  pinMode(Relaypin,OUTPUT);
  pinMode(5,OUTPUT);
  pinMode(switchs,INPUT_PULLUP);
  pinMode(switchc,INPUT_PULLUP);
  
}


void loop()
{
  
  VR=analogRead(0);
  winVR=map(VR,0,1023,0,100);
  motVR=map(VR,0,1023,0,360);
 
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
  
  angleVR=map(motVR,0,360,0,2048);
  {
    if(digitalRead(Relaypin)==LOW)
    {
      digitalWrite(5,HIGH);
      counts+=1;
      delay(100);
    }
    else if(digitalRead(Relaypin)==HIGH)
    {
      digitalWrite(5,LOW);
      delay(100);
    }

    if(counts%2==0)
    {
      angleVR=0;
      myStepper.step(angleVR); 
    }
  }
    
  bsc=digitalRead(switchc);
  {
    if(bsc==HIGH)
    {
      if(stc==0)
      {
        delay(100);
        stc=1;
      }
    }
    if(bsc==LOW)
    {
      if(stc==1)
      {
        countc+=1;
        delay(100);
        stc=0;
      }
    }
    if(countc%2==1)
    {
      myStepper.step(angleVR);  
    }
    else if(countc%2==0)
    {
      myStepper.step(-angleVR);
    }
  }
  Serial.println(motVR); 


  
  lcd.clear();
  
  
 
}
