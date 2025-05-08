#include "LED.h"
       	  

void LED_init()
{

    pinMode(LED_PIN, OUTPUT);
    
    digitalWrite(LED_PIN, LOW);

}

void LED_open()
{
    digitalWrite(LED_PIN, HIGH);
}

void LED_close()
{
    digitalWrite(LED_PIN, LOW);
}

