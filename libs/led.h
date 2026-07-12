#include <stdio.h>
#include <string.h>
#include <avr/io.h>
#include <usart.h>
#include <util/delay.h>

#define __DELAY_BACKWARD_COMPATIBLE__
#define NUMBER_OF_LEDS 4

void enableLed(int led);
void enableLeds(uint8_t leds);
void enableAllLeds();

void lightUpLed(int led);
void lightUpLeds(uint8_t leds);
void lightUpAllLeds();

void lightDownLed(int led);
void lightDownLeds(uint8_t leds);
void lightDownAllLeds();

void toggleLeds(uint8_t leds, int status);

void dimLed(int led, int percent, int duration);

void fadeInLed(int led, int duration);