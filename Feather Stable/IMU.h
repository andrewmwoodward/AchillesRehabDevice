
/*
Wiring:
  MPU 1 - SCL - A5 on nano or SCL on feather
          SDA - A4 on nano or SDA on feather
          AD0 - 5V
  
  MPU 2 - SCL - A5 on nano or SCL on feather
          SDA - A4 on nano or SCL on feather
*/


#include <Wire.h>
#include <math.h>


float vel = 0;
float currAccel;
float prevAccel;

boolean gyroFlag = false;
float gyroThreshold = 4000;
unsigned long msSinceThresh = millis();

// not currently working will need to adjust for drift and fix units
float velocityCalc(int16_t accelX, int16_t accelY, int16_t accelZ){

  float bias = 16900;

  currAccel = sqrt( (accelX*accelX) + (accelY*accelY) + (accelZ*accelZ) ) - bias;
  
  vel += currAccel * (0.001);

  prevAccel = currAccel;

  Serial.print(" ");
  Serial.print("vel ");
  Serial.print(vel);

  return vel;
}


// sets up the transmission of data from the two IMUs
void IMUsetup(int MPUone,int MPUtwo){
  Serial.println("Starting IMU setup");

  Wire.begin();
  Wire.beginTransmission(MPUone);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);     // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);

  Wire.begin();
  Wire.beginTransmission(MPUtwo);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);     // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
  
  Serial.println("Finished IMU setup");
}


// reads the data from the IMU and stores the raw accel and gyro data into the given pointer
void getMpuValue(int MPU, int16_t* accelX, int16_t* accelY, int16_t* accelZ, int16_t* gyroX, int16_t* gyroY, int16_t* gyroZ){
  Wire.beginTransmission(MPU);
  Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU,14,true);  // request a total of 14 registers
  *accelX= Wire.read()<<8|Wire.read();  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)
  *accelY=Wire.read()<<8|Wire.read();  // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  *accelZ=Wire.read()<<8|Wire.read();  // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
  float Tmp=Wire.read()<<8|Wire.read();  // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
  *gyroX=Wire.read()<<8|Wire.read();  // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
  *gyroY=Wire.read()<<8|Wire.read();  // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
  *gyroZ=Wire.read()<<8|Wire.read();  // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)
}


// computes the gravitational force in x,y,z direction and the rotational velocity x,y,z values
void computeForceRot(float* gForceX, float* gForceY, float* gForceZ, float* rotX, float* rotY, float* rotZ, int16_t accelX, int16_t accelY, int16_t accelZ, int16_t gyroX, int16_t gyroY, int16_t gyroZ){
  *gForceX = accelX / 16384.0;
  *gForceY = accelY / 16384.0; 
  *gForceZ = accelZ / 16384.0;
  *rotX = gyroX / 131.0;
  *rotY = gyroY / 131.0; 
  *rotZ = gyroZ / 131.0;
}


// prints the raw data and calculated values of the given IMU
void printIMU(int MPU, float gForceX, float gForceY, float gForceZ, float rotX, float rotY, float rotZ, int16_t accelX, int16_t accelY, int16_t accelZ, int16_t gyroX, int16_t gyroY, int16_t gyroZ, float pitch, float roll){
  Serial.print("gyro: ");
  //Serial.print(",");
  Serial.print(gyroX);
  Serial.print("\t");
  //Serial.print(",");
  Serial.print(gyroY);
  Serial.print("\t");
  //Serial.print(",");
  Serial.print(gyroZ);
  Serial.print("\tAcc: ");
  //Serial.print(",");
  Serial.print(accelX);
  Serial.print("\t");
  //Serial.print(",");
  Serial.print(accelY);
  Serial.print("\t");
  //Serial.print(",");
  Serial.print(accelZ);
  Serial.print("\tPitch: ");
  //Serial.print(",");
  Serial.print(pitch);
  Serial.print("\tRoll: ");
  //Serial.print(",");
  Serial.print(roll);
}

// computes the pitch of the given IMU
float calculatePitch(int x, int y, int z){
  float result = atan( x / sqrt(y*y + z*z));
  return result;
}


// computes the roll of the given IMU
float calculateRoll(int x, int y, int z){
  float result = atan( y / sqrt(x*x + z*z));
  return result;  
}

//converts the pitch value to degrees
float convertToDegrees(float pitch, float z){
  float result = 60.056*abs(pitch) - 5.2898;
  // quadrant 2 (90-180)
  if(z<0 && pitch>0){
    result = 180 - result;
  }
  // quadrant 3 (180-270)
  else if(z<0 && pitch<0){
    result = -180+result;
  }
  // quadrant 4 (270-360)
  else if(z>0 && pitch <0){
    result = 0 - result;
  }
  return result;
}


//converts the pitch value to degrees
float convertToDegreesR(float roll, float x){
  float result = 58.974*abs(roll) + 3.1284;
  // quadrant 2 (90-180)
  if(x<0 && roll>0){
    result = 180 - result;
  }
  // quadrant 3 (180-270)
  else if(x<0 && roll<0){
    result = -180+result;
  }
  // quadrant 4 (270-360)
  else if(x>0 && roll<0){
    result = 0 - result;
  }
  return result;
}

// adjusts the pitch based on the direction of the roll angle
float fixPitch(float pitch, float roll){
  if(roll < 0){
    return pitch - roll;
  }
  if(roll > 0){
    return pitch + roll;
  }
}



boolean TOFSdetection(float gyro){
  if(gyro > gyroThreshold && gyroFlag == false){
    msSinceThresh = millis();
    gyroFlag = true;
  }
  else{
    if(gyro < 0 && gyroFlag == true && (millis() - msSinceThresh) < 1000){
      gyroFlag = false;
      return true;
    }
    if(gyroFlag == true && (millis() - msSinceThresh) > 1000){
      gyroFlag = false;
    }
  }
  return false;
}


