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
kisg6 demo;  // from Class KISG6 that contain the experement Conditions (pins Setup )
debug debugger_main;  // from Debug class the main use is to Serial print complexe Phrases
Shoot shoot_main; // from Shoot Class used to shhot the ball 

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
uint8_t hold_position; // for the delta time 
//********** Recive the Time delta from Interrupt 
uint16_t hold_delta_photo_sensor,hold_delta_hall_sensor,time_target;
//**************************
uint16_t delay_time;

//************ New Calculations using Theata 
float theta_zero,angular_speed,angular_speed_zero,angular_acceleration=0.000000000000;

// new speed var
float filled_speed=0,speed_1,speed_2;

//** the main timers to calculate the acceleration 
uint32_t acceleration_timer_1=0;
uint32_t acceleration_timer_0=0;
//**** these timer wil be used the time to excute a part from the code 
uint32_t start_excu_time=0,end_excu_time=0;
//****************
uint32_t fill_start_timer=0;
//uint16_t time_correction_value;

//************
uint8_t button1_vlaue = LOW, program_mode = 1, switch_input = LOW;
boolean choose_mode_flag=true,shot_flag_holder=false;

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
  demo.pinSetup();
  attachInterrupt(digitalPinToInterrupt(demo.photosens), photo_sens_interrupt, CHANGE);
  attachInterrupt(digitalPinToInterrupt(demo.hallsens), hall_sens_interrupt, CHANGE);
  shoot_main.motorIntil();
  Serial.begin(57600);
  hall_start = millis();
  photo_start = millis();
}

//********************************* the main loop ****************
void loop()
{
  
  /**What is probably happening is that the variables are being changed by 
   * the interrupt routines mid-way through the calculations.My 'fix' reduces 
   * the time spent doing the calculation with the volatile
   * variables thus making it less likely that there is a collision.
   * What I did is copy the volatile variables to local variables with interrupts disabled
     for that brief period.
      using the cli() sei() functions;
   * ***/
//.TODO update the speed and acceleration function delete the array and work without it  
 applyMode();
 readMode();
 //debugger_main.sPrint("first millis ",millis(),"ms");
 if (millis()>5000)
 {
    getAcceleration();
 }
delay(500);


  //************************************************ the main part ******************************************************************

  if (digitalRead(demo.trigger) == HIGH &&  millis() - press_delay_trigger > 2000 && hall_section==0)
  {
    //********************
    Serial.println(" trigger pressed 1 ");
    switch (program_mode)
    {
      /** nur Hall sensor benutzen **/
    case 1:
    getAcceleration();
     cli();
      hold_delta_hall_sensor=time_delta_hall;
      hold_position=hall_section;
      angular_speed=angular_speed_zero+(angular_acceleration*(hold_delta_hall_sensor/1000));
      // calculateTime(angular_acceleration,angular_speed,(hall_section*(PI)),'t');
      time_total_hall=speed_main.totalHallTime(hold_delta_hall_sensor,angular_acceleration);
      time_rest_to_null=speed_main.hallRst(hold_position,hold_delta_hall_sensor,angular_acceleration);
      sei();
      shootMain(angular_speed,hold_position,hall_section,time_total_hall,time_rest_to_null,hold_delta_hall_sensor);
      break;
      /** nur PHOTO sensor benutzen **/

    case 2:
    getAcceleration();
      // the data that will be used in the shoot function
      cli();
      hold_delta_photo_sensor=time_delta_photo;
      hold_position=photo_section;
      sei();
      angular_speed=speed_main.photoSpeed(hold_delta_photo_sensor)+(angular_acceleration*(hold_delta_photo_sensor/1000));
      //calculateTime(angular_acceleration,angular_speed,(photo_section*(PI/6)),'t');
      time_total_photo=speed_main.totalPhotoTime(hold_delta_photo_sensor,angular_acceleration);
      time_rest_to_null=speed_main.photoRst(hold_position,hold_delta_photo_sensor,angular_acceleration);
      shootMain(angular_speed,hold_position,photo_section,time_total_photo,time_rest_to_null,hold_delta_photo_sensor);
      /* code */
      break;

      // here we are using the angel to calculate the values 
      case 3:
      getAcceleration();
    //start_excu_time=millis(); // strat timing for excute 
    cli();
    hold_delta_photo_sensor=time_delta_photo;
    hold_position=photo_section;
    //calculateTime(angular_acceleration,angular_speed,(photo_section*(PI/6)),'t');
    time_total_photo=speed_main.totalPhotoTime(hold_delta_photo_sensor,angular_acceleration);
    time_rest_to_null=speed_main.photoRst(hold_position,hold_delta_photo_sensor,angular_acceleration);
    debugger_main.sPrint("time_rest_to_null",time_rest_to_null,"ms");
    debugger_main.sPrint("time_total_photo",time_total_photo,"ms");
    sei();
    if (time_total_photo>0 && time_rest_to_null>0)
    {   angular_speed=speed_main.photoSpeed(hold_delta_photo_sensor)+(angular_acceleration*(hold_delta_photo_sensor/1000));
        shootMain(angular_speed,hold_position,photo_section,time_total_photo,time_rest_to_null,hold_delta_photo_sensor);
    }
    
        
      /** Manuel just let the ball go... **/
    case 4:
      //shoot_main.shootManuel();
      /* code */
      break;
    }
    press_delay_trigger = millis();
  }
}
































/****Choose Mode function****/
uint8_t chooseMode()
{
  switch_input = digitalRead(demo.switch_input);
  if (digitalRead(demo.butt1) == HIGH && millis() - butt1_press_delay_choose_mod > 500)
  {
    button1_vlaue = !button1_vlaue;
    butt1_press_delay_choose_mod = millis();
    Serial.println(button1_vlaue);
    digitalWrite(demo.led1, button1_vlaue);
  }

  if (switch_input == LOW)
  {
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
}
//************** PHOTO SENS INTERRUPT *********
void photo_sens_interrupt()
{
  time_delta_photo = millis() - photo_start;
  photo_start = millis();
  photo_section = checkCounter(photo_section, 12);
  //debugger_main.sPrint("photo section ",photo_section,"");
}

//************** HALL SENS INTERRUPT *********
void hall_sens_interrupt()
{
  time_delta_hall = millis() - hall_start;
  hall_start = millis();
  hall_section = checkCounter(hall_section, 2);
  if (hall_section==1)
  {
    photo_section=6;
  }
  else if (hall_section==0)
  {
    photo_section=0;
  }
  debugger_main.sPrint("hall_section ",hall_section,"");
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
    digitalWrite(demo.led2, HIGH);
    delay(150);
    digitalWrite(demo.led2, LOW);
  }
  else if (photo_sco != 0)
  {
    digitalWrite(demo.led2, HIGH);
    delay(150);
    digitalWrite(demo.led2, LOW);
  }
}
void getAcceleration()
{
 
    speed_1=speed_main.photoSpeed(time_delta_photo);
    // debugger_main.sPrint("time_delta_photo 1",time_delta_photo,"ms");
    delay(time_delta_photo);
    speed_2=speed_main.photoSpeed(time_delta_photo);
    // debugger_main.sPrint("time_delta_photo 2",time_delta_photo,"ms");
    angular_acceleration=-abs(1000*(speed_2-speed_1)/time_delta_photo);
    // debugger_main.sPrint("the angular acceleration",angular_acceleration,"rad/s2");  
    // debugger_main.sPrint("speed_1 ",speed_1,"rad/s");  
    // debugger_main.sPrint("speed_2 ",speed_2,"rad/s");  
  
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
      if (digitalRead(demo.butt1) == HIGH && millis() - butt1_press_delay_read_mod > 1000)
  {
    program_mode = chooseMode();
    debugger_main.sPrint(" we are in the mode ", program_mode, " ");
    butt1_press_delay_read_mod = millis();
  }
}
// void  calculateTime(float accelaration_1, float winkelgeschwindigkeit_1, float inittheta_1, char control)
// {
//   float c=-(6*PI-(inittheta_1));
//   float a= accelaration_1/2, b=winkelgeschwindigkeit_1;
//     float  x1, x2, discriminant,real_part,imaginary_part;
//     discriminant = (b*b)- (4*a*c);
//   if (control== 't')
//   {
//    if (a!=0){
//      debugger_main.sPrint("a",a,"");
//      debugger_main.sPrint("b",b,"");
//      debugger_main.sPrint("c",c,"");
//      debugger_main.sPrint("discriminant",discriminant,"");
//     if (discriminant > 0 && a !=0) {
//         x1 = (-b + sqrt(discriminant)) / (2*a);
//         x2 = (-b - sqrt(discriminant)) / (2*a);
//         debugger_main.sPrint( "Roots are real and different." ,0,"");
//         debugger_main.sPrint("t1 = ", x1 ,"") ;
//         debugger_main.sPrint("t2 = ", x2 ,"") ;
//         time_soluation[0]=1000*x1;
//         time_soluation[1]=1000*x2;
        
//     }
//     else if (discriminant == 0&& a !=0) {
//         debugger_main.sPrint( "Roots are real and same." ,0,"");
//         x1 = (-b + sqrt(discriminant)) / (2*a);
//         debugger_main.sPrint("t1 = t2 =", x1 ,"");
//         time_soluation[0]=1000*x1;
//         time_soluation[1]=500000;    
//     }
//     else {
//         debugger_main.sPrint("Roots are complex and different."  ,0,"");
//         real_part=-b/(2*a);
//         imaginary_part=sqrt(-discriminant)/(2*a);
//         x1=sqrt((real_part*real_part)+(imaginary_part*imaginary_part));
//         debugger_main.sPrint("real_part ", real_part ,"");
//         debugger_main.sPrint("imaginary_part", imaginary_part ,"");
//         debugger_main.sPrint("x1", x1 ,"");
//         time_soluation[0]=1000*x1;
//         time_soluation[1]=500000;
        
//     }

//    }
//    else
//    {
//     debugger_main.sPrint( "a == 0 " ,0,"");
//       debugger_main.sPrint("a",a,"");
//      debugger_main.sPrint("b",b,"");
//      debugger_main.sPrint("c",c,"");
//         x1 = -c/b;
//         debugger_main.sPrint("t1 = t2 =", x1 ,"");
//         time_soluation[0]=1000*x1;
//         time_soluation[1]=500000;
//    }
   
//   }
// }
void shootMain(float ang_speed, uint8_t pos_holder,uint8_t current_section, uint16_t total_time,uint16_t rest_time,uint16_t delta_hoder)
{
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
      
      shoot_main.fireBall(delta_hoder,rest_time,pos_holder,total_time,delta_hoder,time_fall-(time_delta_hall/6)+100);
      // shoot the ball using the calculated values 
      debugger_main.sPrint("Shooted from slow if  ",0,"");
      debugger_main.sPrint("angular_acceleration ",angular_acceleration,"rad/s2");
      debugger_main.sPrint("angular_speed ",ang_speed,"rad/s");
      debugger_main.sPrint("time_correction_value ",time_correction_value,"ms");
      debugger_main.sPrint("time_rest_to_null ",rest_time-time_correction_value,"ms");
      debugger_main.sPrint("time_total_photo ",total_time-time_correction_value,"ms");
      debugger_main.sPrint("time_fall-(time_delta_hall/6) ",time_fall+100,"ms");
      debugger_main.sPrint("photo section ",current_section,"");
      debugger_main.sPrint("hold_position",pos_holder,"");
}