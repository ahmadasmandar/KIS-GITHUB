#include "debug.h"
#include <Arduino.h>

// debug::debug()
// // {
// //     Serial.begin(57600);
// // }

void debug::sPrint(String to_pr_c, float valo, String einheit)
{
    Serial.print(to_pr_c);
    Serial.print("  ");
    Serial.print(valo);
    Serial.print("  ");
    Serial.println(einheit);
}