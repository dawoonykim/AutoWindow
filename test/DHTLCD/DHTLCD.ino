#include <DHT.h>
#include <DHT_U.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#define DHTPIN 8
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

LiquidCrystal_I2C lcd(0x27,20,4);

    byte W0[8]={B01010,B11110,B01011,B10110,B00000,B00100,B01010,B00100};
    byte W1[8]={B01110,B01010,B01110,B00000,B11111,B00100,B10000,B11111};
    
    byte T2[8]={B01110,B10001,B01110,B00100,B11111,B00000,B10000,B11111};
    byte T3[8]={B01111,B01000,B01000,B01111,B00000,B00010,B00010,B11111};
    byte T4[8]={B00010,B00101,B00010,B00000,B00000,B00000,B00000,B00000};
    
    byte H5[8]={B00100,B01010,B00000,B11111,B01010,B01110,B01010,B01110};
    byte H6[8]={B01111,B01000,B01000,B01111,B00000,B00010,B00010,B11111};
    

int CDS,VR;
int winVR,motVR;
void setup()
{
  Serial.begin(9600);
  dht.begin();
  lcd.init();
  lcd.backlight();
  lcd.begin(16,2);
  
}


void loop()
{
  
  VR=analogRead(0);
  winVR=map(VR,0,1023,0,100);
  motVR=map(VR,0,1023,0,360);
  lcd.createChar(0,W0);
  lcd.setCursor(0,0);
  lcd.createChar(1,W1);
  lcd.setCursor(1,0);

  lcd.write(byte(0));
  lcd.write(byte(1));

  lcd.print("=");
  Serial.print("창문 = ");
  lcd.print(winVR);
  lcd.print("%");
  Serial.print(VR);

  CDS=analogRead(1);

  lcd.setCursor(9,0);
  if(CDS>500)
  {
    lcd.print("Day");
  }
  else if(CDS<250)
  {
    lcd.print("Night");
  }
  else
  {
    lcd.print("Sunset");
  }
    
  Serial.print(" 햇빛량=");
  Serial.println(CDS);
  
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
  delay(1000);
  lcd.clear();
  
  
 
}
