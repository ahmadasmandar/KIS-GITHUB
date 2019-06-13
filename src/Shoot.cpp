#include "Arduino.h"
#include "Servo.h"
#include "debug.h"
#include "Shoot.h"
debug debshoot;
Servo shhot_motor;
float delay_time_shoot;
int angel=20;
int motor_delay=100;
void Shoot::shootManuel()
{
    shhot_motor.write(angel);
    delay(motor_delay);
    shhot_motor.write(0);
    debshoot.sPrint("the ball is shotted", 0 , "   ");

}
void Shoot::motorIntil()
{
    shhot_motor.attach(9);
    //the initial angel is very important
    shhot_motor.write(0);
}

void Shoot::fireBall(uint16_t delta_hoder,float time_resto,uint8_t section, float total_time , uint16_t window,uint16_t target_time)
{

        if (time_resto < target_time && time_resto > target_time - window)
        {
          shhot_motor.write(angel);
          delay(motor_delay);
          shhot_motor.write(0);
          debshoot.sPrint("NORMAL_IF  if work time is ", time_resto, "ms");
          debshoot.sPrint("NORMAL_IF  ", section, "ms");
        //   debshoot.sPrint("NORMAL_IF photo_speed is ", spedo.photoSpeed(time_delta_photo), "rad/s");
        }
        else if (time_resto - 2 > target_time-window)
        {
          delay_time_shoot = time_resto - target_time;
          delay(delay_time_shoot);
          shhot_motor.write(angel);
          delay(motor_delay);
          shhot_motor.write(0);
          // debshoot.sPrint("SECOND_IF 2- if photo_speed is ", spedo.photoSpeed(time_delta_photo), "rad/s");
          debshoot.sPrint("SECOND_IF 2- if work time is ", time_resto - 2, "ms");
          debshoot.sPrint("SECOND_IF 2- if delay time is ", delay_time_shoot, "ms");
          debshoot.sPrint("SECOND_IF 2- if new work time is ", time_resto - delay_time_shoot, "ms");
          
        }
        else if (time_resto < target_time-window)
        {
          if (total_time > target_time)
         {
            delay_time_shoot=total_time-(target_time -time_resto);
            delay(delay_time_shoot);
            shhot_motor.write(angel);
            delay(motor_delay);
            shhot_motor.write(0);
             // debshoot.sPrint("THIRD_IF 3 - if speed is ", spedo.photoSpeed(time_delta_photo), "rad/s");
            debshoot.sPrint("THIRD_IF 3 - if work time is ", time_resto, "ms");
            debshoot.sPrint("THIRD_IF 3 - if delay time is ", delay_time_shoot, "ms");
            debshoot.sPrint("THIRD_IF 3 - if new work time is ", time_resto - delay_time_shoot, "ms");
         }
          else if (total_time < target_time)
         {
            debshoot.sPrint("FOURTH_IF 4 -  if time total  is ", total_time, "ms");
            debshoot.sPrint("FOURTH_IF 4 -  if work time is ", time_resto- 2, "ms");
           delay_time_shoot=total_time-target_time;
           if (delay_time_shoot< 0)
           {
             delay_time_shoot=-total_time+target_time;
           }
            delay(delay_time_shoot);
            shhot_motor.write(angel);
            delay(motor_delay);
            shhot_motor.write(0);
              // debshoot.sPrint("FOURTH_IF 4 -  if photo_speed is ", spedo.photoSpeed(time_delta_photo), "rad/s");
            debshoot.sPrint("FOURTH_IF 4 -  if delay time is ", delay_time_shoot, "ms");
            debshoot.sPrint("FOURTH_IF 4 -  if new work time is ", time_resto + delay_time_shoot, "ms");
         }
        }

}
