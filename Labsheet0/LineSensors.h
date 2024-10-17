/***************************************
  ,        .       .           .     ,-.
  |        |       |           |        )
  |    ,-: |-. ,-. |-. ,-. ,-. |-      /
  |    | | | | `-. | | |-' |-' |      /
  `--' `-` `-' `-' ' ' `-' `-' `-'   '--'
****************************************/

// this #ifndef stops this file
// from being included mored than
// once by the compiler.
#ifndef _LINESENSORS_H
#define _LINESENSORS_H

// We will use all 5 line sensors (DN1 - 5)
// and so define a constant here, rather than
// type '5' in lots of places.
#define NUM_SENSORS 5

// Pin definitions
// This time, we will use an array to store the
// pin definitions.  This is a bit like a list.
// This way, we can either loop through the
// list automatically, or we can ask for a pin
// by indexing, e.g. sensor_pins[0] is A11,
// sensors_pins[1] is A0.
const int sensor_pins[ NUM_SENSORS ] = { A11, A0, A2, A3, A4 };

// This is the pin used to turn on the infra-
// red LEDs.
#define EMIT_PIN   11


// Class to operate the linesensors.
class LineSensors_c {

  public:

    // Store your readings into this array.
    // You can then access these readings elsewhere
    // by using the syntax line_sensors.readings[n];
    // Where n is a value [0:4]
    float readings[ NUM_SENSORS ];

    // Variables to store calibration constants.
    // Make use of these as a part of the exercises
    // in labsheet 2.
    float minimum[NUM_SENSORS]; //UPDATE ALL THREE TO CONTAIN {dn1, dn2, dn3, dn4, dn5}?
    float maximum[NUM_SENSORS];
    float scaling[NUM_SENSORS];

    // Variable to store the calculated calibrated
    // (corrected) readings. Needs to be updated via
    // a function call, which is completed in
    // labsheet 2.
    float calibrated[ NUM_SENSORS ]; //UPDATE TO CONTAIN {dn1, dn2, dn3, dn4, dn5}?

    // Constructor, must exist.
    LineSensor_c() {
      // leave this empty
    }

    // Refer to Labsheet 2: Approach 1
    // Fix areas marked ????
    // Use this function to setup the pins required
    // to perform an read of the line sensors using
    // the ADC.
    void initialiseForADC() {

      //configure the central line sensor pin as input
      //pinMode(A2, INPUT);

      // Ensure that the IR LEDs are on
      // for line sensing
      pinMode( EMIT_PIN, OUTPUT );
      digitalWrite( EMIT_PIN, HIGH );

      // Configure the line sensor pins
      for ( int sensor = 0; sensor < NUM_SENSORS; sensor++ ) {
        pinMode( sensor_pins[sensor], INPUT_PULLUP );
      }

      //configure the serial port
      Serial.begin(9600);//        <---------------------------------------------------------- why?

    } // End of initialiseForADC()


    // Refer to Labsheet 2: Approach 1
    // Fix areas marked ????
    // This function is as simple as using a call to
    // analogRead()
    void readSensorsADC() {
      int dn1;
      int dn2;
      int dn3;
      int dn4;
      int dn5;

      dn1 = analogRead(A11); //bugged when using (12);
      dn2 = analogRead(A0);
      dn3 = analogRead(A2);
      dn4 = analogRead(A3);
      dn5 = analogRead(A4);

      float recorded_values[] {dn1, dn2, dn3, dn4, dn5}; //Recorded values array
      for (int sensor = 0; sensor < NUM_SENSORS; sensor++) {
        readings[sensor] = recorded_values[sensor]; //Updates readings with newest readings
      }

      // First, initialise the pins.
      // You need to complete this function (above).
      //initialiseForADC();

//      Serial.println("Readings:");//    <--------------------------------------------------------------- PRINT READINGS inactive - makes calibrated work in plotter.
//      for ( int sensor = 0; sensor < NUM_SENSORS; sensor++ ) {
//        Serial.println(readings[sensor]);
//        if (sensor < NUM_SENSORS - 1) {
//          Serial.println("\n");
//        }
//      }
      //Blue sensor A11/12 - weird.

    } // End of readSensorsADC()

    void set_minmax() {
      for (int sensor = 0; sensor < NUM_SENSORS; sensor++) {
        maximum[sensor] = 0; //feeding values
        minimum[sensor] = 1023; //feeding values
      }
    }

    //Calibration funct
    void calib() {

      //bool calibrating = true;

      //while (calibrating == true) {

        readSensorsADC(); //Get latest readings

        for (int sensor = 0; sensor < NUM_SENSORS; sensor++) {

          if (readings[sensor] > maximum[sensor]) {
            maximum[sensor] = readings[sensor]; //update maximum array
          }

          if (readings[sensor] < minimum[sensor]) {
            minimum[sensor] = readings[sensor]; //update minimum array
          }
        }

        //delay(10); //lastReading = millis();
    }

    //printing minimum and maximum
    void print_minmax() {
      Serial.println("\n");
      Serial.println("Max Values:");
      for ( int sensor = 0; sensor < NUM_SENSORS; sensor++ ) {
        Serial.println(maximum[sensor]);
        if (sensor < NUM_SENSORS - 1) {
          Serial.println(",");
        }
        else
          Serial.println("\n");
      }
      Serial.println("Min Values:");
      for ( int sensor = 0; sensor < NUM_SENSORS; sensor++ ) {
        Serial.println(minimum[sensor]);
        if (sensor < NUM_SENSORS - 1) {
          Serial.println(",");
        }
        else
          Serial.println("\n");
      }
    }

    //Calc scaling factor
    void calcScaling() {
      for (int sensor = 0; sensor < NUM_SENSORS; sensor++) {
        scaling[sensor] = (maximum[sensor] - minimum[sensor]); //scaling = range = sensing delta for each sensor
      }
      Serial.print("Scaling(max-min):");
      for (int sensor = 0; sensor < NUM_SENSORS; sensor++) {
        Serial.print("\n");
        Serial.print(scaling[sensor]);
      }
      Serial.print("\n");
    }


    // Use this function to apply the calibration values
    // that were captured in your calibration routine.
    // Therefore, you will need to write a calibration
    // routine (see Labsheet 2)
    void calcCalibratedADC() {//
      
      readSensorsADC(); //Get latest readings (raw values)
      
      //Serial.println("Calibrated:");
      
      // Apply calibration values, store in calibrated[]
      for ( int sensor = 0; sensor < NUM_SENSORS; sensor++ ) {
        
        calibrated[sensor] = ((readings[sensor] - minimum[sensor]) / scaling[sensor]);
        //All readings begin from 0
        //Then dividing by maximum total range -> normalizing reading to a value between 0 to 1.
      }
    }

      void printCalibrated(){
      for ( int sensor = 0; sensor < NUM_SENSORS; sensor++ ){
      Serial.print(calibrated[sensor], 4);
       if (sensor < NUM_SENSORS - 1) {
          Serial.println(",");
        }
        else
          Serial.println("\n");
      }
    } // End of printCalibrated()

bool isOnLine(){//     <-------------------------------------------------------------------------reached this pt. When calibrated[n] = 1 --> sensor dn[n] on line. SET THRESHOLD TO 0.8 WORKS FOR ALL SENSORS NOW
     // Check sensor 0 and sensor 4 with a specific threshold
    if (calibrated[0] >= 0.9 || calibrated[4] >= 0.9) { // Higher threshold for less responsive sensors
        return true; // Return true if either sensor 0 or sensor 4 detects a line
    }

    // Loop through the remaining sensors (1 to 3)
    for (int sensor = 1; sensor < NUM_SENSORS - 1; sensor++) {
        if (calibrated[sensor] >= 0.8) {
            return true; // Return true if any of the other sensors detect a line
        }
    }

    return false; // Return false if no sensors are on the line
}

//     <---------------------------------------------------------------------------------------------------------Sensor 1 fucked. --> need to check why

    // Part of the Advanced Exercises for Labsheet 2
    void initialiseForDigital() {

      // Ensure that the IR LEDs are on
      // for line sensing
      pinMode( EMIT_PIN, OUTPUT );
      digitalWrite( EMIT_PIN, HIGH );

    } // End of initialiseForDigital()

    // Part of the Advanced Exercises for Labsheet 2
    void readSensorsDigital() {
      //  ???
    } // End of readSensorsDigital()

}; // End of LineSensor_c class defintion



#endif
