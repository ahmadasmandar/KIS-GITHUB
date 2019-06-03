#ifndef DEBUG_H
#define DEBUG_H
#include <Arduino.h>

class debug
{
private:
    /* data */
public:
    debug(/* args */);
    void sPrint(String to_pr, float val, String einheit);    
};

#endif