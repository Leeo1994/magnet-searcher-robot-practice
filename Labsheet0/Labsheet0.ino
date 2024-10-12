//Github link: https://github.com/paulodowd/EMATM0054_53/tree/main?tab=readme-ov-file#assessment-2-projects

//================================ [INCLUDE] =========================================

#include "Motors.h"         // Labsheet 1
#include "PID.h"            // Labsheet 1 - Advanced
#include "LineSensors.h"    // Labsheet 2
//#include "Magnetometer.h" // Labsheet 3
#include "Kinematics.h"     // Labsheet 4
#include "Encoders.h"     //For encoder counts [doesnt need modifying]

//================================= [DEFINE] =========================================

#define BUZZER_PIN 6

//============================ [CLASS INSTANCES] ======================================

//Instance of a class to operate motors.
Motors_c motors;

//Instance of a class to operate the line
// sensors to measure the surface reflectance.
LineSensors_c line_sensors;
//int maximum[NUM_SENSORS] {};
//int minimum[NUM_SENSORS] {}; <------------------------I MADE THIS - DELETE.

//Instance of a class to operate the magnetometer.
//Magnetometer_c magnetometer;

//Instance of a class to estimate the pose of the robot.
//Kinematics_c pose;

//============================ [GLOBAL VARIABLES] ====================================

unsigned long beepStop_ms; //the time to stop the beep
unsigned long led_blink_ts; //blink time stamp
unsigned long led_blink_ms; //how frequently to blink?(ms)(interval)
float left;
float right;
bool motorStopped; //flag if motors stopped

//================================= [STATES] =========================================

#define STATE_CALIBRATION      0
#define STATE_START_MOVING     1
#define STATE_LINE_ENCOUNTERED 2
#define STATE_SEARCH_MAGNET    3
#define STATE_MAGNET_FOUND     4 //kinematics now or later?
#define STATE_KINEMATICS       5 //fix location and encapsulate magnet
#define STATE_RETURN_TO_START  6 //return to start
int state;

void setup() {

  //=============================== [INITIATION] =======================================

  pinMode( BUZZER_PIN, OUTPUT ); //Setup up the buzzer as an output

  motors.initialise(); //Setup motors.

  line_sensors.initialiseForADC(); //Setup the line sensors

  setupEncoder0(); //Activate encoder sensors to measure the wheel rotation. Encoder counts counted automatically.
  setupEncoder1();

  //pose.initialise(0, 0, 0); //Setup the pose (kinematics).

  Serial.begin(9600); //Set baud

  delay(2000);

  Serial.println("*** READY ***");

  //=============================== [CALIBRATION] ======================================

  state = STATE_CALIBRATION;
  Serial.print(state);

  if (state == STATE_CALIBRATION) {
    line_sensors.set_minmax(); //Sets initial values to min:1023 ; max:0
    left = -23.0;
    right = 24.0;
    motors.setPWM(left, right);  //Start spining in place

    //Start sensor calibration
    for (int i = 0; i <= 200; i++) {  //i for iteration //Spin for a certain number of iterations --> Total delay time[ms](4000) = Number of iterations(200) * Delay per iteration(20) <-----------[DOESNT COMPLETE A 360 SPIN]
      line_sensors.calib(); //Perform a single calibration step
      delay(20); //Small delay to allow sensor reading

      //<--------------------------------------------------------------------------Nonvollatile memory, print from loop
      //Serial.println("iteration+1");

      if (i >= 200) {
        left = 0.0;
        right = 0.0;
        motors.setPWM(left, right); //Stop motors
        //line_sensors.calibrating = false;
        state = STATE_START_MOVING; //state calibration ended.
        Serial.print(state);
      }
    }

    //Calibration is done!
    Serial.println("Calibration complete");

    line_sensors.print_minmax(); //print maximum and mimum

    line_sensors.calcScaling(); //calculate scaling (range)

  }
} //End of Setup()

void loop() {
  //
  // left = -40.0; // I SWAPPED WHEEL ASSIGNMENT - CHECK IF POSSIBLE.
  //  right = 40.0; //RIGHT WHEEL A BIT WEAKER THAN LEFT!!!
  //  //send power to motors.
  //  motors.setPWM(left, right); //(L,R)


  line_sensors.calcCalibratedADC(); //Calculates the calibrated values for b&w
  line_sensors.printCalibrated(); //
  //  Serial.print(line_sensors.calibrated[0], 4);
  //  Serial.print(",");
  //  Serial.print(line_sensors.calibrated[1], 4);
  //  Serial.print(",");
  //  Serial.print(line_sensors.calibrated[2], 4);
  //  Serial.print(",");
  //  Serial.print(line_sensors.calibrated[3], 4);
  //  Serial.print(",");
  //  Serial.print(line_sensors.calibrated[4], 4);
  //  Serial.print(",");
  //  delay(20);




  //=============================== [START_MOVING] =====================================


  int state = STATE_START_MOVING;

  if (state == STATE_START_MOVING) {
    Serial.println ("State: Start moving");
    motors.setPWM(25, 25); //Move Forward

    //SENSE: Check sensors
    //if true, we have hit the boundary box
    for ( int sensor = 0; sensor < NUM_SENSORS; sensor++ ) {
    line_sensors.isOnLine(line_sensors.calibrated[sensor]);

      if (line_sensors.isOnLine(line_sensors.calibrated[sensor]) == true) {
        //Serial.println(sensor);//    <----------------------------------------------------------------------- 2nd print - make sure the print within isOnLine works.
        
        //int state = STATE_LINE_ENCOUNTERED;
        
        unsigned long online_ts = millis();

        motors.setPWM(-25, -25);
//
        delay(150);
        
 //online_ts = millis(); //update after reverse
        //while (millis() - online_ts <= 150) { //was originally if -- try if again.
          //motors.setPWM(-25, -25);
        //}          
        
        //    <-----------------------------------------------------------------------------------------------Currently vibrating back and forth once encountering a line

        //Trigger a turn of n milliseconds
        //setTurn( ?? ?? ) //   <-------------------------------------------------------------------------- FIX FUNCTION - DOESNT EXIST {set random value for wheels?}

       
         motors.setPWM(0, 0);         
         delay(50);
        
        
        motors.setPWM(-25, 25); //(L:R) Turn Left        
        delay(random(30, 70));

        //state = STATE_START_MOVING;

      }
    }
  //else

    //state = STATE_START_MOVING;

}//End of if state == state_start_moving


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
//=============================== [SEARCH_MAGNET] ====================================

//================================ [MAGNET_FOUND] ====================================


//============================== [RETURN_TO_START] ===================================

//}


//================================= [FUNCTIONS] ======================================


//void setTurn(left,right,duration){
//time mechanism???
//setPWM(left,right)}
//=================================== [WHEELS] =======================================

//================================== [SENSORS] =======================================
