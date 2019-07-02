#include "speed.h"
#include "Arduino.h"
#include "Servo.h"
#include "debug.h"
 #include "math.h"
debug debugger_speed;
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
    float help_val_theta=((photo_seco)*(PI/6));
    float result=solveTimeEquation(accelo_1,photo_speed_intern,help_val_theta);
    return result;
}
float speed::hallRst(int hall_seco,int delta_hall_rst,float accelo_2)
{
    float hall_speed_intern=hallSpeed(delta_hall_rst);
    float help_val_theta=hall_seco*PI;
    float result=solveTimeEquation(accelo_2,hall_speed_intern,help_val_theta);
    return result;
}
float speed::totalHallTime(int delto_hall,float accelo_3)
{
    float hallSpeedo=hallSpeed(delto_hall);
    return(solveTimeEquation(accelo_3,hallSpeedo,2*PI));
}
float speed::totalPhotoTime(int delto_photo,float accelo_4)
{
    float photoSpeedo=photoSpeed(delto_photo);
    float result=(solveTimeEquation(accelo_4,photoSpeedo,2*PI));
    return result;
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
     float c;
    //  if (b1 >15)
    //  {
    //      c=-(2*PI-(c1+2*PI));
    //  }
    //  else
    //  {
    //      c=-(2*PI-(c1));
    //  }
      c=-(4*PI-(c1));
   
   float a= a1/2, b=b1;
   float  x1, x2, discriminant,real_part,imaginary_part;
   discriminant = (b*b)- (4*a*c);
   debugger_speed.sPrint("a from time function ",a,"");
   debugger_speed.sPrint("b from time function ",b,"");
   debugger_speed.sPrint("c from time function ",c,"");
   debugger_speed.sPrint("discriminant from time function ",discriminant,"");
   if (a!=0){
    if (discriminant > 0 && a !=0) {
        x1 = (-b + sqrt(discriminant)) / (2*a);
        x2 = (-b - sqrt(discriminant)) / (2*a);
            // return just the small positive values
            if (x1 >0)
            {
                if (x1 >x2 && x2 <15 && x2 >0)
                {
                    debugger_speed.sPrint("the smallest time soluation is x2  ",x2,"sec");
                    return 1000*x2;
                }
                else if (x2 > x1 && x1<15)
                {
                    debugger_speed.sPrint("the smallest time soluation is x1  ",x1,"sec");
                    return 1000*x1;
                }

                else if (x2 >0 && x2 < 15)
                {
                     debugger_speed.sPrint("the smallest time soluation is x2  ",x2,"sec");
                    return 1000*x2;
                }
                else if (x1 >x2 && x1 < 15)
                {
                     debugger_speed.sPrint("the smallest time soluation is x1  ",x1,"sec");
                    return 1000*x1;
                }
                else
                {
                    Serial.print(" the values are not right or so big ");
                    Serial.print(" the time soluation x1  ");
                    Serial.println(x1);
                    Serial.print(" the time soluation x2  ");
                    Serial.println(x2);
                }
            }
            else
            {
                if (x2 >0 && x2 <15)
                {
                    debugger_speed.sPrint("the smallest time soluation is x2  ",x2,"sec");
                    return 1000*x2;
                }
                else
                {
                    Serial.print(" the values are not right or so big ");
                    Serial.print(" the time soluation x1  ");
                    Serial.println(x1);
                    Serial.print(" the time soluation x2  ");
                    Serial.println(x2);
                }
            }
            
    }
    
    else if (discriminant == 0 && a !=0) {
        x1 = (-b + sqrt(discriminant)) / (2*a);
        debugger_speed.sPrint("there is only one soluation  ",x1,"sec");
        return 1000*x1;
    }
    else {
        
        real_part=-b/(2*a);
        imaginary_part=sqrt(-discriminant)/(2*a);
        x1=sqrt((real_part*real_part)+(imaginary_part*imaginary_part));
        debugger_speed.sPrint("the soluation is complex and abs value for the real and imaginary parts are  ",x1,"sec");
        x2=(2*PI-c1)/b;
        debugger_speed.sPrint("the alternative soluation -c/b  ",x2,"sec");
        debugger_speed.sPrint("c1  ",c1,"rad");
        debugger_speed.sPrint("b  ",b,"rad/s");
        return 1000*x2; 
    }

   }
   else
   {
        x1 = -c/b;
        debugger_speed.sPrint("the acceleration is 0 and there is only one soluation x1  ",x1,"sec");
        return 1000*x1;
        
   }
}
