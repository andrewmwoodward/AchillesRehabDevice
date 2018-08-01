#include "IMU.h"
#include "bluetooth.h"
#include "Insole.h"
#include "FMG.h"

// pin definitions
const int MPU2 = 0x69, MPU1=0x68;
const int batteryPin = A3;

const int vibrationPin = 11;

// keeps track of steps
int steps = 0;

// hard coded maximums for vibration check
float angleMax = 100;
float rollMax = 50;
int frontMax = 20;
int backMax = 30;
int FMGMax = 20;


void setup() {
  Serial.begin(9600);
  Serial.println("***** Starting setup *****");
  pinMode(batteryPin, INPUT_PULLUP);
  pinMode(vibrationPin, OUTPUT);
  bluetoothSetup();
  IMUsetup(MPU1, MPU2);
  InsoleSetup();
  FMGsetup();
  Serial.println("***** Finished setup *****");
}

void loop() {
 
  //Serial.print("DATA: ");
  
  int16_t accelX, accelY, accelZ, gyroX, gyroY, gyroZ;
  float gForceX, gForceY, gForceZ,rotX, rotY, rotZ;
  float pitch1, pitch2, roll1, roll2;
  int frontKg, backKg;
  
  getMpuValue(MPU1, &accelX, &accelY, &accelZ, &gyroX, &gyroY, &gyroZ);
  computeForceRot(&gForceX, &gForceY, &gForceZ, &rotX, &rotY, &rotZ, accelX, accelY, accelZ, gyroX, gyroY, gyroZ);
  // reversed due to orientation of IMU in insole
  roll1 = convertToDegrees(calculatePitch(-gForceX*100, gForceY*100, gForceZ*100), gForceZ);
  pitch1 = convertToDegreesR(calculateRoll(-gForceX*100, gForceY*100, gForceZ*100), gForceZ);
  // slight adjustment
  roll1 = roll1 + 5.29;
  pitch1 = pitch1 + 6.85;
  //printIMU(MPU1, gForceX, gForceY, gForceZ, rotX, rotY, rotZ, accelX, accelY, accelZ, gyroX, gyroY, gyroZ, pitch1,roll1);
  
  getMpuValue(MPU2, &accelX, &accelY, &accelZ, &gyroX, &gyroY, &gyroZ);
  computeForceRot(&gForceX, &gForceY, &gForceZ, &rotX, &rotY, &rotZ, accelX, accelY, accelZ, gyroX, gyroY, gyroZ);
  pitch2 = convertToDegrees(calculatePitch(gForceX*100, gForceY*100, gForceZ*100), gForceZ);
  roll2 = convertToDegrees(calculateRoll(gForceX*100, gForceY*100, gForceZ*100), gForceZ);
  // slight adjustment
  pitch2 = pitch2 - 1.56;
  //printIMU(MPU2, gForceX, gForceY, gForceZ, rotX, rotY, rotZ, accelX, accelY, accelZ, gyroX, gyroY, gyroZ, pitch2,roll2);

  float angle = pitch2-pitch1;
  //Serial.print("\tPitch ");
  //Serial.print(angle);

  float rollAngle = roll2-roll1;
  //Serial.print("\tRoll ");
  //Serial.print(rollAngle);


  
  calculateForceKg(&frontKg ,&backKg);
  //Serial.print("\tFront ");
  //Serial.print(frontKg);
  //Serial.print("\t");
  //Serial.print("\tBack ");
  //Serial.print(backKg);

  // if TOFS is detected and some weight is applied to heel then step has occured
  if(TOFSdetection(gyroY) && backKg > 10){
     steps++;
  }
  //Serial.print("\tSteps ");
  //Serial.print(steps);
  
  int batterySensor = analogRead(batteryPin);
  batterySensor *= 2;
  batterySensor *= 3.3;
  batterySensor /= 1024;
  //Serial.print("\tBattery ");
  //Serial.print(batterySensor);
  
  float FMG = updateFMG();

  Serial.println("");
  uploadData(angle,rollAngle, frontKg, backKg, FMG, batterySensor, steps);

  /*
  // if any of the maximums are exceeded then turn on vibration
  if(FMG > FMGMax || frontKg > frontMax || backKg > backMax || angle > angleMax || rollAngle > rollMax){
    digitalWrite(vibrationPin, HIGH);
  }
  */
  delay(50);
  
  // turn off vibration if it was on
  //digitalWrite(vibrationPin, LOW);
}

