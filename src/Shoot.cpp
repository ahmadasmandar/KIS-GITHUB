#include "Arduino.h"
#include "Servo.h"
#include "debug.h"
#include "Shoot.h"
debug debugger_shoot;
Servo shhot_motor;
float delay_time_shoot;
int angel=22;
int motor_delay=100;
int end_angel=0;

void Shoot::motorIntil()
{
  shhot_motor.attach(9);
  shhot_motor.write(0);
}
void Shoot::shootManuel_dropall()
{
  for (int i=0;i<15;i++)
  {
          shhot_motor.write(angel);
          delay(motor_delay);
          shhot_motor.write(end_angel); 
          delay(500);
  }
          
}
void Shoot::shootManuel()
{
  
          shhot_motor.write(angel);
          delay(motor_delay);
          shhot_motor.write(end_angel); 
          delay(500);
          
}

void Shoot::fireBall(uint16_t delta_hoder,float total_time,uint8_t section, float time_resto , uint16_t window,uint16_t target_time)
{
        //  if (time_resto < target_time+delta_hoder && time_resto >target_time)
        // {
        //   delay_time_shoot = time_resto - target_time-10;
        //   if (delay_time_shoot<15000 )
        //   {
        //   delay(delay_time_shoot);
        //   shhot_motor.write(angel);
        //   delay(motor_delay);
        //   shhot_motor.write(end_angel);
        //   debugger_shoot.sPrint("target_time", target_time, "ms");
        //   debugger_shoot.sPrint("normal -if delay time is ", delay_time_shoot, "ms");
        //   }
        // }     
        if (time_resto > target_time && time_resto< 15000)
        {
          delay_time_shoot = time_resto - target_time;
          if (delay_time_shoot<15000 )
          {
            delay(delay_time_shoot);
          shhot_motor.write(angel);
          delay(motor_delay);
          shhot_motor.write(end_angel);
          debugger_shoot.sPrint("target_time", target_time, "ms");
          debugger_shoot.sPrint("rest -if delay time is ", delay_time_shoot, "ms");
          debugger_shoot.sPrint("rest -if window ", window, "ms");
          }
          else
          {
            debugger_shoot.sPrint("rest time delay s big ", delay_time_shoot, "ms");
          }
          
        }
        else if (total_time > target_time)
        {
            delay_time_shoot=total_time-(target_time -time_resto);
            if (delay_time_shoot<15000 )
          {
            delay(delay_time_shoot);
          shhot_motor.write(angel);
          delay(motor_delay);
          shhot_motor.write(end_angel);
          debugger_shoot.sPrint("target_time", target_time, "ms");
          debugger_shoot.sPrint(" total if delay time is ", delay_time_shoot, "ms");
          debugger_shoot.sPrint("rest -if window ", window, "ms");
          }
          else
          {
            debugger_shoot.sPrint("target_time", target_time, "ms");
            debugger_shoot.sPrint("total time delay s big ", delay_time_shoot, "ms");
          }
         }
        else
        {
          debugger_shoot.sPrint("target_time", target_time, "ms");
          debugger_shoot.sPrint("there is error we couldn't shoot",0,"");
        }
        
}
