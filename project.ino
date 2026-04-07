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

// -------- BLYNK --------
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

// -------- SETUP --------
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

Added Blynk control and auto/manual mode logic for fan and light