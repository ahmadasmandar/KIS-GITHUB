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

const uint16_t test_time = 400; // the new value for photo sensor
const uint16_t test_time_hall=400;

//************** Objects from the main Classes  ******************//

speed speed_main; // from The Class Speed that will help in calculate every important value (speed, time, time rest, time total......)
kisg6 hartz;  // from Class KISG6 that contain the experement Conditions (pins Setup )  // from Debug class the main use is to Serial print complexe Phrases
Shoot shoot_main; // from Shoot Class used to shhot the ball 
Servo main_motor;
// Here is the value for the Interrupts Counter for
// the PhotoSensor and The Hall Sensor
/*********  **********************/
volatile uint8_t photo_section = 0, hall_section = 0; // the count wil Start from 0 for the beide Sensors
uint32_t photo_start, hall_start;            // these wil be used to calculate the delta time
/**************************************************************************************/
// **************  here is the Variabls which will be changed in the Interrupts ********//
volatile uint16_t time_delta_photo;
volatile uint16_t time_delta_hall;
volatile boolean shoot_flag=false;
/////
// volatile float time_interrupt_photo, time_interrupt_hall; // these will hold the time rest values from the change point..
//********
uint16_t time_window_hall, time_window_photo; // the time betwenn change in the value
// these just for help
float time_soluation[2];       // this array will hold the soluation values that will be used to shoot the ball using Calculatetime Function
//******************************* speed parameter  S=73cm  **********************///
float photo_speed = 0, hall_speed = 0, new_speed = 0; // Speed from the tow sensor with new_speed to use it later
// float photo_acceleration;
float time_total_photo_speed, time_total_hall; // this values will be used to get the best shoot moment
float time_rest_photo, time_rest_hall;     // it will be generated using Speed Class
float best_speed, best_total_time, best_rest, time_rest_to_null_speed;
/// ******* the Variable will be used in pet the Debounces of the Buttons
uint32_t press_delay_trigger = 0;
uint32_t butt1_press_delay_choose_mod = 0;
uint32_t butt1_press_delay_read_mod = 0;
//****************************************** these Variables will be used to get the Photo section and the Hall section to determine the position
uint8_t hall_pos,pos,photo_pos;
uint8_t hold_position,hold_test_position; //TODO use the hold position from hall to ensure the position from photo sensor for the delta time 
//********** Recive the Time delta from Interrupt 
uint16_t hold_delta_photo_sensor,hold_delta_hall_sensor,time_target;
//**************************
uint16_t delay_time;
//************ New Calculations using Theata 
float theta_zero,angular_speed,angular_speed_zero,angular_acceleration=0.000000000000;

// new speed var
float filled_speed=0,speed_1,speed_2;
boolean fire_flag=true;

//** the main timers to calculate the acceleration 
uint32_t acceleration_timer_1=0;
uint32_t acceleration_timer_0=0;
//**** these timer wil be used the time to excute a part from the code 
uint32_t start_excu_time=0,end_excu_time=0,print_timer_start=0;
//this tow variables are used to test the solve vs normal ohne acceleration
 int time_total_photo_equation, time_rest_to_null_equation;
 int time_total_hall_speed;
//****************
//uint32_t fill_start_timer=0;
//uint16_t time_correction_value;
int time_used_to_calc;
//try to add the angel from interrupt 
volatile uint32_t theta_photo=15,theta_hall=15;
volatile uint32_t photo_cycle=1,hall_cycle=1;
uint32_t hold_theta_photo,hold_theta_hall;
//************
uint8_t button1_vlaue = LOW, program_mode = 1, switch_input = LOW;
volatile boolean choose_mode_flag=true;
// try to use accerleration array
float accel_array[5],speed_array[5];
uint8_t accel_counter,speed_counter;
float new_accel=0;
int divide_number=0;
int cycle_hoder;
//*******************Functions prototypes

void photo_sens_interrupt();
/******/
void hall_sens_interrupt();
/*****/
uint16_t checkCounter(uint16_t counter1, uint8_t maxV);
void sPrint(String to_pr_c, float valo);
void stopSerial(uint8_t checkSerial);
void checkStartCondtions(uint8_t hall_seco, uint8_t hoto_cso);
uint8_t chooseMode();
uint8_t getTargetSection(float theta_target_1);
//**********
void getAcceleration(char x_sens);
//*******
void applyMode();
void readMode();
// void calculateTime(float accelaration_2, float winkelgeschwindigkeit_3, float inittheta_2, char control_1);
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
//TODO update the speed and acceleration function delete the array and work without it  
 applyMode();
 readMode();
 checkStartCondtions(hall_section,photo_section);
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
         getAcceleration('p');
          start_excu_time=millis();
          while (photo_section!=0)
          {
            delay(1);
          }
          Serial.println("Goooooooooo");
          hold_delta_photo_sensor=time_delta_photo;
          hold_position=photo_section;
          hold_test_position=hall_section;
          hold_theta_photo=theta_photo;
          cycle_hoder=photo_cycle;
          divide_number=0;
          // get the average accel value 
          for (int i=0;i<5;i++)
          {
            if(accel_array[i]!=0)
            {
              new_accel+=accel_array[i];
              divide_number+=1;
            }
          }
          new_accel=new_accel/divide_number;
          
          
          time_total_photo_equation=speed_main.totalPhotoTime(hold_delta_photo_sensor,new_accel);
          time_rest_to_null_equation=speed_main.photoRst(hold_position,hold_delta_photo_sensor,new_accel);
          // new way to calculate the time using the angel in degree 
          angular_speed=speed_main.photoSpeed(hold_delta_photo_sensor)+(new_accel*(hold_delta_photo_sensor/1000));
          if (angular_speed<12)
          {   
              time_rest_to_null_speed=1000*((2*PI-(hold_position*(PI/6)))/angular_speed);
              time_total_photo_speed=1000*(2*PI/angular_speed);
          }
          else if (angular_speed>12 && angular_speed<25)
          {
            time_rest_to_null_speed=1000*((4*PI-(hold_position*(PI/6)))/angular_speed);
            time_total_photo_speed=1000*(4*PI/angular_speed);
          }
          else if (angular_speed>25)
          {
            time_rest_to_null_speed=1000*((6*PI-(hold_position*(PI/6)))/angular_speed);
            time_total_photo_speed=1000*(6*PI/angular_speed);
          }
          time_used_to_calc=0;
          //*******************************
          //********************************
           if (time_total_photo_equation ==0)
          {
            time_total_photo_equation=time_total_photo_speed;
            Serial.println("the total eqaution ==0 >> using total speed");
          }
          shootMain(angular_speed,hold_position,photo_section,time_total_photo_equation,time_rest_to_null_equation-time_used_to_calc,hold_delta_photo_sensor);
          // sPrint("end time  exucte and print  ",millis()-start_excu_time,"ms");
          break;
      
      break;
      // here we are using the angel to calculate the values 
      case 3:
          getAcceleration('p');
          while (photo_section!=0)
          {
            delay(1);
          }
          Serial.println("Goooooooooo");
          hold_delta_photo_sensor=time_delta_photo;
          hold_position=photo_section;
          hold_test_position=hall_section;
          hold_theta_photo=theta_photo;
          cycle_hoder=photo_cycle;
          divide_number=0;
          // get the average accel value 
          start_excu_time=millis();
          for (int i=0;i<5;i++)
          {
            if(accel_array[i]!=0)
            {
              new_accel+=accel_array[i];
              divide_number+=1;
            }
          }
          new_accel=new_accel/divide_number;  
          time_total_photo_equation=speed_main.totalPhotoTime(hold_delta_photo_sensor,new_accel);
          time_rest_to_null_equation=speed_main.photoRst(hold_position,hold_delta_photo_sensor,new_accel);
          // new way to calculate the time using the angel in degree 
          angular_speed=speed_main.photoSpeed(hold_delta_photo_sensor)+(new_accel*(hold_delta_photo_sensor/1000));
          if (angular_speed<12)
          {   
              time_rest_to_null_speed=1000*((2*PI-(hold_position*(PI/6)))/angular_speed);
              time_total_photo_speed=1000*(2*PI/angular_speed);
          }
          else if (angular_speed>12 && angular_speed<25)
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
          // sPrint("end time  exucte and print  ",millis()-start_excu_time,"ms");
          break;
    case 4:
          //TODO degree time caculate function using the theta angel
      for (int k=0;k<5;k++)
      {
          getAcceleration('p');
          start_excu_time=millis();
          // Serial.println("we are in ");
          // Serial.println("time to print one line using Serial is ");
          // Serial.println(millis()-start_excu_time);
          //cli();
          hold_delta_photo_sensor=time_delta_photo;
          hold_position=photo_section;
          hold_test_position=hall_section;
          new_accel=0;
          divide_number=0;
          for (int i=0;i<5;i++)
          {
            if(accel_array[i]!=0)
            {
              new_accel+=accel_array[i];
              sPrint("accel_array[i]",accel_array[i]);
              divide_number+=1;
              sPrint("new accel from loop",new_accel);
              sPrint("divide_number",divide_number);
            }
          }
          new_accel=new_accel/divide_number;
          time_total_photo_equation=speed_main.totalPhotoTime(hold_delta_photo_sensor,new_accel);
          time_rest_to_null_equation=speed_main.photoRst(hold_position,hold_delta_photo_sensor,new_accel);
          angular_speed=speed_main.photoSpeed(hold_delta_photo_sensor)+(new_accel*(hold_delta_photo_sensor/1000));
          if (angular_speed<12)
          {   
              time_rest_to_null_speed=1000*((2*PI-(hold_position*(PI/6)))/angular_speed);
              time_total_photo_speed=1000*(2*PI/angular_speed);
          }
          else if (angular_speed>12 && angular_speed<25)
          {
            time_rest_to_null_speed=1000*((4*PI-(hold_position*(PI/6)))/angular_speed);
            time_total_photo_speed=1000*(4*PI/angular_speed);
          }
          else if (angular_speed>25)
          {
            time_rest_to_null_speed=1000*((6*PI-(hold_position*(PI/6)))/angular_speed);
            time_total_photo_speed=1000*(6*PI/angular_speed);
          }
          time_used_to_calc=millis()-start_excu_time;
          // time_used_to_calc=0;
          shootMain(angular_speed,hold_position,photo_section,(time_total_photo_speed-time_used_to_calc),time_rest_to_null_speed-time_used_to_calc,hold_delta_photo_sensor);
          delay(5000);
            }
          
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
  theta_photo+=30;
  // photo_section = checkCounter(photo_section, 12);
   photo_section+=1;
  //  Serial.print(theta_photo);
  //  Serial.print("   ");
  //  Serial.println(theta_photo);
  //  sPrint("photo_section ",photo_section,"");
  //   sPrint("theta_photo ",theta_photo," degree");
  //   sPrint("photo_cycle ",photo_cycle," cycle");
  if (photo_section==12)
  {
    photo_section=0;
    photo_cycle+=1;
  }
}

//************** HALL SENS INTERRUPT *********
void hall_sens_interrupt()
{
  time_delta_hall = millis() - hall_start;
  hall_start = millis();
  hall_section+=1;
  theta_hall+=180;
  // sPrint("hall_section ",hall_section,"");
  // sPrint("theta_hall ",theta_hall," degree");
  // sPrint("hall_cycle ",hall_cycle," cycle");
  if (hall_section==2)
  {
    hall_section=0;
    hall_cycle+=1;
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
void stopSerial(uint8_t checkSerial)
{
  if (checkSerial == HIGH)
  {
    Serial.end();
  }
}

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
    speed_1=speed_main.photoSpeed(time_delta_photo);
    delay(time_delta_photo);
    speed_2=speed_main.photoSpeed(time_delta_photo);
    angular_acceleration=-abs(1000*(speed_2-speed_1))/time_delta_photo;
    accel_array[accel_counter]=angular_acceleration;
    accel_counter=checkCounter(accel_counter,5);
      break;
    case 'h':
      speed_1 = speed_main.hallSpeed(time_delta_hall);
      delay(time_delta_hall);
      speed_2 = speed_main.hallSpeed(time_delta_hall);
      angular_acceleration = -abs(1000 * (speed_2 - speed_1) / time_delta_hall);
      accel_array[accel_counter] = angular_acceleration;
      accel_counter = checkCounter(accel_counter, 5);
      Serial.println(angular_acceleration);
      break;
    } 
}

uint8_t getTargetSection(float theta_target_1)
{
  uint8_t section_help=6*theta_target_1/PI;
  sPrint("section_help",section_help);
  if (section_help > 11)
  {
    return section_help-12;
  }
  else
  {
    return section_help;
  }
  
}
void applyMode()
{
  if (choose_mode_flag== true)
  {
    program_mode=chooseMode();
    sPrint(" we are in the mode ", program_mode);
    choose_mode_flag= false;
  }
}
void readMode()
{
      if (digitalRead(hartz.butt1) == HIGH && millis() - butt1_press_delay_read_mod > 30)
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
      if (current_section!=pos_holder && pos_holder !=11 )
      {
        time_correction_value=1000*(abs(current_section-pos_holder)*((PI/6)/ang_speed));
      }
      else if (current_section!=pos_holder && pos_holder ==11)
      {
        time_correction_value=1000*(((current_section+1)*(PI/6)/ang_speed));
      }
      else
      {
        time_correction_value=0;
      }
      uint16_t time_photo_cor=1000*(hold_test_position*PI/6)/ang_speed;
      uint16_t angel_15_correction=1000*(15*PI/180)/ang_speed;

      uint32_t new_rest_time;
      uint16_t new_timetarget;
      if (program_mode==1)
      {
        new_rest_time=rest_time-time_photo_cor;
        if(ang_speed >12)
        {
            new_timetarget=time_fall+100+angel_15_correction;
        }
        else
        {
          new_timetarget=time_fall+100;
        }
        
      }
      else
      {
        new_rest_time=rest_time-time_correction_value;
         if(ang_speed >12)
        {
            new_timetarget=time_fall+100-angel_15_correction;
        }
        else
        {
          new_timetarget=time_fall+100-angel_15_correction;
        }
      }
      
      shoot_main.fireBall(total_time,pos_holder,new_rest_time,delta_hoder,new_timetarget);
      // Debugging using the serial print
      uint32_t printer_timer2=millis();
      sPrint("***** after shoot values ",1);
      sPrint("accel",angular_acceleration);
      sPrint("avg_accel  ",new_accel);
      sPrint("accel_array_member  ",divide_number);
      sPrint("w rad/s ",ang_speed);
      sPrint("time_correction_value ",time_correction_value);
      sPrint("time_photo_cor ",time_photo_cor);
      sPrint("angel_15_csorrection ",angel_15_correction);
      sPrint("***** after shoot values ",2);
      sPrint("time_rest used here ",rest_time);
      sPrint("time_total_used here ",total_time);
      sPrint("rest_equation ",time_rest_to_null_equation);
      sPrint("total_equation ",time_total_hall);
      sPrint("rest_speed ",time_rest_to_null_speed);
      sPrint("total_speed_photo ",time_total_photo_speed);
      sPrint("total_speed_hall ",time_total_hall_speed);
      sPrint("hold_position",pos_holder);
      sPrint("current position ",current_section);
      sPrint("check the other sensr value ",hold_test_position);
      sPrint("***** End shoot print tme ",(millis()-printer_timer2));
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
    return (1);
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
