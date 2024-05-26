#include "I2Cdev.h"
#include "Wire.h"
#include "display.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

int orentation = 2;
int buttonstate;
int level = 90;

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
int diff = 1;

bool isLevelAxis(int val) {
  val = abs(val);
  if (val - diff <= 0 || val + diff >= 45) {
    return true;
  }
  return false;
}

bool isLevel(int roll, int pitch, int yall) {
  yall = yall % level;

  if (isLevelAxis(yall)) {
    // Serial.println(" Level");
    return true;
  } else {
    // Serial.println(" Not Level");
    return false;
  }
}

void DisplayLandscape(float roll, float pitch, float yall, bool flip) {
  if (flip) {
    OLED.setRotation(2);
  } else {
    OLED.setRotation(0);
  }
  float minV = 3.0;
  float maxV = 4.2;
  float voltage = analogRead(A0) * (3.30 / 1023.00) * 5;
  int percentage = 100 * (voltage - minV) / (maxV - minV);
  if (percentage >= 100) {
    percentage = 100;
  }
  OLED.clearDisplay();
  OLED.setTextColor(SH110X_WHITE);
  OLED.setCursor(0, 0);
  OLED.setTextSize(1);
  OLED.print(percentage);
  OLED.println("%\n");
  OLED.setTextSize(2);
  OLED.println(yall);

  bool levelStatus = isLevel(roll, pitch, yall);
  OLED.println(levelStatus ? "Level!" : "Not Level");

  // Sound buzzer based on level status
  if (levelStatus) {
    tone(buzzerPin, 4000);  // High tone when level
  } else {
    tone(buzzerPin, 1000);  // Low tone when not level
  }

  OLED.display();
}

void DisplayPortrait(float roll, float pitch, float yall, bool flip) {
  if (flip) {
    OLED.setRotation(1);
  } else {
    OLED.setRotation(3);
  }
  float minV = 3.0;
  float maxV = 4.2;
  float voltage = analogRead(A0) * (3.30 / 1023.00) * 5;
  int percentage = 100 * (voltage - minV) / (maxV - minV);
  if (percentage >= 100) {
    percentage = 100;
  }
  OLED.clearDisplay();
  OLED.setTextColor(SH110X_WHITE);
  OLED.setCursor(0, 0);
  OLED.setTextSize(1);
  OLED.println();
  OLED.print(percentage);
  OLED.println("%\n ");
  OLED.setTextSize(2);
  int integer = static_cast<int>(yall);
  int decimal = abs((yall - integer) * 100);
  OLED.println(integer);
  OLED.print(".");
  OLED.println(decimal);

  bool levelStatus = isLevel(roll, pitch, yall);
  OLED.println(levelStatus ? "\n\nLevel" : "\nNot\nLevel");

  // Sound buzzer based on level status
  if (levelStatus) {
    tone(buzzerPin, 4000);  // High tone when level
  } else {
    tone(buzzerPin, 1000);  // Low tone when not level
  }

  OLED.display();
}

void Display(float roll, float pitch, float yall, int orentation) {

  switch (orentation) {
    case 0:
      DisplayLandscape(roll, pitch, yall, false);
      break;
    case 1:
      DisplayPortrait(roll, pitch, yall, false);
      break;
    case 2:
      DisplayLandscape(roll, pitch, yall, true);
      break;
    case 3:
      DisplayPortrait(roll, pitch, yall, true);
      break;
  }
}

void setup() {
  Wire.begin();
  Serial.begin(115200);
  pinMode(buzzerPin, OUTPUT);

  Serial.println("Initialize MPU");
  Serial.println(mpu.begin() ? "MPU: Connected" : "MPU: Connection failed");
  Serial.println("Initialize OLED");
  Serial.println(OLED.begin() ? "Screen: Connected" : "Screen: Connection failed");
}

// Analog input
int analogInput() {
  int analogValue = analogRead(A0);
  float voltage = analogValue * (3.30 / 1023.00);

  if (voltage >= 3.0) {
    // Serial.println("Button 4");
    return 4;
  } else if (voltage >= 2.5) {
    // Serial.println("Button 3");
    return 3;
  } else if (voltage >= 2.0) {
    // Serial.println("Button 2");
    return 2;
  } else if (voltage >= 1.5) {
    // Serial.println("Button 1");
    return 1;
  }
  return 0;
}

int buttonnput() {
  // do {
  int buttonRead = analogInput();
  if (buttonRead = 0) {
    buttonstate = buttonRead;
    // break;
  } else if (buttonstate != buttonRead) {
    buttonstate = buttonRead;
    Serial.println("buttonstate = " + buttonstate);
    // break;
  }
  delay(100);
  // } while (1);
  return buttonstate;
}

void loop() {
  // get raw value
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  float roll = atan2(a.acceleration.y, a.acceleration.z) * 180.0 / PI;
  float pitch = atan2(-a.acceleration.x, sqrt(a.acceleration.y * a.acceleration.y + a.acceleration.z * a.acceleration.z)) * 180.0 / PI;
  float yaw = atan2(a.acceleration.z, a.acceleration.x) * 180.0 / PI;

  // buttoninput();

  Serial.println(analogRead(A0) * 3.30 / 1023.00);

  // Serial.print(" axis x = ");
  // Serial.print(valx);
  // Serial.print(" axis y = ");
  // Serial.print(valy);
  // Serial.print(" axis z = ");
  // Serial.print(valz);

  if (analogInput() == 4) {
    orentation += 1;
    delay(1000);
  } else if (analogInput() == 3) {
    if (diff >= 10) {
      diff = 10;
    } else {
      diff += 1;
    }
    delay(100);
    OLED.setCursor(0, 0);
    OLED.clearDisplay();
    OLED.setTextColor(SH110X_WHITE);
    OLED.setCursor(0, 0);
    OLED.setTextSize(3);
    OLED.print("Offset\n+- ");
    OLED.println(diff);
    OLED.display();
    delay(1000);
  } else if (analogInput() == 2) {
    if (diff <= 0) {
      diff = 0;
    } else {
      diff = diff - 1;
    }
    delay(100);
    OLED.setCursor(0, 0);
    OLED.clearDisplay();
    OLED.setTextColor(SH110X_WHITE);
    OLED.setCursor(0, 0);
    OLED.setTextSize(3);
    OLED.print("Offset\n+- ");
    OLED.println(diff);
    OLED.display();
    delay(1000);
  } else if (analogInput() == 1) {
    if (level == 90) {
      level = 45;
    } else if (level == 45) {
      level = 90;
    }
    OLED.setCursor(0, 0);
    OLED.clearDisplay();
    OLED.setTextColor(SH110X_WHITE);
    OLED.setCursor(0, 0);
    OLED.setTextSize(3);
    OLED.println("Mode ");
    OLED.print(level);
    OLED.println(" Deg");
    OLED.display();
    delay(1000);
  }

  Display(roll, pitch, roll, orentation % 4);

  delay(100);
}