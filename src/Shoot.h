#ifndef SHOOT_H
#define SHOOT_H
#include "Arduino.h"
class Shoot
{
private:
    /* data */
public:
    void fireBall(float time_resto,uint8_t section, float total_time , uint16_t window,uint16_t target_time);
    void motorIntil();
    void shootManuel();
    void shootManuel_dropall();
    void sPrint(String to_pr_c, float valo, String einheit);
};
#endif
