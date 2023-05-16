/* Connections
Relay. D3
Relay.2 D6
Btn.   D8
Soil.  A0
PIR.   D5
SDA.   D2
SCL.   D1
Temp.  D4
*/

//Include the library files
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <DHT.h>
#include <OneWire.h>
#include <DallasTemperature.h>

//Initialize the LCD display

LiquidCrystal_I2C lcd(0x27, 16, 2);


char auth[] = "ua_9QCWSnw5lY_RSqq8h4x-qFLSNXxpd";  //Enter your Blynk Auth token
char ssid[] = "IOT";  //Enter your WIFI SSID
char pass[] = "12345678";  //Enter your WIFI Password

OneWire oneWire(D7); // digital D7 pin DS18B20 sensor
DallasTemperature sensors(&oneWire);
DHT dht(D4, DHT11);//(DHT sensor pin,sensor type)  D4 DHT11 Temperature Sensor
BlynkTimer timer;

//Define component pins
#define soil A0     //A0 Soil Moisture Sensor

int relay1State = LOW;
int relay2State = LOW;

#define RELAY_PIN_1       D3   //D3 Relay for Water pump
#define RELAY_PIN_2       D6   //D6 Relay 2 for LED
#define VPIN_BUTTON_1    V12 
#define VPIN_BUTTON_2    V13 

float  temp = 0;

void setup() {
  Serial.begin(115200);
    
  lcd.init();
  lcd.backlight();

  pinMode(RELAY_PIN_1, OUTPUT);
  pinMode(RELAY_PIN_2, OUTPUT);
  digitalWrite(RELAY_PIN_1, LOW);
  digitalWrite(RELAY_PIN_2, LOW);
 
 
  Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);
  sensors.begin();
  dht.begin();

  lcd.setCursor(0, 0);
    lcd.print(" IOT Smart Agri. ");
  for (int a = 5; a <= 10; a++) {
    lcd.setCursor(a, 1);
    lcd.print(".");
    delay(1000);
  }
  lcd.clear();
  //Call the function
  timer.setInterval(100L, soilMoistureSensor);
  timer.setInterval(100L, DHT11sensor);
  timer.setInterval(1000L, sendTemps);
}

//Get the DHT11 sensor values
void DHT11sensor() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  Blynk.virtualWrite(V0, t);
  Blynk.virtualWrite(V1, h);

  lcd.setCursor(0, 0);
  lcd.print("T:");
  lcd.print(t);

  lcd.setCursor(8, 0);
  lcd.print("H:");
  lcd.print(h);

}

//Get the soil moisture values
void soilMoistureSensor() {
  int value = analogRead(soil);
  value = map(value, 0, 1024, 0, 100);
  value = (value - 100) * -1;
  
  Blynk.virtualWrite(V3, value);
  lcd.setCursor(0, 1);
  lcd.print("SM:");
  lcd.print(value);

}


//Get the soil temperature
void sendTemps()
{
 sensors.requestTemperatures();
 temp = sensors.getTempCByIndex(0);
 Serial.println(String("Sıcaklik=")+temp+ String(" C"));
 Blynk.virtualWrite(V10, temp);
  lcd.setCursor(7, 1);
  lcd.print("ST:");
  lcd.print(temp);
  lcd.print(" ");
}

BLYNK_CONNECTED() {
  // Request the latest state from the server
  Blynk.syncVirtual(VPIN_BUTTON_1);
  Blynk.syncVirtual(VPIN_BUTTON_2);
}

BLYNK_WRITE(VPIN_BUTTON_1) {
  relay1State = param.asInt();
  digitalWrite(RELAY_PIN_1, relay1State);
}

BLYNK_WRITE(VPIN_BUTTON_2) {
  relay2State = param.asInt();
  digitalWrite(RELAY_PIN_2, relay2State);
}

void loop() {
   
  Blynk.run();//Run the Blynk library
  timer.run();//Run the Blynk timer

}
