#pragma config(Sensor, S2,     MS,                  sensorNone) //Magnetic Field Sensor
#pragma config(Sensor, S3,     LS,                  sensorLightActive) //Light Sensor
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

#include "drivers/HTMAG-driver.h"

task main(){
  float offset; //variables
  float Kp = 2;
  float Ki = 0.001;
  float Kd = 15;
  float Tp = 60;
  float error;
  float integral;
  float derivative;
  float lastError;
  float turn;
  float powerB;
  float powerC;
  int tooLow;
  int tooHigh;

  wait10Msec(200);
  PlaySound(soundBlip);
  tooLow = SensorValue(LS); //get black light value
  wait10Msec(200);
  tooHigh = SensorValue(LS); //get white light value
  PlaySound(soundBlip);
  offset = ((tooLow+tooHigh)/2)+2; //get light sensor average for offset
  wait10Msec(100);
  HTMAGstartCal(MS);
  
  nMotorPIDSpeedCtrl[motorB] = mtrNoReg; //turn motor B's PID off
  nMotorPIDSpeedCtrl[motorC] = mtrNoReg; //turn motor C's PID off

  while(true){ //start main PID loop
    if(HTMAGreadVal(MS) <= -20 || HTMAGreadVal(MS) >= 20){ //pauseing cycle
      ClearTimer(T1); //start timing
      while(HTMAGreadVal(MS) <= -20 || HTMAGreadVal(MS) >= 20){ //main pausing loop
        if(time100(T1) >= 40){
          StopAllTasks(); //Abort if timer gets over 4 seconds
        }
        motor[motorB] = 0; //turn motor B off
        motor[motorC] = 0; //turn motor C off
      }
    }
    error = SensorValue(LS)-offset; //calculate difference between offset and light value
    integral = integral+error; //calculate integral (remember the past)
    derivative = error-lastError; //calculate derviative (predict the future)
    turn = (Kp*error)+(Ki*integral)+(Kd*derivative); //summarize PID stuff
     powerB = Tp-turn; //calculate power for motor B
     powerC = Tp+turn; //calculate power for motor C
    motor[motorB] = -powerB; //set motor B to power B in reverse
    motor[motorC] = -powerC; //set motor C to power C in reverse
    lastError = error; //update memory
    wait1Msec(5);
  }
}
