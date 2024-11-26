#include <DHT.h>
#include <DHT_U.h>
#include <LiquidCrystal_I2C.h>
#define DHTPIN 3                // DHT11 연결핀
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27,20,4);
 
 byte t[8]={B10111,B10100,B00100,B00100,B00100,B00100,B00100,B00111};

int Dust=0;
char str1[10];

int Vo=A2;
int V_LED=8;

float Vo_value=0;
float Voltage=0;
float a=0;
 

void setup()
{
  Serial.begin(9600);        // 시리얼 통신 시작
  dht.begin();
  lcd.init();
  lcd.backlight();
  lcd.begin(20, 4);
  
  pinMode(V_LED,OUTPUT);
  pinMode(Vo,INPUT);
}

void loop()
{

  //먼지센서 코드
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

  Serial.print("Dust:");
  Serial.println(Dust);
  Serial.println("");

  delay(500);

  lcd.setCursor(0,3);
  lcd.print("Dust : ");
  sprintf(str1,"%03d",Dust);
  lcd.print(str1);

  lcd.write(0);
  lcd.print("g/m");
  lcd.write(1);

  delay(1500);
  
  //dht11 온습도센서 코드

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
}
