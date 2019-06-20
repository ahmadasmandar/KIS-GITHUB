#include "speed.h"
#include "Arduino.h"
#include "Servo.h"
#include "debug.h"
 #include "math.h"
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
/*Calculate the time rest for the init theta using the theta equation 
    theta=theta0+w0*t+1/2* acceleration*t^2
 */
float calculateTime(float accelaration, float winkelgeschwindigkeit, float inittheta)
{
    float a= accelaration, b=winkelgeschwindigkeit, c=(2*PI-(inittheta));
    float  x1, x2, discriminant, realPart, imaginaryPart;
    float discriminant = b*b - 4*a*c;
    
    if (discriminant > 0) {
        x1 = (-b + sqrt(discriminant)) / (2*a);
        x2 = (-b - sqrt(discriminant)) / (2*a);
        debtest.sPrint( "Roots are real and different." ,0,"");
        debtest.sPrint("t1 = ", x1 ,"") ;
        debtest.sPrint("t2 = ", x2 ,"") ;
    }
    
    else if (discriminant == 0) {
        debtest.sPrint( "Roots are real and same." ,0,"");
        x1 = (-b + sqrt(discriminant)) / (2*a);
        debtest.sPrint("t1 = t2 =", x1 ,0,"");
    }

    else {
        float realPart = -b/(2*a);
        float  imaginaryPart =sqrt(-discriminant)/(2*a);
        debtest.sPrint("Roots are complex and different."  ,0,"");
        // debtest.sPrint( "x1 = ",realPart,"");
        // debtest.sPrint( "x2 = ",realPart,"");
    }

return 0.0;

}