#define __DELAY_BACKWARD_COMPATIBLE__
#include <stdio.h>
#include <string.h>
#include <avr/io.h>
#include <usart.h>
#include <util/delay.h>


#define NUMBER_OF_LEDS 4
//functie da parameter verwacht tussen 1 en 3 en da pinnetje enabelen
void enableLed(int led){
  if (led >= NUMBER_OF_LEDS || led < 0)
  return;
  DDRB |= (1 << (PB2 + led));
}
void enableLeds(uint8_t leds){
  DDRB = (leds << PB2);
}
void enableAllLeds(){
  enableLeds(0b00001111);
}

void lightUpLed(int led){
  if (led >= NUMBER_OF_LEDS || led < 0)
  return;
  PORTB &= ~(1 << (PB2 + led));
}
void lightUpLeds(uint8_t leds){
  PORTB &= (leds << PB2);
}
void lightUpAllLeds(){
  lightUpLeds(0b00000000);
}

void lightDownLed(int led){
  if (led >= NUMBER_OF_LEDS || led < 0)
  return;
  PORTB |= (1 << (PB2 + led));
  
}
void lightDownLeds(uint8_t leds){
  PORTB = (leds << PB2);
}
void lightDownAllLeds(){
  lightDownLeds(0b00001111);
}
//0b00000001 --> 1 toggles status led
void toggleLeds(uint8_t leds, int status){
  if (status == 1){
    PORTB ^= (leds << PB2);
  }else if (status == 0){
    PORTB |= (leds << PB2);
  }
}

void dimLed(int led, int percent, int duration){
  for (int i = duration; i > 0; i -= 10)
  {
    lightUpLed(led);
    _delay_ms(percent / 10);
    lightDownLed(led);
    _delay_ms((100 - percent)/10);
  }
  
  
}

void fadeInLed(int led, int duration){
  for (int j = 1; j <= 100; j++)
  {
    dimLed(led, j, duration / 100);
  }
  lightUpLed(led);
}