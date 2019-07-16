#include <Arduino.h>
#include "kisg6.h"
#include "Servo.h"
#include "speed.h"
#include "Shoot.h"
#include "math.h"
//********* the Constants that we may used alot
#define PI 3.1415926535897932384626433832795
 // the time aus FreienFall Gesetz mit s=0.75m und g= 9.81
uint16_t time_fall=1000* sqrt((2*0.75)/9.81); // the result will be in 391.030 ms 

//************** Objects from the main Classes  ******************//

speed speed_main; // from The Class Speed that will help in calculate every important value (speed, time, time rest, time total......)
kisg6 hartz;  // from Class KISG6 that contain the experement Conditions (pins Setup )  // from Debug class the main use is to Serial print complexe Phrases
Shoot shoot_main; // from Shoot Class used to shhot the ball 
// Servo main_motor;
// Here is the value for the Interrupts Counter for
// the PhotoSensor and The Hall Sensor
/*********  **********************/
volatile uint8_t photo_section = 0, hall_section = 0; // the count wil Start from 0 for the beide Sensors
uint32_t photo_start, hall_start;            // these wil be used to calculate the delta time
/**************************************************************************************/
// **************  here is the Variabls which will be changed in the Interrupts ********//
volatile uint16_t time_delta_photo;
volatile uint16_t time_delta_hall;
/////
// volatile float time_interrupt_photo, time_interrupt_hall; // these will hold the time rest values from the change point..
//********
// these just for help
// float time_soluation[2];       // this array will hold the soluation values that will be used to shoot the ball using Calculatetime Function
//******************************* speed parameter  S=73cm  **********************///
// float photo_speed = 0, hall_speed = 0, new_speed = 0; // Speed from the tow sensor with new_speed to use it later
// float photo_acceleration;
float time_total_photo_speed, time_total_hall; // this values will be used to get the best shoot moment
// float time_rest_photo, time_rest_hall;     // it will be generated using Speed Class
float  time_rest_to_null_speed;
/// ******* the Variable will be used in pet the Debounces of the Buttons
uint32_t press_delay_trigger = 0;
uint32_t butt1_press_delay_choose_mod = 0;
uint32_t butt1_press_delay_read_mod = 0;
//****************************************** these Variables will be used to get the Photo section and the Hall section to determine the position
uint8_t hall_pos,photo_pos;

//hold_position ist Position wo wi trigger gedruckt, hold_test_position für andere Sensor
uint8_t hold_position,hold_test_position; //TODO use the hold position from hall to ensure the position from photo sensor for the delta time 
//********** Recive the Time delta from Interrupt 
uint16_t hold_delta_photo_sensor,hold_delta_hall_sensor,time_target;
//**************************
//************ New Calculations using Theata 
float angular_speed,angular_acceleration;

// new speed var
float speed_1,speed_2;
//** the main timers to calculate the acceleration 
//**** these timer wil be used the time to excute a part from the code 
uint32_t start_excu_time=0;
//this tow variables are used to test the solve vs normal ohne acceleration
 int time_total_photo_equation, time_rest_to_null_equation;
 int time_total_hall_speed;
//****************
int time_used_to_calc;
//try to add the angel from interrupt 
//************
uint8_t button1_vlaue = LOW, program_mode = 1, switch_input = LOW;
uint8_t newMode;
boolean choose_mode_flag=true;
// try to use accerleration array
boolean start_hall=false, Hall_help=true,secure_it=false;
//secure motion and check stop arrays and Parameter
uint16_t sec_arr[2];
uint16_t stop_arr[10];
uint8_t sec_counter,stop_counter;

//*******************Functions prototypes

void photo_sens_interrupt();
/******/
void hall_sens_interrupt();
/*****/
uint16_t checkCounter(uint16_t counter1, uint8_t maxV);
void sPrint(String to_pr_c, float valo);
void checkStartCondtions(uint8_t hall_seco, uint8_t hoto_cso);
uint8_t chooseMode();
//**********
void getAcceleration(char x_sens);
//*******
void applyMode();
void readMode();
//***
void shootMain(float ang_speed, uint8_t pos_holder,uint8_t current_section, uint16_t total_time,uint16_t rest_time,uint16_t delta_hoder);
// end function prototypes

//****************** the setup *****************************

void setup()
{
  hartz.pinSetup();
  attachInterrupt(digitalPinToInterrupt(hartz.photosens), photo_sens_interrupt, CHANGE);
  attachInterrupt(digitalPinToInterrupt(hartz.hallsens), hall_sens_interrupt, CHANGE);
  shoot_main.motorIntil();
  Serial.begin(57600);
  hall_start = millis();
  photo_start = millis();
}

//********************************* the main loop ****************
void loop()
{
  
  //checkStartCondtions(hall_section,photo_section);
  /**What is probably happening is that the variables are being changed by 
   * the interrupt routines mid-way through the calculations.My 'fix' reduces 
   * the time spent doing the calculation with the volatile
   * variables thus making it less likely that there is a collision.
   * What I did is copy the volatile variables to local variables with interrupts disabled
     for that brief period.
      using the cli() sei() functions;
   * ***/
  // hier wird nur die beiden secure und stop array gefüllt
sec_arr[sec_counter]=time_delta_photo; // die Zeit für 30° 
stop_arr[stop_counter]=time_delta_photo;
sec_counter=checkCounter(sec_counter,2);
stop_counter=checkCounter(stop_counter,10);
// check if richtig start
if (Hall_help==true && photo_section>3)
{
  Hall_help=false;
  start_hall=true;
  secure_it=true;
  Serial.println(hall_section);
}
 applyMode();
 readMode();
 program_mode=chooseMode();
 checkStartCondtions(hall_section,photo_section);
 if (start_hall==true)
 {
 secure_it=speed_main.secureMotion(sec_arr[0],sec_arr[1],secure_it);
 }
 if (start_hall==true)
 {
   if (stop_arr[10]!=0 && millis()>10000)
 {
   speed_main.checkStop(stop_arr);
 }
 }

 //sPrint("first millis ",millis(),"ms");
 if (millis()>2000)
 {
   switch (program_mode)
   {
   case 1:
    if(time_delta_hall!=0)
   {
     getAcceleration('h');
   }
     break;
   default:
   if(time_delta_photo!=0)
   {
     getAcceleration('p');
   }
     break;
   }
 }
  //************************************************ the main part ******************************************************************

  if (digitalRead(hartz.trigger) == HIGH &&  millis() - press_delay_trigger > 2000)
  {
    //********************
     Serial.println("********trigger pressed****************");
    switch (program_mode)
    {
      /** nur Hall sensor benutzen **/
    case 1:
    sPrint("we are in Mode ",program_mode);
    time_rest_to_null_equation=0;
    getAcceleration('h');
      while (hall_section!=0)
      {
        delay(1);
      }
      start_excu_time=millis();
      hold_delta_hall_sensor=time_delta_hall;
      hold_position=hall_section;
      hold_test_position=photo_section;
      //die Zeit Berechnung ahand der Equation
      time_total_hall=speed_main.totalHallTime(hold_delta_hall_sensor,angular_acceleration);
      time_rest_to_null_equation=speed_main.hallRst(hold_position,hold_delta_hall_sensor,angular_acceleration);

      
      angular_speed=speed_main.hallSpeed(time_delta_hall)+(angular_acceleration*(hold_delta_hall_sensor/1000));

      if (angular_speed<12)
          {   
              time_rest_to_null_speed=1000*((2*PI-(hold_position*(PI/6)))/angular_speed);
              time_total_hall_speed=1000*(2*PI/angular_speed);
          }
          else if (angular_speed>12 && angular_speed<25)
          {
            time_rest_to_null_speed=1000*((4*PI-(hold_position*(PI/6)))/angular_speed);
            time_total_hall_speed=1000*(4*PI/angular_speed);
          }
          else if (angular_speed>25)
          {
            time_rest_to_null_speed=1000*((6*PI-(hold_position*(PI/6)))/angular_speed);
            time_total_hall_speed=1000*(6*PI/angular_speed);
          }
          
          if (time_total_hall==0)
          {
            time_total_hall=time_total_hall_speed;
            Serial.print("time_total_hall is 0 but updated ");
            Serial.println(time_total_hall_speed);
          }
            if (time_rest_to_null_equation==0)
          {
            time_rest_to_null_equation=time_rest_to_null_speed;
            Serial.print("time_rest_to_null_equation is 0 but updated  ");
            Serial.println(time_rest_to_null_speed);
          }
          time_used_to_calc=millis()-start_excu_time;
          shootMain(angular_speed,hold_position,hall_section,time_total_hall,time_rest_to_null_equation-time_used_to_calc,hold_delta_hall_sensor/6);
          Serial.print("time_used_to_caculate");
          Serial.println(time_used_to_calc);
      break;
      /** nur PHOTO sensor benutzen **/

    case 2:
          sPrint("we are in Mode ",program_mode);
           getAcceleration('p');
          while (photo_section!=0)
          {
            delay(1);
          }
          start_excu_time=millis();
          hold_delta_photo_sensor=time_delta_photo;
          hold_position=photo_section;
          hold_test_position=hall_section;

          time_total_photo_equation=speed_main.totalPhotoTime(hold_delta_photo_sensor,angular_acceleration);
          //hold_position ist die jetzige Section wovon wir mit Berechnungen angefangen haben
          time_rest_to_null_equation=speed_main.photoRst(hold_position,hold_delta_photo_sensor,angular_acceleration);



          angular_speed=speed_main.photoSpeed(hold_delta_photo_sensor)+(angular_acceleration*(hold_delta_photo_sensor/1000));
          //die Berechnung der zeit nur anhand Speed
          if (angular_speed<17)
          {   
              time_rest_to_null_speed=1000*((2*PI-(hold_position*(PI/6)))/angular_speed);
              time_total_photo_speed=1000*(2*PI/angular_speed);
          }
          else if (angular_speed>17 && angular_speed<25)
          {
            time_rest_to_null_speed=1000*((4*PI-(hold_position*(PI/6)))/angular_speed);
            time_total_photo_speed=1000*(4*PI/angular_speed);
          }
          else if (angular_speed>25)
          {
            time_rest_to_null_speed=1000*((6*PI-(hold_position*(PI/6)))/angular_speed);
            time_total_photo_speed=1000*(6*PI/angular_speed);
          }
          
          //*******************************
          //********************************
           if (time_total_photo_equation ==0)
          {
            time_total_photo_equation=time_total_photo_speed;
            Serial.println("the total eqaution ==0 >> using total speed");
          }
          time_used_to_calc=millis()-start_excu_time;
          shootMain(angular_speed,hold_position,photo_section,time_total_photo_equation,time_rest_to_null_equation-time_used_to_calc,hold_delta_photo_sensor);
           Serial.print("time_used_to_caculate");
          Serial.println(time_used_to_calc);
          break;
      
      break;
      // here we are using the tow sensors 
      case 3:
          sPrint("we are in Mode ",program_mode);
          getAcceleration('h');
          while (photo_section!=0)
          {
            delay(1);
          }
          //TODO we need to take care of 15° angel because the deference between hall and 
          start_excu_time=millis();
          hold_delta_photo_sensor=time_delta_photo;
          hold_position=photo_section;
          hold_test_position=hall_section;
          time_total_photo_equation=speed_main.totalPhotoTime(hold_delta_photo_sensor,angular_acceleration);
          time_rest_to_null_equation=speed_main.photoRst(hold_position,hold_delta_photo_sensor,angular_acceleration);
          angular_speed=speed_main.photoSpeed(hold_delta_photo_sensor)+(angular_acceleration*(hold_delta_photo_sensor/1000));

          if (angular_speed<15)
          {   
              time_rest_to_null_speed=1000*((2*PI-(hold_position*(PI/6)))/angular_speed);
              time_total_photo_speed=1000*(2*PI/angular_speed);
          }
          else if (angular_speed>15 && angular_speed<25)
          {
            time_rest_to_null_speed=1000*((4*PI-(hold_position*(PI/6)))/angular_speed);
            time_total_photo_speed=1000*(4*PI/angular_speed);
          }
          else if (angular_speed>25)
          {
            time_rest_to_null_speed=1000*((6*PI-(hold_position*(PI/6)))/angular_speed);
            time_total_photo_speed=1000*(6*PI/angular_speed);
          }
          
          //*******************************
          //********************************
           if (time_total_photo_equation ==0)
          {
            time_total_photo_equation=time_total_photo_speed;
            Serial.println("the total eqaution ==0 >> using total speed");
          }
          if (time_rest_to_null_equation==0)
          {
            time_rest_to_null_equation=time_rest_to_null_speed;
            Serial.println("the rest eqaution ==0 >> using rest speed");
          }
          time_used_to_calc=millis()-start_excu_time;
          shootMain(angular_speed,hold_position,photo_section,time_total_photo_equation,time_rest_to_null_equation-time_used_to_calc,hold_delta_photo_sensor);
           Serial.print("time_used_to_caculate");
          Serial.println(time_used_to_calc);
          //TODO try the time calc with the total time too
          // sPrint("end time  exucte and print  ",millis()-start_excu_time,"ms");
          break;
    case 4:
      sPrint("we are in Mode ",program_mode);
      shoot_main.shootManuel();         
      break;
    }
    press_delay_trigger = millis();
  }
}







//************** PHOTO SENS INTERRUPT *********
void photo_sens_interrupt()
{
  time_delta_photo = millis() - photo_start;
  photo_start = millis();
   photo_section+=1;
  if (photo_section==12)
  {
    photo_section=0;
    // photo_cycle+=1;
  }
}

//************** HALL SENS INTERRUPT *********
void hall_sens_interrupt()
{
  time_delta_hall = millis() - hall_start;
  hall_start = millis();
  if(start_hall)
  {
       hall_section+=1;
  }
 
  if (hall_section==2)
  {
    hall_section=0;
    // hall_cycle+=1;
  }
}

/********************/
// the counter function will add 1 and check if he reached his Max
uint16_t checkCounter(uint16_t counter1, uint8_t maxV)
{
  counter1 += 1;
  if (counter1 == maxV)
  {
    counter1 = 0;
  }
  return counter1;
}

/********end serial *********/
// void stopSerial(uint8_t checkSerial)
// {
//   if (checkSerial == HIGH)
//   {
//     Serial.end();
//   }
// }

/******* check start positions **********/

void checkStartCondtions(uint8_t hall_Seco, uint8_t photo_sco)
{
  if (hall_Seco != 0)
  {
    digitalWrite(hartz.led2, HIGH);
    delay(150);
    digitalWrite(hartz.led2, LOW);
  }
  else if (photo_sco != 0)
  {
    digitalWrite(hartz.led2, HIGH);
    delay(150);
    digitalWrite(hartz.led2, LOW);
  }
}

// calculate the acceleration using the one of the tow sensors 
void getAcceleration(char x_sens_1)
{  
    switch (x_sens_1)
    {
    case 'p':
    speed_1=speed_main.photoSpeed(time_delta_photo)+angular_acceleration*(time_delta_photo/1000);
    delay(time_delta_photo);
    speed_2=speed_main.photoSpeed(time_delta_photo)+angular_acceleration*(time_delta_photo/1000);
    angular_acceleration=-abs(1000*(speed_2-speed_1))/time_delta_photo;
    // accel_array[accel_counter]=angular_acceleration;
    // accel_counter=checkCounter(accel_counter,5);
      break;
    case 'h':
      speed_1 = speed_main.hallSpeed(time_delta_hall)+angular_acceleration*(time_delta_hall/1000);
      delay(time_delta_hall);
      speed_2 = speed_main.hallSpeed(time_delta_hall)+angular_acceleration*(time_delta_hall/1000);
      angular_acceleration = -abs(1000 * (speed_2 - speed_1) / time_delta_hall);
      // accel_array[accel_counter] = angular_acceleration;
      // accel_counter = checkCounter(accel_counter, 5);
      break;
    } 
}

// uint8_t getTargetSection(float theta_target_1)
// {
//   uint8_t section_help=6*theta_target_1/PI;
//   sPrint("section_help",section_help);
//   if (section_help > 11)
//   {
//     return section_help-12;
//   }
//   else
//   {
//     return section_help;
//   }
  
// }
void applyMode()
{
   program_mode=chooseMode();
  if (choose_mode_flag== true)
  {
    sPrint(" we are in the mode ", program_mode);
    choose_mode_flag= false;
  }
}
void readMode()
{
    if (digitalRead(hartz.butt1) == HIGH && millis() - butt1_press_delay_read_mod > 300)
  {
    program_mode = chooseMode();
    sPrint(" we are in the mode ", program_mode);
    butt1_press_delay_read_mod = millis();
  }
}

void shootMain(float ang_speed, uint8_t pos_holder,uint8_t current_section,
 uint16_t total_time,uint16_t rest_time,uint16_t delta_hoder)
{
    // this funktion was commited becuase we calcualte the lost time in the case funktions
     uint16_t time_correction_value;       
      // correct the time after caculation and take care if we are from 1 to 11 ....or 11 to 1
      float angular_speed_shoot=speed_main.photoSpeed(time_delta_photo)+angular_acceleration*(time_delta_photo/1000);
      if (current_section!=pos_holder && pos_holder !=11 )
      {
        time_correction_value=1000*(abs(current_section-pos_holder)*((PI/6)/angular_speed_shoot));
      }
      else if (current_section!=pos_holder && pos_holder ==11)
      {
        time_correction_value=1000*(((current_section+1)*(PI/6)/angular_speed_shoot));
      }
      else
      {
        time_correction_value=0;
      }
      uint16_t time_photo_cor=1000*(hold_test_position*PI/6)/angular_speed_shoot;
      uint16_t angel_15_correction=1000*(15*PI/180)/angular_speed_shoot;
      uint32_t new_rest_time;
      int new_timetarget;
      if (program_mode==1)
      {
        new_rest_time=rest_time-time_photo_cor;
         new_timetarget = - 0.4945*angular_speed_shoot + 94.54;
            //  new_timetarget=- 0.076*angular_speed_shoot*angular_speed_shoot*angular_speed_shoot + 2.9*angular_speed_shoot*angular_speed_shoot - 38*angular_speed_shoot +2.5e+02;
         new_timetarget-=angel_15_correction/10;
        //   if(ang_speed >15)
        // {
        //     new_timetarget = - 0.4945*angular_speed_shoot + 94.54;
        //     //  new_timetarget=- 0.076*angular_speed_shoot*angular_speed_shoot*angular_speed_shoot + 2.9*angular_speed_shoot*angular_speed_shoot - 38*angular_speed_shoot +2.5e+02;
        //      new_timetarget-=angel_15_correction/10;
        //     // new_timetarget=100-delta_hoder-delta_hoder;
        // }
        // else
        // {
        //   new_timetarget = - 0.4945*angular_speed_shoot + 94.54;
        //   // new_timetarget = - 0.2698*(angular_speed_shoot*angular_speed_shoot*angular_speed_shoot) + 7.0946*(angular_speed_shoot*angular_speed_shoot) - 57.666*angular_speed_shoot + 228.95;
        //   new_timetarget-=angel_15_correction/10;
        //   // new_timetarget=100-delta_hoder+delta_hoder;
        // }
      }
      else // if programm mode 2,3,4 
      {
        new_rest_time=rest_time-time_correction_value;
        if (ang_speed>12)
        {
          // new_timetarget= 1.12*angular_speed_shoot*angular_speed_shoot - 22.75*angular_speed_shoot + 174.8;
          // new_timetarget-=angel_15_correction/5;
          // new_timetarget= - 2.597*angular_speed_shoot + 96.08;
          // y = - 2.03*x + 97.05
          new_timetarget= - 3.03*angular_speed_shoot + 102.6;
          // new_timetarget= - 2.03*angular_speed_shoot + 97.05;
          new_timetarget-=angel_15_correction/10;
        }
        else
        {
          // y = - 3.03*x + 102.8
          // y = - 3.067*x + 102.6
          // new_timetarget= - 2.597*angular_speed_shoot + 96.08;
          new_timetarget= - 3.067*angular_speed_shoot + 102.6;
          // new_timetarget+=angel_15_correction/10;
        }
        
      }
       Serial.print("matlab");
       Serial.println(new_timetarget);
      shoot_main.fireBall(total_time-2,pos_holder,new_rest_time-2,delta_hoder,time_fall+new_timetarget);
      // Debugging using the serial print
      sPrint("w_new_from shoot main rad/s ",angular_speed_shoot); 
      sPrint("time_correction_value ",time_correction_value);
      sPrint("time_photo_cor ",time_photo_cor);
      sPrint("angel_15_csorrection ",angel_15_correction);
      sPrint("***** after shoot values ",2);
      sPrint("w rad/s",angular_speed_shoot);
      sPrint("time_rest used here ",rest_time);
      sPrint("time_total_used here ",total_time);
      sPrint("time_rest_equation ",time_rest_to_null_equation);
      sPrint("time_total_equation_hall ",time_total_hall);
      sPrint("time_rest_speed ",time_rest_to_null_speed);
      sPrint("time_total_speed_photo ",time_total_photo_speed);
      sPrint("time_total_speed_hall ",time_total_hall_speed);
      sPrint("hold_position",pos_holder);
      sPrint("current position ",current_section);
      sPrint("check the other sensr value ",hold_test_position);
}

/****Choose Mode function****/
uint8_t chooseMode()
{
  switch_input = digitalRead(hartz.switch_input);
  if (digitalRead(hartz.butt1) == HIGH && millis() - butt1_press_delay_choose_mod > 500)
  {
    button1_vlaue = !button1_vlaue;
    butt1_press_delay_choose_mod = millis();
    Serial.println(button1_vlaue);
    digitalWrite(hartz.led1, button1_vlaue);
  }

  if (switch_input == LOW)
  {
    digitalWrite(hartz.bboxled,LOW);
    switch (button1_vlaue)
    {
    case LOW:
      return (1);
      break;
    case HIGH:
      return (2);
      break;
    }
  }
  else if (switch_input == HIGH)
  {
    digitalWrite(hartz.bboxled, HIGH);
    switch (button1_vlaue)
    {
    case LOW:
      return (3);
      break;
    case HIGH:
      return (4);
      break;
    }
  }
  else
  {
    // return (1);
  }
  return 0;
}
void sPrint(String to_pr_c, float valo)
{
    Serial.print(to_pr_c);
    Serial.print("  ");
    Serial.print(valo);
    Serial.println();
}
// float adujstAngelwithSpeed(float speed_in,uint8_t section_number,char t_r)
// {
//     if (speed_in<12)
//     {   
//       if(t_r=='t')
//       {
//           return 1000*(2*PI/speed_in);
//       }
//       else
//       {
//         return 1000*((2*PI-(section_number*(PI/6)))/speed_in);
//       }
      
//     }
//     else if (speed_in>12 && speed_in<25)
//     {
//         if(t_r=='t')
//       {
//           1000*(4*PI/speed_in);
//       }
//       else
//       {
//         return 1000*((4*PI-(section_number*(PI/6)))/speed_in);
//       }
//     }
//     else if (speed_in>25)
//     {
//              if(t_r=='t')
//       {
//           1000*(6*PI/speed_in);
//       }
//       else
//       {
//         return 1000*((6*PI-(section_number*(PI/6)))/speed_in);
//       } 
//     }
// }