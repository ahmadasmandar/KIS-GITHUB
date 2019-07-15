#ifndef SPEED_H
#define SPEED_H
#include "Arduino.h"
#define PI 3.1415926535897932384626433832795
class speed
{

public:
    speed();
    // photo speed = 1000* PI/6 / delta_time rad/s 
    float photoSpeed(int delta_photo); // die wINKEL GESCHINDIGKEIT IN jeder Zeit unabhängig von der Beschleungiung
    // hall speed = 1000* PI / delta_time rad/s 
    float hallSpeed(int delta_hall);
    float photoRst(int photo_sec, int delta_photo,float accelo);// die Zeit, die der verbleibende WInkel braucht zum 0
    float hallRst(int hall_sec,int delta_hall,float accelo2);
    float totalPhotoTime(int delta_pho_time,float accelo3);
    float totalHallTime(int delta_hal_time,float accelo4);
    boolean secureMotion(int val11 , int val21, boolean STrto);
    void checkStop(uint16_t arr[10]);
    // float getThetavalues(int val51, int val_time, float angul_accel, char val_char);
    float solveTimeEquation(float a, float b,float c);
};

#endif