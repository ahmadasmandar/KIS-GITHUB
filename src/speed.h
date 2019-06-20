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
    boolean secureMotion(int val11 , int val21, boolean STrto);
    void checkStop(int val31, int val41);
    float getThetavalues(int val51, int val_time, float angul_accel, char val_char);
    // float calculateTime(float accelaration, float winkelgeschwindigkeit, float inittheta);
};

#endif