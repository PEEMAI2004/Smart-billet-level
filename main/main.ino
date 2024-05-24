#include "I2Cdev.h"
#include "Wire.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

// MPU6050 at 60 (0x3C)
Adafruit_MPU6050 mpu;
int16_t ax, ay, az;

// OLED screen at 104 (0x68)
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SH1106G OLED(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Pins
// i2c pin is D1 for SCL and D2 for SDA
const int buzzerPin = D0;
// int buttonState = 0;

// Configs
int diff = 10;

bool isLevelAxis(int val) {
  if (val - diff <= 0 || val + diff >= 45) {
    return true;
  }
  return false;
}

bool isLevel(int valx, int valy, int valz) {
  valx = valx % 45;
  valy = valy % 45;
  valz = valz % 45;

  if (isLevelAxis(valx) && isLevelAxis(valy) && isLevelAxis(valz)) {
    Serial.println(" Level");
    tone(buzzerPin, 4000);
    return true;
  } else {
    Serial.println(" Not Level");
    tone(buzzerPin, 6000);
    return false;
  }
}

void DisplayLandscape(int valx, int valy, int valz, bool flip) {
  if (flip) {
    OLED.setRotation(2);
  } else {
    OLED.setRotation(0);
  }
  OLED.clearDisplay();
  OLED.setTextColor(SH110X_WHITE);
  OLED.setCursor(0, 0);
  OLED.setTextSize(2);
  OLED.print("x = ");
  OLED.println(valx);
  OLED.print("y = ");
  OLED.println(valy);
  OLED.print("z = ");
  OLED.println(valz);
  OLED.println(isLevel(valx, valy, valz) ? "Level!" : "Not Level");

  OLED.display();
}

void DisplayPortrait(int valx, int valy, int valz, bool flip) {
  if (flip) {
    OLED.setRotation(1);
  } else {
    OLED.setRotation(3);
  }
  OLED.clearDisplay();
  OLED.setTextColor(SH110X_WHITE);
  OLED.setCursor(0, 0);
  OLED.setTextSize(2);
  OLED.print("x ");
  OLED.println(valx);
  OLED.println();
  OLED.print("y ");
  OLED.println(valy);
  OLED.println();
  OLED.print("z ");
  OLED.println(valz);
  OLED.println();
  OLED.println(isLevel(valx, valy, valz) ? "Level" : "Not\nLevel");

  OLED.display();
}

void setup() {
  Wire.begin();
  Serial.begin(115200);
  pinMode(buzzerPin, OUTPUT);

  Serial.println("Initialize MPU");
  Serial.println(mpu.begin() ? "MPU: Connected" : "MPU: Connection failed");
  Serial.println("Initialize OLED");
  Serial.println(OLED.begin() ? "Screen: Connected" : "Screen: Connection failed");
  
  // myButton.begin();
}

void loop() {
  // get raw value
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  float roll  = atan2(a.acceleration.y, a.acceleration.z) * 180.0 / PI;
  float pitch = atan2(-a.acceleration.x, sqrt(a.acceleration.y * a.acceleration.y + a.acceleration.z * a.acceleration.z)) * 180.0 / PI;
  float yaw   = atan2(a.acceleration.z, a.acceleration.x) * 180.0 / PI;

  float valx = roll;
  float valy = pitch;
  float valz = yaw;

  Serial.print(" axis x = ");
  Serial.print(valx);
  Serial.print(" axis y = ");
  Serial.print(valy);
  Serial.print(" axis z = ");
  Serial.print(valz);

  int orentation = 0;

  switch (orentation) {
    case 0:
      DisplayLandscape(valx, valy, valz, false);
      break;
    case 1:
      DisplayPortrait(valx, valy, valz, false);
      break;
      case 2:
      DisplayLandscape(valx, valy, valz, true);
      break;
    case 3:
      DisplayPortrait(valx, valy, valz, true);
      break; 
  }

  delay(1000);
}