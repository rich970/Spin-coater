/*
 SpinCoater.h - Library of functions for a budget
 spin coater running a quadcopter brushless motor

 Created by R. Rowan-Robinson, 8th April 2020
 */
 #include "SpinCoater.h"
 
 #include <Servo.h>
 #include <LiquidCrystal.h>
 #include "Arduino.h"


Servo ESC; // create servo object to control ESC
LiquidCrystal lcd(12, 11, 5, 4, 3, 2); // initialise the library with the numbers of the interface pins

SpinCoater::SpinCoater(int ESC_pin, int SW_pin, int X_pin, int Y_pin)
 { 
    _ESC_pin = ESC_pin;
    _SW_pin = SW_pin;
    _X_pin = X_pin;
    _Y_pin = Y_pin;
 }

void SpinCoater::initSC() {
   ESC.attach(_ESC_pin,1000,2000); // Attach the electronic speed controller - ESC. (pin, min pulse width, max pulse width in microseconds)
   ESC.write(0);
   pinMode(_SW_pin, INPUT); //Set the joystick switch pin as an input 
   digitalWrite(_SW_pin, HIGH);  //Set this pin as live  
   lcd.begin(16, 2);       // set up the LCD's number of columns and rows
   Serial.begin(9600);    //This line for debugging the output on the pins
}

int SpinCoater::select_program(int _N_programs) {
    lcd.clear();  //Clear the LCD display 
    lcd.setCursor(0, 0); //Set the LCD cursor to the first line
    lcd.print("Select program:  "); 
    while (digitalRead(_SW_pin) > 0.85) {   //Run this loop while the joystick push switch is raised
        if (analogRead(_X_pin) < 250) {
           if (_count == _N_programs) {    //If count goes above the number of program, reset the count number.
              _count = 1; }
           else {   
              _count += 1; }           
        }
        if (analogRead(_X_pin) > 750) {
           if (_count == 1) {             //If count goes below zero, jump to the highest program number
              _count = _N_programs; }
           else {   
           _count -= 1; }
        }
        lcd.setCursor(0, 1);
        lcd.print("Program: ");
        if (_count == _N_programs) {      //When count reaches the highest program number, display 'Custom' instead
          lcd.print("Custom");            // of the program number. Last program is reserved as a custom user defined program.
        }
        else {  
          lcd.print(_count);
          lcd.print("     "); //Need to add some blank space to overwrite 'Custom' otherwise end up with '1ustom' etc.
        }
        delay(250);
     }
     return _count;
}

bool SpinCoater::confirm_selection(int _count){   //method to allow the user to confirm choices or go back
   lcd.clear();
   lcd.setCursor(0, 0);
   lcd.print("Selected =  ");
   lcd.print(_count);   
   lcd.setCursor(0, 1);
   lcd.print("Confirm or back");
   while (true) {
      delay(250);
      if (digitalRead(_SW_pin) < 0.85) {   //Push the joystick switch down to confirm
           return true;
      }
      if (analogRead(_Y_pin) > 750) {     //Push the joystick to the left to go back (dependent on the mounting
           return false;                  // orientation of the joystick)
      }
   }
}

bool SpinCoater::ramp_up(int _startspeed, int _endspeed, float _acc) {  //Method for ramping up the spin coater speed to a target speed.
   lcd.clear();
   lcd.setCursor(0, 0);
   lcd.print("Ramping up");

   float _N_steps = (_endspeed -_startspeed)/_rampstep;   //Calculate the number of steps to the target speed.
   for (int i = 0; i <= _N_steps; i++) {
      ESC.write(_startspeed + i*_rampstep);
      Serial.println(pulseIn(9, HIGH));  //for debugging PWM pin
      lcd.setCursor(0, 1);
      lcd.print(_startspeed + i*_rampstep);
      delay(_rampstep*(1000/_acc));
   }
   return true;
}

bool SpinCoater::ramp_down(int _startspeed, int _endspeed, float _acc) {  //Method for ramping down the spin coater speed to a target speed.
   lcd.clear();
   lcd.setCursor(0, 0);
   lcd.print("Ramping down");

   float _N_steps = (_startspeed -_endspeed)/_rampstep;
   for (int i = 0; i <= _N_steps; i++) {
      ESC.write(_startspeed - i*_rampstep);
      Serial.println(pulseIn(9, HIGH));  //for debugging PWM pin
      lcd.setCursor(0, 1);
      lcd.print(_startspeed - i*_rampstep);
      delay(_rampstep*(1000/_acc));
   }
   ESC.write(0); //When finished, set speed to zero, just to be sure ;)
   lcd.clear();
   lcd.setCursor(0, 0);
   lcd.print("Program finished");
   delay(3000);  
   return true;
}

bool SpinCoater::hold_speed(int _setspeed, int _holdtime) {  //Method to hold the motor at some fixed speed for a defined time.
   lcd.clear();
   lcd.setCursor(0, 0);
   lcd.print("Holding speed"); 
   ESC.write(_setspeed);
   lcd.setCursor(0, 1);
   lcd.print(_setspeed);
   delay(_holdtime*1000);
   return true;
}

bool SpinCoater::start_motor(int startspeed) {    //Method to start the motor running at the minimum speed. Runs for at least 2s, but will run continuously
   lcd.clear();                     //unless another command is sent to the motor.
   lcd.setCursor(0, 0);
   lcd.print("Starting"); 
   ESC.write(startspeed);
   delay(2000);
   Serial.println(pulseIn(9, HIGH));  //for debugging PWM pin
   return true;
}

int SpinCoater::user_value(String str) {   //Method to allow the user to input values using the joystick and LCD. 
    lcd.clear();  //Clear the LCD display 
    lcd.setCursor(0, 0); //Set the LCD cursor to the first line
    lcd.print(str); 
    _val = 0;
    delay(1000);   
    //note that for joystick min = 0, centre = 512, max = 1024
    while (digitalRead(_SW_pin) > 0.85) {  
        if (analogRead(_X_pin) < 250) {  
              _val += 1; 
              if (analogRead(_X_pin) < 5) {
                delay(20); }  //If the joystick is pushed fully forwards, the delay will be just 20ms, and hence will cycle 
              else {          // up through the values quicker. 
                delay(500); }
                }           
        if (analogRead(_X_pin) > 750) {  //If the joystick is pushed fully backwards, the delay will be just 20ms, and hence will cycle 
              _val -= 1;                // down through the values quicker. 
              if (analogRead(_X_pin) > 1013) {
                delay(20); }
              else {
                delay(500); }
              }   
        lcd.setCursor(0, 1);
        if (_val < 0) {
          _val = 0;
        }
        lcd.print(_val);
        lcd.print("     "); //Need to add some blank space to overwrite 'Custom' otherwise end up with '1ustom' etc.
        
     }
     return _val; 
}

bool SpinCoater::LCDprint(String str) {  //Method to allow strings to be printed directly to the LCD. 
   lcd.clear();
   lcd.setCursor(0, 0);
   lcd.print(str); 
   delay(1000);
   while (digitalRead(_SW_pin) > 0.85) { 
       delay(250);
       if (analogRead(_Y_pin) > 750) {
       return false;
      }
   }
   return true;
}

void SpinCoater::calibrate() {  //Method for calibrating max and min throttle values
   int mx = user_value("Set max throttle:");
   int mn = user_value("Set min throttle:");
   delay(1000);
   if (LCDprint("Confirm or back") == true) { 
      delay(1000);
      ESC.write(mx);  //output the full throttle position
      Serial.println(pulseIn(9, HIGH));  //for debugging PWM pin       
      LCDprint("Max throttle set");
      ESC.write(mn);   //output the miniumum throttle position
      Serial.println(pulseIn(9, HIGH));  //for debugging PWM pin
      LCDprint("Min throttle set");
      delay(1000);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Calibration");
      lcd.setCursor(0,1 );
      lcd.print("complete");
     }
}
