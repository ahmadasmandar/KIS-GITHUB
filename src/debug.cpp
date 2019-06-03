#include "debug.h"
#include <Arduino.h>

debug::debug()
{
    Serial.begin(9600);
}

void debug::sPrint(String to_pr_c, float valo, String einheit)
{
    Serial.println();
    Serial.print(to_pr_c);
    Serial.print("  ");
    Serial.print(valo);
    Serial.print("  ");
    Serial.print(einheit);
    Serial.println();
}