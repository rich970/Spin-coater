/*
 SpinCoater.h - Library of functions for a budget
 spin coater running a quadcopter brushless motor

 Created by R. Rowan-Robinson, 8th April 2020
 */

#ifndef SpinCoater_h
#define SpinCoater_h

#include "Arduino.h"
#include "Servo.h"
#include "LiquidCrystal.h"


class SpinCoater
{
  public:
    SpinCoater(int ESC_pin, int SW_pin, int X_pin, int Y_pin);
    void initSC();
    void calibrate();
    int select_program(int N_programs);
    int user_value(String str);
    bool LCDprint(String str);
    bool confirm_selection(int count);
    bool ramp_up(int startspeed, int endspeed, float acc);
    bool ramp_down(int startspeed, int endspeed, float acc);
    bool hold_speed(int setspeed, int holdtime);
    bool start_motor(int startspeed);
    
   
  private:
    int _ESC_pin;
    int _SW_pin;
    int _X_pin;
    int _Y_pin;
    int _val = 1;
    int _count = 1;
    const float _rampstep = 0.25; 

};

#endif