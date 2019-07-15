#include "Arduino.h"
#include "Servo.h"
#include "Shoot.h"
Servo shhot_motor;
float delay_time_shoot;
// diese Werte sind experimental bekommen
int angel = 22;
int motor_delay = 100;
int end_angel = 0;

void Shoot::motorIntil()
{
  shhot_motor.attach(9);
  shhot_motor.write(0);
}
// void Shoot::shootManuel_dropall()
// {
//   for (int i = 0; i < 15; i++)
//   {
//     shhot_motor.write(angel);
//     delay(motor_delay);
//     shhot_motor.write(end_angel);
//     delay(500);
//   }
// }
void Shoot::shootManuel()
{
  shhot_motor.write(angel);
  delay(motor_delay);
  shhot_motor.write(end_angel);
}

void Shoot::fireBall( float total_time, uint8_t section, float time_resto, uint16_t window, uint16_t target_time)
{
  delay_time_shoot=0;
  if (time_resto >= target_time && time_resto < 5000)
  {
    delay_time_shoot = time_resto - target_time;
    
    if (delay_time_shoot < 5000)
    {
      delay(delay_time_shoot);
      // schieß den Ball nach dem Delay
      shhot_motor.write(angel);
      delay(motor_delay);
      shhot_motor.write(end_angel);
      Serial.println("* printer from shoot rest *");
      sPrint("target_time", target_time, "ms");
      sPrint("rest -if delay time is ", delay_time_shoot, "ms");
      sPrint("rest -if total ", total_time, "ms");
      sPrint("rest -if rest ", time_resto, "ms");
      sPrint("rest -if window ", window, "ms");
      Serial.println("***********");
    }
    else
    {
      Serial.println("* Error from shoot *");
      sPrint("rest time delay s big ", delay_time_shoot, "ms");
      sPrint("rest -if total", total_time, "ms");
      sPrint("rest -if rest ", time_resto, "ms");
      sPrint("rest -if window ", window, "ms");
      Serial.println("****");
    }
  }
  else if (total_time >= target_time)
  {
    delay_time_shoot = total_time - (target_time - time_resto);
    if (delay_time_shoot < 5000)
    {
      delay(delay_time_shoot);
      shhot_motor.write(angel);
      delay(motor_delay);
      shhot_motor.write(end_angel);
      Serial.println("*** printer from shoot total ****");
      sPrint("target_time", target_time, "ms");
      sPrint("total if delay time is ", delay_time_shoot, "ms");
      sPrint("total -if total ", total_time, "ms");
      sPrint("total -if rest ", time_resto, "ms");
      sPrint("total -if window ", window, "ms");
      Serial.println("***");
    }
    else
    {
      Serial.println("-------- Error from shoot -------");
      sPrint("target_time", target_time, "ms");
      sPrint("total time delay s big ", delay_time_shoot, "ms");
      sPrint("total -if total ", total_time, "ms");
      sPrint("total -if rest ", time_resto, "ms");
      sPrint("total -if window ", window, "ms");
      Serial.println("--------------------------");
    }
  }
  else
  {
    Serial.println("------- Error from shoot -------");
    sPrint("target_time", target_time, "ms");
    sPrint("there is error we couldn't shoot", 0, "");
    sPrint("total -if time ", total_time, "ms");
    sPrint("total -if rest ", time_resto, "ms");
    sPrint("total -if window ", window, "ms");
    Serial.println("-------------------------");
  }
}

void Shoot::sPrint(String to_pr_c, float valo, String einheit)
{
    Serial.print(to_pr_c);
    Serial.print("  ");
    Serial.print(valo);
    Serial.print("  ");
    Serial.println(einheit);
}

