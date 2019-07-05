#include <Arduino.h>
#include "kisg6.h"
#include "Servo.h"
#include "speed.h"
#include "debug.h"
#include "Shoot.h"
#include "math.h"
//********* the Constants that we may used alot
#define PI 3.1415926535897932384626433832795
 // the time aus FreienFall Gesetz mit s=0.75m und g= 9.81
float time_fall=1000* sqrt((2*0.75)/9.81); // the result will be in 391.030 ms 

const uint16_t test_time = 400; // the new value for photo sensor
const uint16_t test_time_hall=400;

//************** Objects from the main Classes  ******************//

speed speed_main; // from The Class Speed that will help in calculate every important value (speed, time, time rest, time total......)
kisg6 hartz;  // from Class KISG6 that contain the experement Conditions (pins Setup )
debug debugger_main;  // from Debug class the main use is to Serial print complexe Phrases
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
float time_total_photo, time_total_hall; // this values will be used to get the best shoot moment
float time_rest_photo, time_rest_hall;     // it will be generated using Speed Class
float best_speed, best_total_time, best_rest, time_rest_to_null;
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
 int time_total_photo_test, time_rest_to_null_test;
//****************
uint32_t fill_start_timer=0;
//uint16_t time_correction_value;
int time_used_to_calc;
//try to add the angel from interrupt 
volatile uint32_t theta_photo=15,theta_hall=15;
volatile uint32_t photo_cycle=1,hall_cycle=1;
uint32_t hold_theta_photo,hold_theta_hall;
//************
uint8_t button1_vlaue = LOW, program_mode = 1, switch_input = LOW;
volatile boolean choose_mode_flag=true,shot_flag_holder=false;

// try to use accerleration array
float accel_array[5],speed_array[5];
  int accel_counter,speed_counter;
float new_accel=0;
int divide_number=0;
//*******************Functions prototypes

void photo_sens_interrupt();
/******/
void hall_sens_interrupt();
/*****/
uint16_t checkCounter(uint16_t counter1, uint8_t maxV);

void stopSerial(uint8_t checkSerial);
void checkStartCondtions(uint8_t hall_seco, uint8_t hoto_cso);
uint8_t chooseMode();
uint8_t getTargetSection(float theta_target_1);
//**********
void fillSpeed();
void getAcceleration();
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
  // main_motor.attach(9);
  // main_motor.write(0);
  Serial.begin(57600);
  hall_start = millis();
  photo_start = millis();
}

//********************************* the main loop ****************
void loop()
{
  
  checkStartCondtions(hall_section,photo_section);
  /**What is probably happening is that the variables are being changed by 
   * the interrupt routines mid-way through the calculations.My 'fix' reduces 
   * the time spent doing the calculation with the volatile
   * variables thus making it less likely that there is a collision.
   * What I did is copy the volatile variables to local variables with interrupts disabled
     for that brief period.
      using the cli() sei() functions;
   * ***/
//TODO update the speed and acceleration function delete the array and work without it  
//  Serial.print(theta_photo);
//  Serial.print(" ");
//  Serial.println(time_delta_photo);
 applyMode();
 readMode();
 //debugger_main.sPrint("first millis ",millis(),"ms");
 if (millis()>5000)
 {
    getAcceleration();
 }
  //************************************************ the main part ******************************************************************

  if (digitalRead(hartz.trigger) == HIGH &&  millis() - press_delay_trigger > 300)
  {
    //********************
    Serial.println(" trigger pressed 1 ");
    Serial.println(photo_section);
    
    switch (program_mode)
    {
      /** nur Hall sensor benutzen **/
    case 1:
    // while (hall_section!=1)
    // {
    //   Serial.println(" wait to change ");
    //   delay(100);
    // }
    
    getAcceleration();
    start_excu_time=millis();
     cli();
      hold_delta_hall_sensor=time_delta_hall;
      hold_position=hall_section;
      hold_test_position=photo_section;
      sei();
      time_total_hall=speed_main.totalHallTime(hold_delta_hall_sensor,angular_acceleration);
      time_rest_to_null=speed_main.hallRst(hold_position,hold_delta_hall_sensor,angular_acceleration);
      angular_speed=speed_main.hallSpeed(time_delta_hall)+(angular_acceleration*(hold_delta_hall_sensor/1000));
      time_used_to_calc=-start_excu_time+millis();
      shootMain(angular_speed,hold_position,hall_section,time_total_hall-(time_used_to_calc),time_rest_to_null-(time_used_to_calc),hold_delta_hall_sensor);
      debugger_main.sPrint("time_used_to_calc",time_used_to_calc,"ms");
      debugger_main.sPrint("time_rest_to_null-1",time_rest_to_null,"ms");
      debugger_main.sPrint("time_total_hall-1",time_total_hall,"ms");
      break;
      /** nur PHOTO sensor benutzen **/

    case 2:
    getAcceleration();
      // the data that will be used in the shoot function
      start_excu_time=millis();
      cli();
      hold_delta_photo_sensor=time_delta_photo;
      hold_position=photo_section;
      sei();
      angular_speed=speed_main.photoSpeed(hold_delta_photo_sensor)+(angular_acceleration*(hold_delta_photo_sensor/1000));
      //calculateTime(angular_acceleration,angular_speed,(photo_section*(PI/6)),'t');
      time_total_photo=speed_main.totalPhotoTime(hold_delta_photo_sensor,angular_acceleration);
      time_rest_to_null=speed_main.photoRst(hold_position,hold_delta_photo_sensor,angular_acceleration);
      time_used_to_calc=-start_excu_time+millis();
      shootMain(angular_speed,hold_position,photo_section,time_total_photo-time_used_to_calc,time_rest_to_null-time_used_to_calc,hold_delta_photo_sensor);
      /* code */
      
      break;
      // here we are using the angel to calculate the values 
      case 3:
          getAcceleration();
          start_excu_time=millis();
          // Serial.println("we are in ");
          // Serial.println("time to print one line using Serial is ");
          // Serial.println(millis()-start_excu_time);
          //cli();
          while (hall_section!=0)
          {
            delay(1);
            Serial.println("goooooooooooooo");
          }
          
          hold_delta_photo_sensor=time_delta_photo;
          hold_position=photo_section;
          hold_test_position=hall_section;
          hold_theta_photo=theta_photo;
          Serial.print("target theta from shhot  : ");
          Serial.println((photo_cycle)*2*PI);
          Serial.print("photo_section from shhot  : ");
          Serial.println(hold_position);
          Serial.print("hold_theta_photo from shhot  : ");
          Serial.println(hold_theta_photo);
          for (int i=0;i<5;i++)
          {
            if(accel_array[i]!=0)
            {
              new_accel+=accel_array[i];
              debugger_main.sPrint("accel_array[i]",accel_array[i],"sss");
              divide_number+=1;
              debugger_main.sPrint("new accel from loop",new_accel,"sss");
              debugger_main.sPrint("divide_number",divide_number,"sss");
            }
          }
          new_accel=new_accel/divide_number;
          
          Serial.print("new_accel from shhot  : ");
          Serial.println(new_accel);
          //sei();
          print_timer_start=millis();
          time_total_photo_test=speed_main.totalPhotoTime(hold_delta_photo_sensor,new_accel);
          time_rest_to_null_test=speed_main.photoRst(hold_position,hold_delta_photo_sensor,new_accel);
          // new way to calculate the time using the angel in degree 
          //time_rest_to_null_test=speed_main.photoRst(hold_theta_photo,hold_delta_photo_sensor,new_accel);
          angular_speed=speed_main.photoSpeed(hold_delta_photo_sensor)+(new_accel*(hold_delta_photo_sensor/1000));
          if (angular_speed<12)
          {   
              time_rest_to_null=1000*((2*PI-(hold_position*(PI/6)))/angular_speed);
              time_total_photo=1000*(2*PI/angular_speed);
          }
          else if (angular_speed>12 && angular_speed<25)
          {
            time_rest_to_null=1000*((4*PI-(hold_position*(PI/6)))/angular_speed);
            time_total_photo=1000*(4*PI/angular_speed);
          }
          else if (angular_speed>25)
          {
            time_rest_to_null=1000*((6*PI-(hold_position*(PI/6)))/angular_speed);
            time_total_photo=1000*(6*PI/angular_speed);
          }
          time_used_to_calc=0;

          //*******************************

          //********************************
          shootMain(angular_speed,hold_position,photo_section,(time_total_photo-time_used_to_calc),time_rest_to_null-time_used_to_calc,hold_delta_photo_sensor);
          debugger_main.sPrint("time_total_photo normal ",time_total_photo,"ms");
          debugger_main.sPrint("time_rest_to_null normal ",time_rest_to_null,"ms");
          // debugger_main.sPrint("end time  exucte and print  ",millis()-start_excu_time,"ms");
          break;
    case 4:
          //TODO degree time caculate function using the theta angel
      for (int k=0;k<5;k++)
      {
          getAcceleration();
          // start_excu_time=millis();
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
              debugger_main.sPrint("accel_array[i]",accel_array[i],"sss");
              divide_number+=1;
              debugger_main.sPrint("new accel from loop",new_accel,"sss");
              debugger_main.sPrint("divide_number",divide_number,"sss");
            }
          }
          new_accel=new_accel/divide_number;
          time_total_photo_test=speed_main.totalPhotoTime(hold_delta_photo_sensor,new_accel);
          time_rest_to_null_test=speed_main.photoRst(hold_position,hold_delta_photo_sensor,new_accel);
          angular_speed=speed_main.photoSpeed(hold_delta_photo_sensor)+(new_accel*(hold_delta_photo_sensor/1000));
          if (angular_speed<12)
          {   
              time_rest_to_null=1000*((2*PI-(hold_position*(PI/6)))/angular_speed);
              time_total_photo=1000*(2*PI/angular_speed);
          }
          else if (angular_speed>12 && angular_speed<25)
          {
            time_rest_to_null=1000*((4*PI-(hold_position*(PI/6)))/angular_speed);
            time_total_photo=1000*(4*PI/angular_speed);
          }
          else if (angular_speed>25)
          {
            time_rest_to_null=1000*((6*PI-(hold_position*(PI/6)))/angular_speed);
            time_total_photo=1000*(6*PI/angular_speed);
          }
          //time_used_to_calc=millis()-start_excu_time;
          time_used_to_calc=0;
          shootMain(angular_speed,hold_position,photo_section,(time_total_photo-time_used_to_calc),time_rest_to_null-time_used_to_calc,hold_delta_photo_sensor);
          delay(5000);
            }
          
      break;
    }
    press_delay_trigger = millis();
  }
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
  //  debugger_main.sPrint("photo_section ",photo_section,"");
  //   debugger_main.sPrint("theta_photo ",theta_photo," degree");
  //   debugger_main.sPrint("photo_cycle ",photo_cycle," cycle");
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
  // debugger_main.sPrint("hall_section ",hall_section,"");
  // debugger_main.sPrint("theta_hall ",theta_hall," degree");
  // debugger_main.sPrint("hall_cycle ",hall_cycle," cycle");
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
void getAcceleration()
{

 
    // speed_1=speed_main.hallSpeed(time_delta_hall);
    speed_1=speed_main.photoSpeed(time_delta_photo);
    // debugger_main.sPrint("speed_1",speed_1,"rad/sec");
    delay(time_delta_photo);
    // delay(time_delta_hall);
    // speed_2=speed_main.hallSpeed(time_delta_hall);
    speed_2=speed_main.photoSpeed(time_delta_photo);
    // debugger_main.sPrint("speed_2",speed_2,"rad/sec");
    // debugger_main.sPrint("time_delta_photo 2",time_delta_photo,"ms");
    // angular_acceleration=-abs(1000*(speed_2-speed_1)/time_delta_hall);
    angular_acceleration=-abs(1000*(speed_2-speed_1))/time_delta_photo;
    accel_array[accel_counter]=angular_acceleration;
    accel_counter=checkCounter(accel_counter,5);
    //TODO make time delta array and test 
    // debugger_main.sPrint("the angular acceleration",angular_acceleration,"rad/s2");  
    // debugger_main.sPrint("speed_1 ",speed_1,"rad/s");  
    // debugger_main.sPrint("speed_2 ",speed_2,"rad/s");  
    // debugger_main.sPrint("time_delta_photo ",time_delta_photo,"ms");  
  
}
//check speed every delta ms
// void fillSpeed()
// {
//   //debugger_main.sPrint("second millis ",millis(),"ms");
//   filled_speed=speed_main.photoSpeed(hold_delta_photo_sensor);
//   //debugger_main.sPrint("third millis ",millis(),"ms");
//   acceleration_timer_0=millis();
// }

uint8_t getTargetSection(float theta_target_1)
{
  uint8_t section_help=6*theta_target_1/PI;
  debugger_main.sPrint("section_help",section_help,"");
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
    debugger_main.sPrint(" we are in the mode ", program_mode, " ");
    choose_mode_flag= false;
  }
}
void readMode()
{
      if (digitalRead(hartz.butt1) == HIGH && millis() - butt1_press_delay_read_mod > 1000)
  {
    program_mode = chooseMode();
    debugger_main.sPrint(" we are in the mode ", program_mode, " ");
    butt1_press_delay_read_mod = millis();
  }
}

void shootMain(float ang_speed, uint8_t pos_holder,uint8_t current_section, uint16_t total_time,uint16_t rest_time,uint16_t delta_hoder)
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
      int angel_15_correction=1000*(15*PI/180)/ang_speed;
      int shoot_timer=millis();
      shoot_main.fireBall(delta_hoder,total_time-time_correction_value,pos_holder,
      rest_time-time_correction_value,delta_hoder,time_fall+angel_15_correction);
      Serial.print(" motor timer  ");
      Serial.println(millis()-shoot_timer);

      // shoot the ball using the calculated values 
  //   if (rest_time > time_fall+angel_15_correction)
  // {
  //   int delay_time_shoot = rest_time-time_fall-time_correction_value;
  //   if (delay_time_shoot < 15000)
  //   {
  //     Serial.print(" motor timer  ");
  //     Serial.println(millis()-shoot_timer);
  //     delay(delay_time_shoot);
  //     main_motor.write(20);
  //     delay(100);
  //     main_motor.write(0);
  //   }
  // }
      debugger_main.sPrint("time_delta_photo ",time_delta_photo,"ms");
      debugger_main.sPrint("angular_acceleration ",angular_acceleration,"rad/s2");
      debugger_main.sPrint("angular_speed ",ang_speed,"rad/s");
      debugger_main.sPrint("time_correction_value ",time_correction_value,"ms");
      debugger_main.sPrint("angel_15_correction ",angel_15_correction,"ms");
      debugger_main.sPrint("time_rest_to_null_test ",rest_time+time_used_to_calc,"ms");
      debugger_main.sPrint("time_total_photo_test ",total_time+time_used_to_calc,"ms");
      debugger_main.sPrint("photo section ",current_section,"");
      debugger_main.sPrint("hold_position",pos_holder,"");
      debugger_main.sPrint("hold_position_test",hold_test_position,"");
}