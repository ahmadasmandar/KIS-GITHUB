#include <Arduino.h>
#include "kisg6.h"
#include "Servo.h"
#include "speed.h"
#include "debug.h"
#include "Shoot.h"
//********* the Constants that we may used alot
#define PI 3.1415926535897932384626433832795
 // the time aus FreienFall Gesetz mit s=0.73m und g= 9.81
const uint16_t test_time = 400; // the new value for photo sensor
const uint16_t test_time_hall=400;
//************** Objects from the main Classes  ******************//

speed spedo; // from The Class Speed that will help in calculate every important value (speed, time, time rest, time total......)
kisg6 demo;  // from Class KISG6 that contain the experement Conditions (pins Setup )
debug debo;  // from Debug class the main use is to Serial print complexe Phrases
Servo motor;
Shoot shooter;

// Here is the value for the Interrupts Counter for
// the PhotoSensor and The Hall Sensor
/*********  **********************/
uint8_t photo_section = 0, hall_section = 0; // the count wil Start from 0 for the beide Sensors
uint32_t photo_start, hall_start;            // these wil be used to calculate the delta time
/**************************************************************************************/
// **************  here is the Variabls which will be changed in the Interrupts ********//

volatile uint16_t time_delta_photo;
volatile uint16_t time_delta_hall;
volatile boolean shoot_flag=false;
/////
volatile float time_interrupt_photo, time_interrupt_hall; // these will hold the time rest values from the change point..

//********
uint16_t time_window_hall, time_window_photo;

// these just for help
uint16_t new_delay;

//******************************* speed parameter  S=73cm  **********************///

float photo_speed = 0, hall_speed = 0, new_speed = 0; // Speed from the tow sensor with new_speed to use it later
// float photo_acceleration;

float time_total_photo, time_total_hall; // this values will be used to get the best shoot moment
                                         //time_rest_photo, time_rest_hall;     // it will be generated using Speed Class

float best_speed, best_total_time, best_rest, time_rest_to_null;
/// ******* the Variable will be used in pet th eDebounces of the Button
uint32_t last_pressed = 0;
int triggerlastpressed = 0;
uint8_t hall_pos,pos,photo_pos;
uint16_t i_time = 0, hold_delta,time_target;
int time_array[2];
boolean start_flag = true;
float delay_time;

//************ New Calculations using Theata 
double theata,theta_zero,angular_speed,angular_speed_zero,angular_acceleration=0.000000000000;
float speed_array[2];
uint8_t j_speed=0,target_section;
double max_theta,theta_target;
uint16_t fill_timer=0;



//************
uint8_t button1_vlaue = LOW, program_mode = 1, switch_input = LOW;
boolean choose_mode_flag=true,shot_flag_holder=false;

//function prototypes

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
// end function prototypes

//****************** the setup *****************************

void setup()
{
  demo.pinSetup();
  shooter.motorIntil();
  Serial.begin(57600);
  attachInterrupt(digitalPinToInterrupt(demo.photosens), photo_sens_interrupt, CHANGE);
  attachInterrupt(digitalPinToInterrupt(demo.hallsens), hall_sens_interrupt, CHANGE);
  hall_start = millis();
  photo_start = millis();
}
// here is new test for using git hub

//********************************* the main loop ****************
void loop()
{

   applyMode();
 readMode();
  //******* fill the speed values every 1 second
  if (millis()-fill_timer > 1000)
  {
  fillSpeed();
  getAcceleration();
  fill_timer=millis();
  }
    // take the values from he inerrupt and used to fill the time array
  
  /**What is probably happening is that the variables are being changed by 
   * the interrupt routines mid-way through the calculations.My 'fix' reduces 
   * the time spent doing the calculation with the volatile
   * variables thus making it less likely that there is a collision.
   * What I did is copy the volatile variables to local variables with interrupts disabled
     for that brief period.
      using the cli() sei() functions;
   * ***/
  cli();
  hold_delta = time_delta_photo;
  time_array[i_time] = hold_delta;
  sei();
  start_flag = spedo.secureMotion(time_array[1], time_array[0], start_flag); // after 5 second will this function works
  i_time = checkCounter(i_time, 2);                                        // further the ounter with 1 and check if he reached his max reset it
  stopSerial(digitalRead(demo.butt2));
  checkStartCondtions(hall_section, pos);
  if (digitalRead(demo.trigger) == HIGH &&  millis() - last_pressed > 2000)
  {
    Serial.println(" trigger pressed ");
    //********************
    cli();
    hold_delta = time_delta_photo;
    sei();
    time_window_photo = hold_delta;
    time_target =(393+hold_delta-(0.2*hold_delta)+100);
    
    max_theta=spedo.getThetavalues(hold_delta,time_target,angular_acceleration,'x');
    theta_target=spedo.getThetavalues(hold_delta,time_target,angular_acceleration,'t');
    target_section=getTargetSection(theta_target);
    angular_speed=spedo.photoSpeed(hold_delta);
    
  
    
    //*********************** print the values to test 
    debo.sPrint("hall section",hall_section,"");
    debo.sPrint("photo section",photo_section,"");
    debo.sPrint("hold_delta ",hold_delta,"ms");
    debo.sPrint("time_target ",time_target,"ms");
    debo.sPrint("time_window_photo ",time_window_photo,"ms");
    debo.sPrint("the angular speed ",angular_speed,"rad/s");
    debo.sPrint("angular_acceleration ",angular_acceleration,"rad/s2");
    debo.sPrint("max_theta ",max_theta,"rad");
    debo.sPrint("theta_target ",theta_target,"rad");
    debo.sPrint("target_section ",target_section,"");
    debo.sPrint("speed_array 1 ",speed_array[0],"rad/s");
    debo.sPrint("speed_array 2 ",speed_array[1],"rad/s");
    //
    switch (program_mode)
    {
      /** nur Hall sensor benutzen **/
    case 1:
      cli();
      hold_delta = time_delta_hall;
      pos = hall_section;
      sei();
      time_rest_to_null = spedo.hallRst(pos,hold_delta);
      time_total_hall = spedo.totalHallTime(hold_delta);
      time_window_hall = (hold_delta/ 6) - 20;
      shooter.fireBall(hold_delta, time_rest_to_null, pos, time_total_hall, time_window_hall, time_target);
      break;


      /** nur PHOTO sensor benutzen **/
    case 2:
      // the data that will be used in the shoot function
      cli();
      hold_delta = time_delta_photo;
      pos = photo_section;
      sei();
      time_rest_to_null = time_interrupt_photo;
      time_total_photo = spedo.totalPhotoTime(hold_delta);
      time_window_photo = (time_delta_photo) - 20;
      shooter.fireBall(hold_delta, time_rest_to_null, pos, time_total_photo, time_window_photo, time_target);
      /* code */
      break;

      case 3:
      cli();
      hold_delta = time_delta_photo;
      pos = photo_section;
      sei();
      time_total_photo = spedo.totalPhotoTime(hold_delta);
      theta_target=spedo.getThetavalues(hold_delta,time_target,angular_acceleration,'t');
      target_section=getTargetSection(theta_target);
        if (time_total_photo < time_target)
         {
           if (pos== target_section){
             shooter.shootManuel();
             Serial.print("Shooted manulay");
           }
              // debshoot.sPrint("FOURTH_IF 4 -  if photo_speed is ", spedo.photoSpeed(time_delta_photo), "rad/s");
            debo.sPrint("FOURTH_IF new_4_theta -  theta_target  is ", theta_target, "rad");
            debo.sPrint("FOURTH_IF new_4_theta -  target_section ", target_section, "");
         }
      /** Manuel just let the ball go... **/

    case 40:
      shooter.shootManuel();
      /* code */
      break;
    }
    stopSerial(digitalRead(demo.butt2));
    last_pressed = millis();
    debo.sPrint("time target",time_target,"ms");
  }
}
































/****Choose Mode function****/
uint8_t chooseMode()
{
  switch_input = digitalRead(demo.switch_input);
  if (digitalRead(demo.butt1) == HIGH && millis() - triggerlastpressed > 500)
  {
    button1_vlaue = !button1_vlaue;
    triggerlastpressed = millis();
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
}

//************** HALL SENS INTERRUPT *********
void hall_sens_interrupt()
{
  time_delta_hall = millis() - hall_start;
  hall_start = millis();
  //time_interrupt_hall = spedo.hallRst(hall_section, time_delta_hall);
  //time_window_hall = (time_delta_hall / 6) - 20;
  hall_section = checkCounter(hall_section, 2);
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
    angular_acceleration=(speed_array[1]-speed_array[0])/time_delta_photo;
}
void fillSpeed()
{
  cli();
  speed_array[j_speed]=spedo.photoSpeed(time_delta_photo);
  angular_speed_zero=spedo.photoSpeed(time_delta_photo);
  sei();
  j_speed=checkCounter(j_speed,2);
}

uint8_t getTargetSection(float theta_target_1)
{
  uint8_t section_help=6*theta_target_1/PI;
  debo.sPrint("section_help",section_help,"");
  if (section_help > 12)
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
    debo.sPrint(" we are in the mode ", program_mode, " ");
    choose_mode_flag= false;
  }
}
void readMode()
{
      if (digitalRead(demo.butt1) == HIGH && millis() - triggerlastpressed > 1000)
  {
    program_mode = chooseMode();
    debo.sPrint(" we are in the mode ", program_mode, " ");
    triggerlastpressed = millis();
  }
}