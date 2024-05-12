#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"
#include "Wire.h"

MPU6050 mpu;
int16_t ax, ay, az;
int16_t gx, gy, gz;
int valx, valy, valz;

// Pins
int pin11 = 5, pin10 = 4  ; // SCL, SCA
int buzzerPin = 14;

// Configs
int diff = 10;

bool isLevelAxis(int val){
  if(val - diff <= 0 || val + diff >= 45){
    return true;
  }
  return false;
}

void isLevel(int valx, int valy, int valz){
  valx = valx%45;
  valy = valy%45;
  valz = valz%45;

  if(isLevelAxis(valx) && isLevelAxis(valy) && isLevelAxis(valz)){
    Serial.println(" Level");
    tone(buzzerPin, 4000);
  } else {
    Serial.println(" Not Level");
    tone(buzzerPin, 6000);
  }
}

void setup()
{
    Wire.begin();
    Serial.begin(115200);
    pinMode(buzzerPin, OUTPUT);

    Serial.println("Initialize MPU");
    mpu.initialize();
    Serial.println(mpu.testConnection() ? "Connected" : "Connection failed");
}
void loop()
{
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

    isLevel(valx, valy, valz);

    delay(1000);
}