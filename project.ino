#include <DHT.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// -------------------- PIN Definitions --------------------
const int pirPin = 2;           // PIR Sensor (motion)
const int relayPin = 7;         // Relay to control light
const int beamPin = 4;          // Break-beam sensor
const int DHTPIN = 5;           // DHT11 sensor pin
#define DHTTYPE DHT11

// -------------------- Objects --------------------
LiquidCrystal_I2C lcd(0x27, 16, 2);
DHT dht(DHTPIN, DHTTYPE);

// -------------------- Settings --------------------
const bool RELAY_ACTIVE_LOW = true;
const unsigned long autoOffSeconds = 5;  // Light auto OFF delay

// -------------------- Variables --------------------
unsigned long lastMotionMillis = 0;
bool lightState = false;
int peopleCount = 0;
int lastBeamState = HIGH;

// ----------------------------------------------------------
void setup() {
  Serial.begin(9600);

  pinMode(pirPin, INPUT);
  pinMode(relayPin, OUTPUT);
  pinMode(beamPin, INPUT_PULLUP);

  setRelay(false);

  lcd.init();
  lcd.backlight();

  dht.begin();

  delay(1000);
  lcd.clear();
  lcd.print("Smart Room Sys");
  delay(1500);
}

// ----------------------------------------------------------
void loop() {
  handlePIR();
  handleBreakBeam();
  displayData();
}

// ----------------------------------------------------------
// PIR SENSOR + LIGHT CONTROL
void handlePIR() {
  int motion = digitalRead(pirPin);

  if (motion == HIGH) {
    lastMotionMillis = millis();

    if (!lightState) {
      setRelay(true);
      lightState = true;
      Serial.println("Motion -> Light ON");
    }
  } 
  else {
    if (lightState) {
      unsigned long elapsed = (millis() - lastMotionMillis) / 1000;
      if (elapsed >= autoOffSeconds) {
        setRelay(false);
        lightState = false;
        Serial.println("No Motion -> Light OFF");
      }
    }
  }
}

// ----------------------------------------------------------
// BREAK-BEAM COUNTER
void handleBreakBeam() {
  int beamState = digitalRead(beamPin);

  if (beamState == LOW && lastBeamState == HIGH) {
    peopleCount++;
    Serial.print("Person Count: ");
    Serial.println(peopleCount);
  }

  lastBeamState = beamState;
}

// ----------------------------------------------------------
// LCD DISPLAY
void displayData() {
  float hum = dht.readHumidity();
  float temp = dht.readTemperature();

  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("T:");
  lcd.print((int)temp);
  lcd.print("C  H:");
  lcd.print((int)hum);
  lcd.print("%");

  lcd.setCursor(0, 1);
  lcd.print("Cnt:");
  lcd.print(peopleCount);
  lcd.print(" Light:");
  lcd.print(lightState ? "ON" : "OFF");

  delay(500);
}

// ----------------------------------------------------------
// RELAY CONTROL (Active LOW/HIGH Compatible)
void setRelay(bool on) {
  if (RELAY_ACTIVE_LOW)
    digitalWrite(relayPin, on ? LOW : HIGH);
  else
    digitalWrite(relayPin, on ? HIGH : LOW);
}