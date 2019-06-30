#include "Arduino.h"
#include "Servo.h"
#include "debug.h"
#include "Shoot.h"
debug debugger_shoot;
Servo shhot_motor;
float delay_time_shoot;
int angel=20;
int motor_delay=100;
int end_angel=0;

void Shoot::motorIntil()
{
  shhot_motor.attach(9);
  shhot_motor.write(0);
}
void Shoot::shootManuel(int ango)
{
          shhot_motor.write(ango);
          delay(motor_delay);
          shhot_motor.write(end_angel); 
}

void Shoot::fireBall(uint16_t delta_hoder,float time_resto,uint8_t section, float total_time , uint16_t window,uint16_t target_time)
{
        if (time_resto > target_time)
        {
          delay_time_shoot = time_resto - target_time;
          delay(delay_time_shoot);
          shhot_motor.write(angel);
          delay(motor_delay);
          shhot_motor.write(end_angel);
          debugger_shoot.sPrint("target_time", target_time, "ms");
          debugger_shoot.sPrint("rest -if delay time is ", delay_time_shoot, "ms");
        }
        else if (time_resto < target_time)
        {
          if (total_time > target_time)
         {
            delay_time_shoot=total_time-(target_time -time_resto);
            delay(delay_time_shoot);
            shhot_motor.write(angel);
            delay(motor_delay);
            shhot_motor.write(end_angel);
            debugger_shoot.sPrint(" total if delay time is ", delay_time_shoot, "ms");
         }
        }
        else
        {
          debugger_shoot.sPrint("there is error we couldn't shoot",0,"");
        }
        
}
