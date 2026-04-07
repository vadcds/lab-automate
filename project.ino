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