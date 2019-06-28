#include "speed.h"
#include "Arduino.h"
#include "Servo.h"
#include "debug.h"
 #include "math.h"
debug debtest;
#define PI 3.1415926535897932384626433832795
uint16_t NEW_DELAY_SPEED;
float back_array[2];
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
float speed::photoRst(int photo_seco,int delta_pho_rst,float accelo_1)
{
    float photo_speed_intern=photoSpeed(delta_pho_rst);
    return(solveTimeEquation(accelo_1,photo_speed_intern,(((12-(photo_seco))*(PI/6)))));
}
float speed::hallRst(int hall_seco,int delta_hall_rst,float accelo_2)
{
    float hall_speed_intern=hallSpeed(delta_hall_rst);
    return(solveTimeEquation(accelo_2,hall_speed_intern,(((2-(hall_seco))*(PI)))));
}
float speed::totalHallTime(int delto_hall,float accelo_3)
{
    float hallSpeedo=hallSpeed(delto_hall);
    return(solveTimeEquation(accelo_3,hallSpeedo,0));
}
float speed::totalPhotoTime(int delto_photo,float accelo_4)
{
    float photoSpeedo=photoSpeed(delto_photo);
    return((solveTimeEquation(accelo_4,photoSpeedo,0)));
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
float speed::getThetavalues(int time_interval, int time_target_val, float angular_acceleartion_val, char xval)
{
    switch (xval)
    {
    case 'x':
        return (2*PI)+(photoSpeed(time_interval)*time_target_val/1000)-(0.5*angular_acceleartion_val*((time_target_val/1000)*(time_target_val/1000)));
        break;
    case 't':
        float x_help=(2*PI)+(photoSpeed(time_interval)*time_target_val/1000)-(0.5*angular_acceleartion_val*((time_target_val/1000)*(time_target_val/1000)));
        return x_help-(photoSpeed(time_interval)*time_target_val/1000)-(0.5*angular_acceleartion_val*((time_target_val/1000)*(time_target_val/1000)));
    default:
        break;
    }
}

// Calculate the time values
 float speed::solveTimeEquation(float a1, float b1,float c1)
 {
     float c_1;
  float a_1= a1/2, b_1=b1;
    float  x1, x2, discriminant,real_part,imaginary_part;
   if (a_1!=0){
     c_1=-(2*PI-(c1));
     discriminant = (b_1*b_1)- (4*a_1*c_1);
    if (discriminant > 0 && a_1 !=0) {
        x1 = (-b_1 + sqrt(discriminant)) / (2*a_1);
        x2 = (-b_1 - sqrt(discriminant)) / (2*a_1);
            // return just the small positive values
        if (x1>0)
        {
            if (x1 > x2 )
            {
                if (x2 >0 )
                {
                    cli();
                    Serial.print("the time soluation is 1  ");
                    Serial.println(x2);
                    return 1000*x2;
                    sei();
                   
                }
                else
                {
                    Serial.print("the time soluation is 2  ");
                    Serial.println(x1);
                    return 1000*x1;
                    
                }
                
            }
        }
    }
    
    else if (discriminant == 0&& a_1 !=0) {
        x1 = (-b_1 + sqrt(discriminant)) / (2*a_1);
        Serial.print("the time soluation is  3 ");
                    Serial.println(x1);
        return 1000*x1;
        
    }

    else {
        real_part=-b_1/(2*a_1);
        imaginary_part=sqrt(-discriminant)/(2*a_1);
        x1=sqrt((real_part*real_part)+(imaginary_part*imaginary_part));
        Serial.print("the time soluation is 4  ");
                    Serial.println(x1);
        return 1000*x1;
        
        
    }

   }
   else
   {
        x1 = -c_1/b_1;
        Serial.print("the time soluation is  5 ");
                    Serial.println(x1);
        return 1000*x1;
        
   }
   
}
