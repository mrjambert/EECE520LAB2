/*
  Serial-WR

  Output hello to Serial
  Read input from Serial. If there is a 'E' detected, flip the LED

  yluo

*/
#include <MPU6050.h>
#include<Wire.h>

char cmd;
int buzzcheck;
unsigned char i;
const int X_pin = 0; // analog pin connected to X output
const int Y_pin = 1; // analog pin connected to Y output
const int buzzer = 3; //digital buzzer pin
const int MPU_addr = 0x68; // I2C address of the MPU-6050
int16_t AcX1, AcX2, AcY1, AcY2, AcZ, Tmp, GyX, GyY, GyZ, GyXoff, GyYoff;
int AcXcheck, AcYcheck;

// the setup routine runs once when you press reset:
void setup() {
  Wire.begin();
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);     // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(buzzer, OUTPUT);
}

void buzz() //buzzes speaker
{
  for (i = 0; i < 80; i++)
  {
    digitalWrite(buzzer, HIGH);
    delay(1);//wait for 1ms
    digitalWrite(buzzer, LOW);
    delay(1);//wait for 1ms
  }
}
void mpu()
{
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr, 14, true); // request a total of 14 registers
  AcX1 = Wire.read() << 8 | Wire.read(); // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)
  AcY1 = Wire.read() << 8 | Wire.read(); // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  AcZ = Wire.read() << 8 | Wire.read(); // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
  Tmp = Wire.read() << 8 | Wire.read(); // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
  GyX = Wire.read() << 8 | Wire.read(); // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
  GyY = Wire.read() << 8 | Wire.read(); // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
  GyZ = Wire.read() << 8 | Wire.read(); // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)
}

void acucheck ()
{
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr, 14, true); // request a total of 14 registers
  AcX2 = Wire.read() << 8 | Wire.read(); // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)
  AcY2 = Wire.read() << 8 | Wire.read(); // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
}

void gycheck ()
{
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x43);  // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr, 14, true); // request a total of 14 registers
  GyXoff = Wire.read() << 8 | Wire.read(); // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
  GyYoff = Wire.read() << 8 | Wire.read(); // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
}
// the loop routine runs over and over again forever:
void loop() { //NOTE FOR GYRO VARIABLES, WITH HOW IT IS MOUNTED ON MY BOARD, UP ON MY BOARD IS LEFT FOR THE GYRO, RIGHT ON MY BOARD IS UP FOR THE GYRO, ETC)
  gycheck(); //gets base values of the gyroscope when it is level
  while (true)
  {
    mpu(); //gets values from gyro and accel
    acucheck(); //immediatly gets another set of accel values to compare 
    AcXcheck = AcX2 - AcX1;
    AcYcheck = AcY2 - AcY1; //constrats the two values, if the differences are big enough, then it is being shaken
    if (analogRead(Y_pin) >= 1000 || (GyX - GyXoff <= -500)) //if the joystick or gyroscope moves up, it goes up
      Serial.println('U');
    else if (analogRead(Y_pin) <= 100 || (GyX - GyXoff >= 500)) //same for down
      Serial.println('D');
    else if (analogRead(X_pin) <= 100 || (GyY - GyYoff <= -500)) //same for left
      Serial.println('L');
    else if (analogRead(X_pin) >= 1000 || (GyY - GyYoff >= 500))//same for right
      Serial.println('R');
    if (AcXcheck > 500 || AcXcheck < -500 || AcYcheck > 500 || AcYcheck < -500) //if accel difference is big enough, it doubles score
      Serial.println('Y');
    if (Serial.available() > 0) //if it gets a signal from the snake program that a point has been scored, rings a buzzer
      buzzcheck = Serial.read();
    if (buzzcheck == 'y')
      buzz();
    delay(50);
    buzzcheck = 'n';
  }
}
