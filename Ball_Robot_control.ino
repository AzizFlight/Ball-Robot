//See BareMinimum example for a list of library functions

//Includes required to use Roboclaw library
#include <SoftwareSerial.h>
#include "RoboClaw.h"
#include <Wire.h>
#include <LSM6.h>



//See limitations of Arduino SoftwareSerial
SoftwareSerial serial(15,14);	
RoboClaw roboclaw(&serial,10000);

#define address 0x80



int mode = 8;
int motor1Channel = 4; 
int motor2Channel = 6;
int direction = 0;

LSM6 imu;

char report[80];


void setup() {
  //Open roboclaw serial ports
  roboclaw.begin(38400);
  Serial.begin(9600);
  Serial.println("Aziz");
  pinMode(motor1Channel, INPUT);
  pinMode(motor2Channel, INPUT);
  pinMode(mode, INPUT);
  

  if (!imu.init())
  {
    Serial.println("Failed to detect and initialize IMU!");
    while (1);
  }
  imu.enableDefault();
}

void loop() {
  Serial.println("Aziz");

  int modeValue = pulseIn(mode, HIGH, 25000);

  int currentMode = map(modeValue, 989, 1987, 0, 20);

  Serial.println("Current mode is: ");
  Serial.println(modeValue);

  Serial.println(currentMode);



  if( currentMode < 5){

    roboclaw.ForwardM1(address, 0); //start Motor1 forward at half speed
    roboclaw.ForwardM2(address, 0); 

  }
  else{
    if( currentMode > 5 and currentMode < 15){
      manual();
    }
    else if(currentMode > 15){
      automatic();
    }
  }
  delay(10);



}


void manual(){

  int ch1Value = pulseIn(motor1Channel,HIGH,25000);
  int ch2Value = pulseIn(motor2Channel,HIGH,25000);



 
  int Motor1Roll = map(ch1Value, 990, 1980, 0, 127);//values for roll from 1000 to 2000
  int Motor2Roll = map(ch2Value, 990, 1980, 0, 127);//values for roll from 1000 to 2000
  Serial.println("Speeds");
  Serial.println(Motor1Roll);
  Serial.println(Motor2Roll);





  roboclaw.ForwardBackwardM1(address, Motor1Roll); //start Motor1 forward at half speed
  roboclaw.ForwardBackwardM2(address,Motor2Roll); //start Motor2 backward at half speed

  delay(10);
}



void automatic(){

  Serial.println("current mode is auto");

  randomSeed(analogRead(0));
  int speed = random(0, 127);
  int timeToWaitInSec = random(1, 4);

  double angle = accelerometer();

  Serial.println("angle is");
  Serial.println(angle);

  if(angle > 30){
    if(direction == 0){
      roboclaw.ForwardM1(address, 100);
      roboclaw.ForwardM2(address, 100);

      direction = 1;


    }
    else{
      roboclaw.BackwardM1(address, 100);
      roboclaw.BackwardM2(address, 100);
      direction = 0;      
  }
  }

  else{
    if(direction == 0){
      roboclaw.ForwardM1(address, speed);
      roboclaw.ForwardM2(address, 127-speed);
    }

    else{
      roboclaw.BackwardM1(address, speed);
      roboclaw.BackwardM2(address, 127-speed);
    }
  }

  


  delay(timeToWaitInSec * 1000);


}




double accelerometer(){

  imu.read();

  double X_out = imu.a.x/256; // X-axis value
//For a range of +-2g, we need to divide the raw values by 256, according to the datasheet
  double Y_out = imu.a.y/256; // Y-axis value

  double Z_out = imu.a.z/256; // Z-axis value


  // Calculate Roll and Pitch (rotation around X-axis, rotation around Y-axis)
  double roll = atan(Y_out / sqrt(pow(X_out, 2) + pow(Z_out, 2))) * 180 / PI;

  return roll;
}
