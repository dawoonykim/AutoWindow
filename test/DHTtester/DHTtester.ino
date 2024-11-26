#include <DHT.h>
#include <DHT_U.h>



#define DHTPIN 7
#define DHTTYPE DHT11

DHT dht(DHTPIN,DHTTYPE);
void setup()
{
  Serial.begin(9600);
  dht.begin();
}

void loop()
{
  float hum,tem;
  tem=dht.readTemperature();
  hum=dht.readHumidity();

 

  Serial.print("온도 : ");
  Serial.print(tem);
  Serial.print("습도 : ");
  Serial.println(hum);

  delay(1000);
}
