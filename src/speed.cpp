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
    return((float)(((12-(photo_seco))*(PI/6))/photo_speed_intern )*1000);
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
        return((float)(((2-(hall_seco))*PI)/hall_speed_intern )*1000);
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