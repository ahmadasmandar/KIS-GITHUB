#include "speed.h"
#include "Arduino.h"
#include "Servo.h"
#include "debug.h"
debug debtest;
Servo motor_shoot;
#define PI 3.1415926535897932384626433832795
uint16_t NEW_DELAY_SPEED;
speed::speed()
{
    //motor_shoot.write(20);
    //motor_shoot.write(0);
}

float speed::photoSpeed(int delta_pho)
{
return((PI/6)*1000/delta_pho);
}
float speed::hallSpeed(int delta_hal)
{
return(PI*1000/delta_hal);
}
float speed::photoRst(int photo_seco,int delta_pho_rst)
{
    float photo_speed_intern=photoSpeed(delta_pho_rst);
    return((float)(((12-(photo_seco+1))*(PI/6))/photo_speed_intern )*1000);
}
float speed::hallRst(int hall_seco,int delta_hall_rst)
{
    float hall_speed_intern=hallSpeed(delta_hall_rst);
    if (hall_seco == 0)
    {
        return((float)(2*PI/hall_speed_intern )*1000);
    }
    else
    {
        return((float)(((2-(hall_seco+1))*PI)/hall_speed_intern )*1000);
    }
    
  
    
}
float speed::totalHallTime(int delto_hall)
{
    float hallSpeedo=hallSpeed(delto_hall);
    return(1000*2*PI/hallSpeedo);
}
float speed::totalPhotoTime(int delto_photo)
{
    float photoSpeedo=photoSpeed(delto_photo);
    return(1000*2*PI/photoSpeedo);
}
void speed::shootBall(float TIME_INTERRUPT_HALL_SHOOT,uint16_t TARGET_TIME_SHOOT,uint16_t TIME_WINDOW_HALL_IN_SHOOT,uint16_t TIME_DELTA_HALL_SHHOT,uint8_t HALL_SECTION_SHOOT)
{
    while (1)
    
        {
          debtest.sPrint("TIME_INTERRUPT_HALL_SHOOT",TIME_INTERRUPT_HALL_SHOOT,"");
    debtest.sPrint("TIME_DELTA_HALL_SHHOT",TIME_DELTA_HALL_SHHOT,"");
          if(TIME_INTERRUPT_HALL_SHOOT!=0){
            float work_time=TIME_INTERRUPT_HALL_SHOOT;
          if(  work_time < TARGET_TIME_SHOOT &&  work_time>TARGET_TIME_SHOOT-TIME_WINDOW_HALL_IN_SHOOT)
          {  
            
            delay(100);
            motor_shoot.write(0);
            break;
          }
          else if (work_time > TARGET_TIME_SHOOT )//&& hall_section==1)
          {
            int delay_time=work_time-TARGET_TIME_SHOOT;
            delay(delay_time);
            //if (delay_time < TARGET_TIME_SHOOT && delay_time > TARGET_TIME_SHOOT-TIME_WINDOW_HALL_IN_SHOOT)
            //{
            motor_shoot.write(20);
            delay(100);
            motor_shoot.write(0);
            break;
          //}
          }
          else if (work_time < TARGET_TIME_SHOOT-TIME_WINDOW_HALL_IN_SHOOT)
          {
            float TIME_TOTAL_HALL_SHOOT=totalHallTime(TIME_DELTA_HALL_SHHOT);
            float TIME_REST_HALL_SHOOT=hallRst(HALL_SECTION_SHOOT,TIME_DELTA_HALL_SHHOT);  
            if (TARGET_TIME_SHOOT< TIME_TOTAL_HALL_SHOOT){
            NEW_DELAY_SPEED=TIME_TOTAL_HALL_SHOOT-(TARGET_TIME_SHOOT-TIME_REST_HALL_SHOOT);
            delay(NEW_DELAY_SPEED);
            motor_shoot.write(20);
            delay(100);
            motor_shoot.write(0);
            break;
            }
            else if (TARGET_TIME_SHOOT > TIME_TOTAL_HALL_SHOOT)
            {
              NEW_DELAY_SPEED=TIME_TOTAL_HALL_SHOOT+(TARGET_TIME_SHOOT-TIME_REST_HALL_SHOOT);
            } 
            delay(NEW_DELAY_SPEED);
            motor_shoot.write(20);
            delay(100);
            motor_shoot.write(0);
            break;
            
          }
          else
          {
              motor_shoot.write(180);
              delay(100);
              break;
          }
          
          }
        
        }
}
boolean speed::secureMotion(int val1, int val2,boolean start_FLAG)
{
  if (millis()>5000)
  {
    if(start_FLAG==true)
    {
      Serial.println("the check is started");
      start_FLAG=false;
    }
    if (abs(val1-val2)> val2 || abs(val1-val2)> val1)
  {
      while (1)
      {
        Serial.print(" Something is Wrong please Restart the deference =   ");
        Serial.println(abs(val1-val2));
        delay(500);
      }
  }
  }
  return start_FLAG;
  
}

//******* 
void speed::checkStop(int time1, int time2)
{ 
    if (time1 == time2 )
    { 
        Serial.print(" Errror Error Errror   the check position error   ");
        Serial.println(time1);
        Serial.println(time2);
        
    }
  
}