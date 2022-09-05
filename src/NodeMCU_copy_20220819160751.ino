#define BLYNK_TEMPLATE_ID "TMPLJOuEzWeq"
#define BLYNK_DEVICE_NAME "Quickstart Device"
#define BLYNK_AUTH_TOKEN "xaRyx_y86B_yI03B2Jjf1PJUl_OaFVdS"
#define BLYNK_PRINT Serial
#include <Wire.h>
#include "SH1106.h"
SH1106 display(0x3c, D1, D2);
#include "images.h"
#include "font.h"
#include <Adafruit_Sensor.h>
#include "DHT.h"
#define DHTPIN D4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
bool dhtState = true;
// variable dht
float h = 0.0;
float t = 0.0;
// pin relay
#define Relay1 D5 // หลอดไฟ
#define Relay2 D6 // ปั้มน้ำ

// Soil Moisture setup
const int AirValue = 790;
const int WaterValue = 390;
const int SensorPin = A0;
int soilMoistureValue = 0;
int soilmoisturepercent = 0;
int val = 0;

// setup buttons
#define btnPump D7
#define btnLigth D8
#define btnUnknown D9
int btnPumpState = 0;
int btnLigthState = 0;
int btnUnknownState = 0;
int PumpState = 0;
int LightState = 0;
int UnknownState = 0;

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

char auth[] = BLYNK_AUTH_TOKEN;
// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "Redmi";
char pass[] = "kingsman";
bool wifiState = true;
float timeout = (millis() / 1000);
String ssidString = String(ssid);
void setup()
{
  // Debug console
  Serial.begin(9600);
  // blynk config
  WiFi.begin(ssid, pass);
  Blynk.config(auth);
  Serial.println(F("DHTxx test!"));

  // set Relay
  pinMode(Relay1, OUTPUT);
  digitalWrite(Relay1, HIGH);
  pinMode(Relay2, OUTPUT);
  digitalWrite(Relay2, HIGH);
  // btn config
  pinMode(btnPump, INPUT);
  pinMode(btnLigth, INPUT);
  pinMode(btnUnknown, INPUT);
  display.init();
  display.setFont(ArialMT_Plain_10);
  display.flipScreenVertically();
  dht.begin();
  display.clear();
  display.drawString(0, 5, "Setup");
  display.drawHorizontalLine(0, 18, 128);
  display.drawString(0, 20, "Connect Wi-Fi : ");
  display.drawString(0, 30, String(ssidString));
  display.drawHorizontalLine(0, 42, 128);
  display.display();
  while (Blynk.connect() == false)
  {
    if (((millis() / 1000) - timeout) > 10)
    {
      wifiState = false;
      break;
    }
  }
  if (wifiState)
  {
    display.drawString(0, 44, "State: OK");
    display.display();
    Serial.println("wifi connect true");
  }
  else
  {
    display.drawString(0, 44, "State: Error");
    display.display();
    Serial.println("wifi connect false");
  }
  display.clear();
  display.drawString(0, 5, "setup complete");
  display.drawHorizontalLine(0, 18, 128);
  display.display();
  delay(2000);
}

void dhtSensor()
{
  display.clear();
  h = dht.readHumidity();
  t = dht.readTemperature();
  if (isnan(h) || isnan(t))
  {
    Serial.println(F("Failed to read from DHT sensor!"));
    dhtState = false;
    return;
  }
  else
  {
    dhtState = true;
  }
}
void soil()
{
  soilMoistureValue = analogRead(SensorPin); // put Sensor insert into soil
  // Serial.println(soilMoistureValue);
  soilmoisturepercent = map(soilMoistureValue, AirValue, WaterValue, 0, 100);
  if (soilmoisturepercent > 100)
  {
    val = 100;
  }
  else if (soilmoisturepercent < 0)
  {
    val = 0;
  }
  else if (soilmoisturepercent >= 0 && soilmoisturepercent <= 100)
  {
    val = soilmoisturepercent;
  }
}
BLYNK_WRITE(V3)
{
  int light = param.asInt();
  if (light == LOW)
  {
    Serial.println("btn light active off");
    LightState = 0;
  }
  if (light == HIGH)
  {
    Serial.println("btn light active on");
    LightState = 1;
  }
}

BLYNK_WRITE(V4)
{
  int pump = param.asInt();
  if (pump == LOW)
  {
    Serial.println("btn pump active off");
    PumpState = 0;
  }
  if (pump == HIGH)
  {
    Serial.println("btn pump active on");
    PumpState = 1;
  }
}

void blynkscreen()
{
  // แสดงอุณหภูมิ
  Blynk.virtualWrite(V0, t);
  // แสดงความชื้น
  Blynk.virtualWrite(V1, h);
  // ความชื้นในดิน
  Blynk.virtualWrite(V2, val);
}

void Process()
{
  if (LightState == 1)
  {
    digitalWrite(Relay1, LOW);
  }
  else if (LightState < 1)
  {
    digitalWrite(Relay1, HIGH);
  }

  if (PumpState == 1)
  {
    digitalWrite(Relay2, LOW);
  }
  else if (PumpState < 1)
  {
    digitalWrite(Relay2, HIGH);
  }
}

void BtnProcess()
{
  btnLigthState = digitalRead(btnLigth);
  btnPumpState = digitalRead(btnPump);

  if (btnLigthState == HIGH && LightState == 1)
  {
    Blynk.virtualWrite(V3, 0);
    LightState = 0;
    Serial.println("Light OFF");
    delay(1000);
  }
  else if (btnLigthState == HIGH && LightState == 0)
  {
    Blynk.virtualWrite(V3, 1);
    LightState = 1;
    Serial.println("Light ON");
    delay(1000);
  }
  if (btnPumpState == HIGH && PumpState == 1)
  {
    Blynk.virtualWrite(V4, 0);
    PumpState = 0;
    Serial.println("Pump OFF");
    delay(1000);
  }
  else if (btnPumpState == HIGH && PumpState == 0)
  {
    Blynk.virtualWrite(V4, 1);
    PumpState = 1;
    Serial.println("Pump ON");
    delay(1000);
  }
}

void setFontsize1()
{
  display.setFont(ArialMT_Plain_10);
  display.drawString(20, 30, String(t) + " C");
  display.drawString(85, 30, String(val) + " %");
}
void setFontsize2()
{
  display.setFont(Dialog_plain_8);
  display.drawString(0, 5, "Humidity: " + String(h) + " %");
  if (dhtState)
  {
    display.drawString(80, 5, "DHT: OK");
  }
  else
  {
    display.drawString(80, 5, "DHT: Error");
  }
  // light state
  if (LightState == 1)
  {
    display.drawString(23, 50, "ON");
    digitalWrite(Relay1, LOW);
  }
  else if (LightState < 1)
  {
    display.drawString(23, 50, "OFF");
    digitalWrite(Relay1, HIGH);
  }
  // pump state
  if (PumpState == 1)
  {
    display.drawString(60, 50, "ON");
    digitalWrite(Relay2, LOW);
  }
  else if (PumpState < 1)
  {
    display.drawString(60, 50, "OFF");
    digitalWrite(Relay2, HIGH);
  }
  // wifi state
  if (wifiState)
  {
    display.drawString(100, 50, "ON");
  }
  else
  {
    display.drawString(100, 50, "OFF");
  }
}
void MainScreen()
{
  setFontsize2();
  display.drawHorizontalLine(0, 16, 128);
  display.drawXbm(0, 18, temperature_width, temperature_height, temperature_logo);
  display.drawXbm(68, 18, soil_width, soil_height, soil_logo);
  setFontsize1();
  display.drawHorizontalLine(0, 43, 128);
  display.drawXbm(5, 45, light_width, light_height, light);
  display.drawXbm(45, 45, weather_width, weather_height, weather);
  display.drawXbm(85, 45, WiFi_width, WiFi_height, WiFi_logo);
  display.display();
}

void loop()
{
  display.clear();
  dhtSensor();
  soil();
  BtnProcess();
  Process();
  blynkscreen();
  MainScreen();
  Blynk.run();
}