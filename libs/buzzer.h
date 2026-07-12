#define __DELAY_BACKWARD_COMPATIBLE__
#include <avr/io.h>
#include <usart.h>
#include <util/delay.h>
void enableBuzzer();

void playTone(float frequency, uint32_t duration);

void disableBuzzer();
void buzzerOn();