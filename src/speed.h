#ifndef SPEED_H
#define SPEED_H
#include "Arduino.h"
#define PI 3.1415926535897932384626433832795
class speed
{

public:
    speed();
    float photoSpeed(int delta_photo);
    float hallSpeed(int delta_hall);
    float photoRst(int photo_sec, int delta_photo);
    float hallRst(int hall_sec,int delta_hall);
    float totalPhotoTime(int delta_pho_time);
    float totalHallTime(int delta_hal_time);
    void shootBall(float TIME_INTERRUPT_HALL_IN,uint16_t TARGET_TIME,uint16_t TIME_WINDOW_HALL_IN,uint16_t TIME_DELTA_HALL,uint8_t HALL_SECTION);
    boolean secureMotion(int val11 , int val21, boolean STrto);
    void checkStop(int val31, int val41);

};


#endif