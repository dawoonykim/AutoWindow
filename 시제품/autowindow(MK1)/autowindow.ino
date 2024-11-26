#include <LiquidCrystal_I2C.h>
#include <DHT.h>
#include <DHT_U.h>

#define DHTPIN 3
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE); 


LiquidCrystal_I2C lcd(0x27,20,4);

    byte t[8]={B10111,B10100,B00100,B00100,B00100,B00100,B00100,B00111};

int rain;
int CDS;
int sun;
void setup()
{
  Serial.begin(9600);
  dht.begin();
  lcd.init();
  lcd.backlight();
  lcd.begin(20,4); 
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
  CDS=analogRead(1);
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
  lcd.print("Temp = ");
  lcd.print(tem);
  
  lcd.createChar(2,t);
  lcd.write(byte(2));
  lcd.setCursor(10,2);
  
  Serial.print("온도 : ");
  Serial.println(tem);

  lcd.setCursor(0,3);
  lcd.print("Humi = ");
  lcd.print(hum);
  
  Serial.print("습도 : ");
  lcd.print(" %");
  Serial.println(hum);
  delay(100);
  
}
