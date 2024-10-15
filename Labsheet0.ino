//Github link: https://github.com/paulodowd/EMATM0054_53/tree/main?tab=readme-ov-file#assessment-2-projects

//================================ [INCLUDE] =========================================

#include "Motors.h"         // Labsheet 1
#include "PID.h"            // Labsheet 1 - Advanced - no need to use pid
#include "LineSensors.h"    // Labsheet 2
#include "Magnetometer.h" // Labsheet 3
#include "Kinematics.h"     // Labsheet 4
#include "Encoders.h"     //For encoder counts [doesnt need modifying]


//================================= [DEFINITIONS DECLARATIONS] =========================================

#define BUZZER_PIN 6
#define turn_threshold 0.1
void checkMag();
void setRev(int ms);
bool checkTurn();
void shortTurn(float target_angle);
void setTurn(float target_angle);
void updatePose();
void faceHome();

//============================ [CLASS INSTANCES] ======================================

//Instance of a class to operate motors.
Motors_c motors;

//Instance of a class to operate the line
// sensors to measure the surface reflectance.
LineSensors_c line_sensors;

//Instance of a class to operate the magnetometer.
Magnetometer_c magnetometer;

//Instance of a class to operate LIS3MDL
LIS3MDL mag;

//Instance of a class to estimate the pose of the robot.
Kinematics_c pose;

//============================ [GLOBAL VARIABLES] ====================================

unsigned long beepStop_ms; //the time to stop the beep
bool isReversing = false; //flag for reversing
bool isTurning = false; //flag for turning
unsigned long online_ts; // online time stamp
unsigned long led_blink_ms; //how frequently to blink?(ms)(interval)
float left;
float right;
bool motorStopped; //flag if motors stopped
float magnitude;
float target_angle;
unsigned long current_millis = millis();
unsigned long last_millis = 0;
bool onLine;

//================================= [STATES] =========================================

#define STATE_CALIBRATION      0
#define STATE_LEAVE_START_AREA 1
#define STATE_SEARCH_MAGNET    2
#define STATE_MAGNET_FOUND     3 //kinematics now or later? //fix location and encapsulate magnet
#define STATE_RETURN_TO_START  4 //return to start
#define STATE_DEBUG            6
int state; //track active state

void setup() {

  //=============================== [INITIATION] =======================================

  pinMode( BUZZER_PIN, OUTPUT ); //Setup up the buzzer as an output

  motors.initialise(); //Setup motors.

  line_sensors.initialiseForADC(); //Setup the line sensors

  setupEncoder0(); //Activate encoder sensors to measure the wheel rotation. Encoder counts counted automatically.
  setupEncoder1();

  pose.initialise(0, 0, 0); //Setup the pose (kinematics).

  Serial.begin(9600); //Set baud

  delay(2000);

  magnetometer.initialise(); //Setup the magnetometer

  Serial.println("*** READY ***");

  //=============================== [CALIBRATION] ======================================

  state = STATE_CALIBRATION; //set to spin 720 degrees!
  Serial.print(state);

  if (state == STATE_CALIBRATION) {
    line_sensors.set_minmax(); //Sets initial line-sensor values to min: 1023 ; max: 0
    magnetometer.set_minmax(); //Sets initial magnetometer values to min: 9999 ; max: -9999 <-------------- COMPLETE
    left = -23.0;
    right = 24.0;
    motors.setPWM(left, right);  //Start spinning in place

    //Start sensor calibration
    for (int i = 0; i < 200; i++) {  //i for iteration //Spin for a certain number of iterations --> Total delay time[ms](4000) = Number of iterations(200) * Delay per iteration(20) <-----------[DOESNT COMPLETE A 360 SPIN]

      line_sensors.calib(); //Perform a single line-sensor calibration step

      delay(10);

      magnetometer.calib(); //Perform a single magnetometer calibration step

      delay(10); //Small delay to allow sensor reading

    }

    //<--------------------------------------------------------------------------Nonvollatile memory, print from loop

    //Calibration is done!
    Serial.println("Calibration complete");

    left = 0.0;
    right = 0.0;
    motors.setPWM(left, right); //Stop motors

    unsigned long motor_stop_ts = millis();

    while (millis() - motor_stop_ts < 1000) {
      //analogWrite(BUZZER_PIN, 200); //buzzer on}
    }

    line_sensors.print_minmax(); //print maximum and mimum - Line Sensors

    line_sensors.calcScaling(); //calculate scaling (range) - Line Sensors

    magnetometer.print_minmax(); //print maximum and mimum - Magnetometer x, y, z.
    //NO MAGNET:
    //MAX [X: -5778, Y: -4702, Z: -9870] <---------- WEIRD.
    //MIN [X: -5858, Y: -4784, Z: -10023]

    magnetometer.calcScaling(); //calculate scaling - Magnetometer
    //NO MAGNET:
    //Range(max-min) = [X: 80, Y: 82, Z: 153]             <-------- [MAX - MIN] CALC FINE
    //offset(mid-point) = [X: -5818, Y: -4743, Z: -9946.5]   <-------- [MIN + (RANGE / 2)] CALC FINE
    //scaling(normalized) = [X: 0.03, Y: 0.02, Z: 0.01]    <-------- [1 / (RANGE / 2)] CALC wrong presenting 0.015 as 0.02


  } //End of state == calibration

  //check if i should make an IF for the ending of calibration.

  state = STATE_SEARCH_MAGNET;

  Serial.print(state);

} //End of Setup()

void loop() {


  updatePose();


  //line_sensors.calcCalibratedADC(); //Calculates the calibrated values for b&w ---> inside search magnet?

  // line_sensors.printCalibrated();

  //magnetometer.calcCalibratedMag(); //  <----- [(READINGS - OFFSET) * SCALING] CALC

  //NO MAGNET:           <----------------------------------- Readings [X: -4808, Y: -4765, Z: 1682797] <-- Hand calculated readings. Z CRAZY
  //Calibrated Readings  <-------------------------------------------- [X: 30.30, Y: -0.44, Z: 16927.43]
  //magnetometer.printCalibrated();


  //=============================== [LEAVE_START_AREA] =====================================

  //  if (state == STATE_LEAVE_START_AREA) {
  //
  //    left = 0.0;
  //    right = 0.0;
  //    motors.setPWM(left, right); //Stop motors
  //
  //    unsigned long motor_stop_ts = millis();
  //
  //    //while (millis() - motor_stop_ts < 1000) {
  //    //analogWrite(BUZZER_PIN, 200); //buzzer on
  //    //}
  //    state = STATE_SEARCH_MAGNET;
  //
  //  }


  // left = -40.0; // I SWAPPED WHEEL ASSIGNMENT - CHECK IF POSSIBLE.
  //  right = 40.0; //RIGHT WHEEL A BIT WEAKER THAN LEFT!!!
  //  //send power to motors.
  //  motors.setPWM(left, right); //(L,R)




  //=============================== [SEARCH_MAGNET] ====================================

  //if state = STATE_SEARCH_MAGNET;

  if (state == STATE_SEARCH_MAGNET) {
    //Serial.println ("State: Start moving");

    if (!isReversing && !isTurning) {
      motors.setPWM(25, 25); //Move Forward -- when moving indifinitely it blocks the code apparently
    }

    //checks magnitude threshold - beeps when magnet near
    checkMag();

    //SENSE: Check sensors
    line_sensors.calcCalibratedADC();

    //if true, we have hit the boundary box
    //onLine = line_sensors.isOnLine();

    if (line_sensors.isOnLine() == true) { //for all sensors not just one

      online_ts = millis();

      //Reverse when online for (ms) milliseconds
      setRev(150);
      online_ts = millis(); //update after reverse

      //Trigger a turn of rand angle

      //generate target
      const int angles[] = {45, 90, 135, 180, 225, 270, 315};

      int randomIndex = random(0, 8); //random index from 0-7

      float randAngle = angles[randomIndex]; //randoms out of 45, 90, 135, 180, 225, 270, 315, 360?

      shortTurn(randAngle);

      checkTurn();

      online_ts = millis(); //update after turn
    }
  } //End of if state == STATE_SEARCH_MAGNET


  //    <-----------------------------------------------------------------------------------------------Currently vibrating back and forth once encountering a line






  //state = STATE_SEARCH_MAGNET;

  //else

  //state = STATE_SEARCH_MAGNET;




  //motors.setPWM(25, 25); //(L:R) Continue Forward
  //
  //    }
  //  }
  //}
  // PLAN: what is the robot currently doing?
  // Is the robot doing a turn operation?
  // checkTurn() will return true of false
  //    bool turn_status = checkTurn();
  //
  //    if ( turn_status == false ) { // i.e. not turning
  //
  //      // ACT: Code to drive forwards
  //
  //
  //    } else { // i.e., robot is turning
  //
  //              // ACT: robot is set to turn, so
  //              // nothing to do here currently.
  //
  //            }
  //          }
  //        }

}// End of loop()

//============================= [LINE_ENCOUNTERED] ===================================

//  if (state == STATE_LINE_ENCOUNTERED) {
//
//    Serial.println ("State: Line encountered");
//
//    unsigned long online_ts = millis();
//
//    motors.setPWM(-25, -25);
//
//    if (millis() < online_ts + 150) {
//
//      //delay(150);
//
//      //if (millis() >= (online_ts + 150)) {
//      //Currently vibrating back and forth once encountering a line
//      //Trigger a turn of n milliseconds
//      //setTurn( ?? ?? ) //   <-------------------------------------------------------------------------- FIX FUNCTION - DOESNT EXIST {set random value for wheels?}
//
//      motors.setPWM(-25, 25); //(L:R) Turn Left
//      delay(random(50, 100));
//    }
//
//  }
//}


//================================ [MAGNET_FOUND] ====================================

//record magnet coordinations
//cover magnet completely

//============================== [RETURN_TO_START] ===================================


void faceHome(){
if (state == STATE_RETURN_TO_START){
//face origin:
float thetaH = atan2(pose.y, pose.x); //returns degrees
float thetaI = (pose.theta * (180 / PI)); //transforms radians --> degrees
float faceHome = (thetaH - thetaI); //how to turn to face home

setTurn(faceHome); //turn to start
//checkTurn();
}
}

//============================ [STATE_RETURN_TO_MAG] =================================

//return to recorded magnet location


//================================= [FUNCTIONS] ======================================



//_____________________[checkMag]_____________________


//checks magnitude threshold - beeps when magnet near
void checkMag() {

  Serial.println ("magnitude:");
  magnitude = magnetometer.calcCalibratedMag(); //get newest calibrated readings + magnitude values
  // MAGNITUDE VALUES ; No magnet:(2 ~ 10) ; Magnet detected(15 ~ 300)

  if (magnetometer.detect_threshold() == true) {

    motors.setPWM(0.0, 0.0); //wheels stopped <----------- DOESNT WORK BECAUSE OF THE

    //analogWrite(BUZZER_PIN, 200); //buzzer on
  }
  else
    analogWrite(BUZZER_PIN, 0); //buzzer off

} //End of checkMag


//____________________[setRev]_________________________


//Sets reverse for (ms) milliseconds.
void setRev(int ms) {
  isReversing = true; //reversing

  //Serial.println(sensor);

  unsigned long startRev = millis(); //recorded start time
  while (millis() - startRev < ms) {
    motors.setPWM(-25, -25);
  }

  isReversing = false; //reverse complete

} //End of setRev


//______________________[checkTurn]_______________________


//check difference between current rotation and target
bool checkTurn() {
  if (abs(target_angle - pose.theta) < turn_threshold) { // (turn_threshold = 0.1)

    motors.setPWM(0, 0); //stop

    return true; //turn complete

  }
  return false; //turn not complete

} //end of checkTurn


//______________________[shortTurn]_______________________


//Trigger the shortest turn both ways -- RADIANS!
//call first then checkturn to see if it should stop

void shortTurn(float target_angle) {
  if (!isReversing) { //only turn if not reversing
    isTurning = true; //turning

    checkTurn();

    if (checkTurn == false) {

      float turn_R = (target_angle - (-1 * pose.theta)); //turning right - lower turn value
      float turn_L = (target_angle - pose.theta); //turning left - higher turn value

      if (abs(turn_R) <= abs(turn_L)) {

        //turn right
        motors.setPWM(25, -25); //(L:R) Turn Left
      }
      else

        //turn left
        motors.setPWM(-25, 25); //(L:R) Turn Left
    }
  }
  isTurning = false; //turning complete

}// End of shortTurn


//______________________[setTurn]_______________________


void setTurn(float target_angle) {
  if (!isReversing) { //only turn if not reversing
    isTurning = true; //turning

    if (checkTurn == false) {

      float current_angle = pose.theta * (180 / PI); //conversion of theta radians --> degrees

      float turn_R = (target_angle - (current_angle + 360)); //turning right
      float turn_L = (target_angle - current_angle); //turning left

      //keeping angles in the range of [-180, 180]
      if (turn_R > 180) turn_R -= 360;
      if (turn_R < -180) turn_R += 360;
      if (turn_L > 180) turn_L -= 360;
      if (turn_L < -180) turn_L += 360;


      if (abs(turn_R) <= abs(turn_L)) {

        //turn right
        motors.setPWM(25, -25); //(L:R) Turn Left
      }
      else

        //turn left
        motors.setPWM(-25, 25); //(L:R) Turn Left
    }
  }
  isTurning = false; //turning complete
  
} //End of setTurn



//______________________[updatePose]_______________________


//updates pose at a 20ms interval
void updatePose() {

  if (current_millis - last_millis >= 20) {

    // Instruct the kinematics to perform an update of the robot position recommended at a fixed interval 20ms.
    pose.update();

    last_millis = current_millis;

  }
} //End of updatePose
