//******** Written by Ahmad Asmanda 03.05.2019 **************
// 
#ifndef KIS6_H
#define KISG6_H
#include "Arduino.h"
#include "Servo.h"

 class kisg6
 {
 private:
     /* data */
 public:
     kisg6();
    int photosens,
         hallsens,
         trigger,
         switch_input,
         bboxled,
         servopin,
         butt1,butt2,
         led1,led2;
    Servo servo;
    
    void pinSetup();
    void serialLogger();
    void Cservo(int deg);
    void deatachServo();
    void attachServo();
    void Demo();
    // void Seth();
    // void Setl();
    // int Get();


 };

 #endif
