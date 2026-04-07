#define BLYNK_TEMPLATE_ID ""
#define BLYNK_TEMPLATE_NAME ""
#define BLYNK_AUTH_TOKEN ""

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <DHT.h>

char ssid[] = "Neeraj";
char pass[] = "1234567890";

// Pins
#define FAN_RELAY 5
#define LIGHT_RELAY 18
#define BUTTON_FAN 4
#define BUTTON_LIGHT 13
#define PIR_PIN 27
#define DHTPIN 14
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

// States
bool autoMode = true;
bool lightAuto = true;

int fanState = HIGH;
bool lightState = false;

int lastFanButton = HIGH;
int lastLightButton = HIGH;

// Timer
unsigned long lastCheck = 0;
unsigned long interval = 2000;

// PIR HOLD
unsigned long lastMotionTime = 0;
int motionDelay = 10000;

//  BLYNK 
BLYNK_WRITE(V0) {
  if (!autoMode) {
    fanState = param.asInt() ? LOW : HIGH;
    digitalWrite(FAN_RELAY, fanState);
  }
}

BLYNK_WRITE(V1) {
  autoMode = param.asInt();
}

BLYNK_WRITE(V2) {
  if (!lightAuto) {
    lightState = param.asInt();
    digitalWrite(LIGHT_RELAY, lightState ? LOW : HIGH);
  }
}

BLYNK_WRITE(V3) {
  lightAuto = param.asInt();
}

// SETUP 
void setup() {
  Serial.begin(115200);

  pinMode(FAN_RELAY, OUTPUT);
  pinMode(LIGHT_RELAY, OUTPUT);
  pinMode(BUTTON_FAN, INPUT_PULLUP);
  pinMode(BUTTON_LIGHT, INPUT_PULLUP);
  pinMode(PIR_PIN, INPUT);

  digitalWrite(FAN_RELAY, fanState);
  digitalWrite(LIGHT_RELAY, HIGH);

  dht.begin();

  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) delay(500);

  Blynk.config(BLYNK_AUTH_TOKEN);
  Blynk.connect();
}

// LOOP 
void loop() {
  Blynk.run();

  //  FAN BUTTON
  int fanBtn = digitalRead(BUTTON_FAN);
  if (fanBtn == LOW && lastFanButton == HIGH) {
    autoMode = false;
    fanState = !fanState;

    digitalWrite(FAN_RELAY, fanState);

    Blynk.virtualWrite(V0, fanState == LOW ? 1 : 0);
    Blynk.virtualWrite(V1, 0);

    delay(300);
  }
  lastFanButton = fanBtn;

  // LIGHT BUTTON
  int lightBtn = digitalRead(BUTTON_LIGHT);
  if (lightBtn == LOW && lastLightButton == HIGH) {

    lightAuto = false;

    lightState = !lightState;

    digitalWrite(LIGHT_RELAY, lightState ? LOW : HIGH);
    Blynk.virtualWrite(V2, lightState);
    Blynk.virtualWrite(V3, 0);

    delay(300);
  }
  lastLightButton = lightBtn;

  //  PIR 
  bool motion = digitalRead(PIR_PIN);

  if (motion) {
    lastMotionTime = millis();
  }

  bool roomActive = (millis() - lastMotionTime < motionDelay);

 
  Blynk.virtualWrite(V6, roomActive);

  // LIGHT AUTO 
  if (lightAuto) {
    lightState = roomActive;
    digitalWrite(LIGHT_RELAY, lightState ? LOW : HIGH);
  }

  //  SENSOR 
  if (millis() - lastCheck > interval) {
    lastCheck = millis();

    float temp = dht.readTemperature();
    float hum = dht.readHumidity();

    Blynk.virtualWrite(V4, temp);
    Blynk.virtualWrite(V5, hum);

    //  FAN AUTO 
    if (autoMode) {
      if (!roomActive) {
        fanState = HIGH;
      }
      else {
        if (temp >= 30 || hum >= 70) {
          fanState = LOW;
        }
        else if (temp <= 27 && hum <= 60) {
          fanState = HIGH;
        }
      }

      digitalWrite(FAN_RELAY, fanState);
      Blynk.virtualWrite(V0, fanState == LOW ? 1 : 0);
    }
  }
}