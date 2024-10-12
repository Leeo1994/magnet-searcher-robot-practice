/************************************
  ,        .       .           .      ,
  |        |       |           |     '|
  |    ,-: |-. ,-. |-. ,-. ,-. |-     |
  |    | | | | `-. | | |-' |-' |      |
  `--' `-` `-' `-' ' ' `-' `-' `-'    '
*************************************/

// this #ifndef stops this file
// from being included more than
// once by the compiler.
#ifndef _MOTORS_H
#define _MOTORS_H

// Pin definitions.  By using #define we can
// switch the number here, and everywhere the
// text appears (i.e. L_PWM) it will be
// replaced.
#define L_PWM 10
#define L_DIR 16 //according to sheet 16 //PIN ASSIGNMENTNS https://www.pololu.com/docs/0J83/5.9
#define R_PWM 9
#define R_DIR 15 //according to sheet 15 //TRY SWAP LEFT<->RIGHT SO THAT - positive=FWD negative=REV

// It is a good idea to limit the maximum power
// sent to the motors. Using #define means we
// can set this value just once here, and it
// can be used in many places in the code below.
#define MAX_PWM 180.0

// defining forward/backwards for negative/positive values
#define FWD LOW //OPPOSITE - ASK PROF.
#define REV HIGH //OPPOSITE - ASK PROF. --> (Workaround to positive PWM leading to reverse when FWD defined as HIGH)

// Class to operate the motors.
class Motors_c {

  public:

    // Constructor, must exist.
    Motors_c() {
      // Leave empty. Ensure initialise() is called
      // instead.
    }

    // Use this function to initialise the pins that
    // will control the motors, and decide what first
    // value they should have.
    void initialise() {

      // Uncomment and replace the ???? with the correct
      // values, e.g  pinMode( <pin>, <INPUT or OUTPUT> );
      pinMode(L_PWM , INPUT); //*initial value 0
      pinMode(L_DIR , OUTPUT); //*initial value 0
      pinMode(R_PWM , INPUT); //initial value 0
      pinMode(R_DIR , OUTPUT); //initial value 0

      // Uncomment and replace ???? with the correct
      // values, e.g. digitalWrite( <pin>, <HIGH or LOW> );
      // Which pins will be either HIGH or LOW? What
      // purpose do they serve? <- direction of the robot (wheels spinning back or forth)
      digitalWrite(R_DIR, LOW); //Starting in place
      digitalWrite(L_DIR, LOW); //HIGH-FORWARD

      // Uncomment and replace ???? with the correct
      // values, e.g. analogWrite( <pin>, < 0 : 255 > );
      // Which pins will take a value in range [0:255]? <-PWM pins - gas.
      // What purpose do they serve?
      analogWrite(R_PWM, 0); //Start in place
      analogWrite(L_PWM, 0); //Range(0:225)

    } // End of initialise()

                           // This function will be used to send a power value
                           // to the motors.
                           //
                           // The power sent to the motors is created by the
                           // analogWrite() function, which is producing PWM.
                           // analogWrite() is intended to use a range between
                           // [0:255].
                           //
                           // This function takes two input arguments: "left_pwr"
                           // and "right_pwr", (pwr = power) and they are of the
                           // type float. A float might be a value like 0.01, or
                           // -150.6
    void setPWM( float left_pwr, float right_pwr ) {
      // What should happen if the request for left_pwr
      // is less than 0? Recall, how are these motors
      // operated in terms of the pins used?
      if ( left_pwr < 0 ) {
        digitalWrite(L_DIR, REV); //NEGATIVE = FORWARD?? ---ASK WHY NEGATIVE IS FORWARD
      } else {
        digitalWrite(L_DIR, FWD); //POSITIVE = REVERSE
      }

      // What should happen if the request for right_pwr
      // is less than 0? Recall, how are these motors
      // operated in terms of the pins used?
      if ( right_pwr < 0 ) {
        digitalWrite(R_DIR, REV); //NEGATIVE = FORWARD
      }
      else {
        digitalWrite(R_DIR, FWD); //POSITIVE = REVERSE
      }


      // analogWrite() requires a value in the range
      // [0:255], and note this is positive only!
      // Write some code here to take the value of
      // left_pwr and ensure it is:
      // - positive only
      // - within the range 0 to 255
      // Note, we have used the sign to determine
      // the direction - so we don't care about the
      // sign of the value any more.
      left_pwr = abs(left_pwr); //absolute value = positive
      if (left_pwr > 225) {
        left_pwr = 225;
      } //make 225 if >225

      // analogWrite() requires a value in the range
      // [0:255], and note this is positive only!
      // Write some code here to take the value of
      // right_pwr and ensure it is:
      // - positive only
      // - within the range 0 to 255
      // Note, we have used the sign to determine
      // the direction - so we don't care about the
      // sign of the value any more.
      right_pwr = abs(right_pwr); //absolute value = positive
      if (right_pwr > 225) {
        right_pwr = 225;
      } //make 225 if >225

      // Lastly, write the requested power value to
      // the motors as a PWM signal.
      // Without the code above, this is likely to
      // produce very unexpected behaviours.
      analogWrite(L_PWM, left_pwr);
      analogWrite(R_PWM, right_pwr);

      // Done!
      return;

    } // End of setPWM()

//Motor calibration rotation
//void motorCalib() {
  //loop() {
    //left = 20.0; // I SWAPPED WHEEL ASSIGNMENT - CHECK IF POSSIBLE.
    //right = -20.0; //RIGHT WHEEL A BIT WEAKER THAN LEFT!!!
    //motors.setPWM(left, right); //(L:R) Callibration begins!
  //}
//}


}; // End of Motors_c class definition.



#endif
