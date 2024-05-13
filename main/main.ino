#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"
#include "Wire.h"
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

// MPU6050 at 60 (0x3C)
MPU6050 mpu;
int16_t ax, ay, az;
int16_t gx, gy, gz;
int valx, valy, valz;

// OLED screen at 104 (0x68)
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SH1106G OLED(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


// Pins
// i2c pin is D1 for SCL and D2 for SDA
const int buttonPin = D5;
const int buzzerPin = D0;
int buttonState = 0;

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

void setup() {
  Wire.begin();
  Serial.begin(115200);
  pinMode(buzzerPin, OUTPUT);
  pinMode(buttonPin, INPUT);

  Serial.println("Initialize MPU");
  mpu.initialize();
  Serial.println(mpu.testConnection() ? "MPU: Connected" : "MPU: Connection failed");
  Serial.println("Initialize OLED");
  Serial.println(OLED.begin() ? "Screen: Connected" : "Screen: Connection failed");
}
void loop() {
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
  valx = map(ax, -17000, 17000, 0, 179);
  valy = map(ay, -17000, 17000, 0, 179);
  valz = map(az, -17000, 17000, 0, 179);

  Serial.print(" axis x = ");
  Serial.print(valx);
  Serial.print(" axis y = ");
  Serial.print(valy);
  Serial.print(" axis z = ");
  Serial.print(valz);

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
  delay(100);
}