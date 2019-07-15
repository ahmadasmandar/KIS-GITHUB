//******** Written by Ahmad Asmanda 03.05.2019 **************
// 
#include "kisg6.h"
#include "Arduino.h"
//#include "Servo.h"

kisg6::kisg6()
{
    //INPUTS
    photosens=2;
    hallsens=3;
    trigger=4;
    switch_input=5;
    butt1=10;
    butt2=11;
    // OUTPUTS 
    bboxled=7;
    //servopin=9;
    led1=12;
    led2=13;
}
void kisg6::pinSetup(){

    // servo.attach(servopin);
    pinMode(photosens, INPUT);
    pinMode(hallsens, INPUT);
    pinMode(trigger, INPUT);
    pinMode(switch_input, INPUT);
    pinMode(butt1, INPUT);
    pinMode(butt2, INPUT);
    //OUTPUTS PINS einstellen
    pinMode(bboxled, OUTPUT);
    //pinMode(servopin, OUTPUT);
    pinMode(led1, OUTPUT);
    pinMode(led2, OUTPUT);

    Serial.begin(9600);

    }
    
    

// void kisg6::serialLogger()
// {
//     // nothing
// }
// // void kisg6::Cservo(int deg1)
// // {
// //     if (servo.attached())
// //     {
// //         servo.write(deg1);
// //         //servo.write(0);
// //     }
// //     else
// //     {
// //         attachServo();
// //         servo.write(deg1);
// //         Serial.println("Servo was not attached so i doit for you");
// //         delay(15);
// //     }
    
    
// // }
// // void kisg6::deatachServo()
// // {
// //     servo.detach();
// // }
// // void kisg6::attachServo()
// // {
// //     servo.attach(servopin);
// // }
// void kisg6::Demo()
// {

//     if (digitalRead(butt1)==HIGH)
//     {
//         Serial.print("Button 1 is pressed ; OK");
//     }
//     if (digitalRead(butt2)==HIGH)
//     {
//         Serial.println("Button 2 is pressed ; OK");
//     }
//     if (digitalRead(trigger)==HIGH)
//     {
//         Serial.println("trigger is pressed ; OK");
//     }
//     if (digitalRead(switch_input)==HIGH)
//     {
//         Serial.println("Switch is pressed ; OK");
//     }
//     else if (digitalRead(switch_input)==LOW)
//     {
//         Serial.println("Switch is not pressed ; OK");
//     }
//     digitalWrite(led1, HIGH);
//     delay(500);
//     digitalWrite(led2, HIGH);
//     delay(500);
//     digitalWrite(bboxled, HIGH);
//     delay(500);
//     digitalWrite(led1, LOW);
//     delay(500);
//     digitalWrite(led2, LOW);
//     delay(500);
//     digitalWrite(bboxled, LOW);
//     delay(500);
// }
// // void kisg6::Seth(int pinnumber)
// // {
// //     digitalWrite(pinnumber, HIGH);
// // }
// // void kisg6::SetL(int pinnumber)
// // {
// //     digitalWrite(pinnumber, LOW);
// // }
// // int kisg6::Get(int pinnumber)
// // {
// //     return digitalRead(pinnumber);
// // }

