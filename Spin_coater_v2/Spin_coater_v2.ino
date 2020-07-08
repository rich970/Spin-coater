/*
  LCD pins:
 * LCD RS pin to digital pin 12
 * LCD Enable pin to digital pin 11
 * LCD D4 pin to digital pin 5
 * LCD D5 pin to digital pin 4
 * LCD D6 pin to digital pin 3
 * LCD D7 pin to digital pin 2
 * LCD R/W pin to ground
 * LCD VSS pin to ground
 * LCD VCC pin to 5V
 * 10K resistor:
 * ends to +5V and ground
 * wiper to LCD VO pin (pin 3)

 // Joystick pin numbers
SW_pin = 8; // digital pin connected to switch output
X_pin = 1; // analog pin connected to X output
Y_pin = 2; // analog pin connected to Y output

//ESC pin number
ESC_pin = 9
 */
 
#include <SpinCoater.h>

int Prog;
int N_programs = 4;  //Number of programs in the loop() +1 for the custom program 
int cv;
int ca;
int ct;
int start_v = 12;
bool Status;

//Set pins:
int ESC_pin = 9;
int SW_pin = 8;
int X_pin = 1;
int Y_pin = 2;

SpinCoater SC(ESC_pin,SW_pin,X_pin,Y_pin); //Create a SpinCoater object. Arguments: (ESC_pin, SW_pin, X_pin, Y_pin)

void setup() {   //This runs once when the Arduino is turned on.
  Serial.begin(9600);  //for debugging 
  SC.initSC();   //Initialise the spin-coater, attaching the pins which were defined when tbe SC object was created.
  delay(1000);   //Wait 1s to give the user an opportunity to push the switch to go into callibration mode
  if (digitalRead(SW_pin) < 0.85) {    //go into calibration mode if joystick switch is held down at startup.
    SC.calibrate();
  }
}

void loop() {
  Prog = SC.select_program(N_programs); //Run the 'select program' method. Return an integer for selected program

  if (Prog == 1) {
     if (SC.confirm_selection(Prog) == true) {    //Ask user to confirm or go back. Return 'true' if confirmed or 'false' if back is selected
        SC.start_motor(start_v);         //Start the motor spinning at minimum throttle. Args (start speed)
        SC.ramp_up(start_v,18,2);  //Ramp up the motor speed. Arguments (start speed, end speed, acceleration [speed unit/s])
        SC.hold_speed(18,5);  //Hold the motor at a set speed for a defined time Args (speed, time [s])
        SC.ramp_down(18,0,2); // Ramp down the motor speed. Will set the speed to zero when finished ramping down. Args (start speed, end speed, deceleration )
     }
  }  
  
  if (Prog == 2) {
     if (SC.confirm_selection(Prog) == true) {    //Ask user to confirm or go back. Return 'true' if confirmed or 'false' if back is selected
        SC.start_motor(start_v);         //Start the motor spinning at minimum throttle. Args (start speed)
        SC.ramp_up(start_v,18,2);  //Ramp up the motor speed. Arguments (start speed, end speed, acceleration [speed unit/s])
        SC.hold_speed(18,5);  //Hold the motor at a set speed for a defined time Args (speed, time [s])
        SC.ramp_down(18,0,2); // Ramp down the motor speed. Will set the speed to zero when finished ramping down. Args (start speed, end speed, deceleration )
     }
  }  
  
    if (Prog == 3) {
     if (SC.confirm_selection(Prog) == true) {    //Ask user to confirm or go back. Return 'true' if confirmed or 'false' if back is selected
        SC.start_motor(start_v);
        SC.ramp_down(start_v,0,1);
     }
  }  
  
  // The last program is always the 'custom' program which can be edited on-the-fly
  if (Prog == N_programs) {             
    cv = SC.user_value("Target speed: "); // Take user input value as target speed
    ca = SC.user_value("Acceleration: ");  // Take user input value as acceleration (used for decelaration on ramp down as well)
    ct = SC.user_value("Hold time: ");  // Take user input value as hold time
      if (SC.LCDprint("Confirm or back") == true) { 
      SC.start_motor(start_v);
      SC.ramp_up(start_v,cv,ca); 
      SC.hold_speed(cv,ct);
      SC.ramp_down(cv,0,ca);
     }
  }
}
