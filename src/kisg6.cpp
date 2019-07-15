
// 
#include "kisg6.h"
#include "Arduino.h"
//#include "Servo.h"
kisg6::kisg6()
{
    //INPUTS
    photosens=2;
    hallsens=3;
    trigger=4;
    switch_input=5;
    butt1=10;
    butt2=11;
    // OUTPUTS 
    bboxled=7;
    led1=12;
    led2=13;
}
void kisg6::pinSetup(){

    pinMode(photosens, INPUT);
    pinMode(hallsens, INPUT);
    pinMode(trigger, INPUT);
    pinMode(switch_input, INPUT);
    pinMode(butt1, INPUT);
    pinMode(butt2, INPUT);
    //OUTPUTS PINS einstellen
    pinMode(bboxled, OUTPUT);
    pinMode(led1, OUTPUT);
    pinMode(led2, OUTPUT);

    }
    
