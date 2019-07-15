
#ifndef KISG6_H
#define KISG6_H
#include "Arduino.h"
#include "Servo.h"

class kisg6
{
private:
    /* data */
public:
    kisg6();
    uint8_t photosens,
        hallsens,
        trigger,
        switch_input,
        bboxled,
        servopin,
        butt1, butt2,
        led1, led2;
    Servo servo;

    void pinSetup();
};

#endif
