#define __DELAY_BACKWARD_COMPATIBLE__
#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>
#include <usart.h>
#include <display.h>
#include <led.h>
#include <stdlib.h>
#include <stdio.h>
#include <buzzer.h>
#include <math.h>
#include <stdbool.h>
#include <time.h>
#define NUMBER_OF_LEDS 4
#define LED_PORT PORTB
#define BUTTON_PORT PORTC
#define BUTTON_PIN PINC
#define LED_DDR DDRB
#define BUTTON_DDR DDRC
#define BUTTON2 PC2
#define LED1 PB2
#define LED2 PB3
#define LED3 PB4
#define LED4 PB5

uint32_t counter = 0;
float currentSpeed = 100; //  snelheid - m/s  (meter per seconde)
float gravity = 1.622;    // versnelling – m/s² (
int distance = 9999;      // afstand tot het maanoppervlak in meter – m
int fuelReserve = 1500;   //liter
float burstFuelConsumption = 0;
int burstConsumption = 0;
int maximumFuelConsumptionPerSecond = 50;  // Maximum fuel consumption per second

struct LogEntry {
  int distance;
  int currentSpeed;
  int burstConsumption;
  int fuelReserve;
};
    

// Interrupt Service Routine for Timer Overflow (assuming Timer0 is used)
ISR(TIMER0_OVF_vect) {
  counter++;
}

// Define variables to store button press/release timestamps
int buttonPressTime = 0;
int lastButtonPressTime = 0;
int buttonReleaseTime = 0;

// Interrupt Service Routine for Pin Change Interrupt 1
ISR(PCINT1_vect) {
  // Check if the button is pressed or released
  if (bit_is_clear(BUTTON_PIN, BUTTON2)) {  // Button pressed
    if (buttonPressTime == 0)
    {
      buttonPressTime = counter;  // Record the button press timestamp
    }
  } else {  // Button released
  if (buttonPressTime != 0)
  {
    buttonReleaseTime = counter;  // Record the button release timestamp

    // Calculate the button press duration
    uint32_t buttonPressDuration = buttonReleaseTime - buttonPressTime;
    //printf("duration: %ld =, release: %d, - press: %d\n", buttonPressDuration, buttonReleaseTime, buttonPressTime);

    // Calculate burst fuel consumption klopt nog niet
    burstFuelConsumption = (buttonPressDuration / 1000.0) * maximumFuelConsumptionPerSecond;
    if (buttonPressDuration > 99)
    {
      for (int i = 0; i < floor(buttonPressDuration/100); i++)
      {
        burstFuelConsumption += maximumFuelConsumptionPerSecond;
      }
      float lastTwoDigits = buttonPressDuration % 100;
      if (lastTwoDigits != 0)
      {
        burstFuelConsumption += (lastTwoDigits / 100) * maximumFuelConsumptionPerSecond;
      }
    }else{
      burstFuelConsumption += ((float)buttonPressDuration / 100) * maximumFuelConsumptionPerSecond;
    }
    if (fuelReserve <= 0)
    {
      burstConsumption = 0;
    }else{
      burstConsumption = round(burstFuelConsumption);
    }
    
    burstConsumption = 50;
    //printf("burstfuel: %d\n", burstConsumption);
    //printf("Bpress: %d, Rel: %d\n", buttonPressTime, buttonReleaseTime);  
  }
  
  
    
    //Nieuwe situatie
    
    // Process the button press duration
    // (e.g., calculate burst fuel consumption based on duration)
  }
}

void initTimer0() {
 // STAP 1: kies de WAVE FORM en dus de Mode of Operation
    // Hier kiezen we FAST PWM waardoor de TCNT0 steeds tot 255 telt
    TCCR0A |= _BV(WGM00) | _BV(WGM01); // WGM00 = 1 en WGM01 = 1 --> Fast PWM Mode

    // STAP 2: stel *altijd* een PRESCALER in, anders telt hij niet.
    // De snelheid van tellen wordt bepaald door de CPU-klok (16Mhz) gedeeld door deze factor.
    TCCR0B |= _BV(CS02) | _BV(CS00); // CS02 = 1 en CS00 = 1 --> prescalerfactor is nu 1024 (=elke 64µs)

    // STAP 3: enable INTERRUPTs
    // Enable interrupts voor twee gevallen: TCNT0 == TOP en TCNT0 == OCR0A
    TIMSK0 |= _BV(TOIE0); // overflow interrupt enablen

    sei(); // interrupts globaal enablen
}
// deze ISR runt telkens wanneer TCNT0 gelijk is aan de waarde TOP-waarde (255)
/*ISR(TIMER0_OVF_vect) {
   counter++;
   PORTB |= _BV(PB2) | _BV(PB3) | _BV(PB4) | _BV(PB5);
}*/

void showParameters(){
  //alleen afstand op display tonen
  writeNumberAndWait(distance, 1000);

  //Feul reserve op led lampjes tonen
  
  if (fuelReserve > 1125)
  {
    lightUpAllLeds();
    if (fuelReserve < 1500)
    {
      /*
      //bereken percentage tussen 1500 en 1125 hoe lager percentage hoe sneller het ledje knippert
      int percentage = (fuelReserve - 1125) * 100 / (1500 - 1125);
      // Adjust the delay time based on the percentage
      int delayTime = 500 - ((400 * percentage) / 100); // Adjust the range and coefficients as needed
      //Dit stopt het programma, wil ik niet
      for (int i = 0; i < 1; i++)
      {
        lightDownLed(3);
        _delay_ms(delayTime);
        lightUpLed(3);
        _delay_ms(delayTime);
      }
      */

    }
  }else if (fuelReserve < 1125 && fuelReserve > 750)
  {
    lightDownLed(3);
  }else if (fuelReserve < 750 && fuelReserve > 375)
  {
    lightDownLed(2);
  }else if (fuelReserve < 375 && fuelReserve > 0)
  {
    lightDownLed(1);
  }else if (fuelReserve <= 0)
  {
    lightDownAllLeds();
  }
 
}

void enableButtons(){
  BUTTON_DDR &=  ~_BV(BUTTON2);
  BUTTON_PORT |= _BV(BUTTON2);
}
void enableLedPort(){
  LED_DDR |= _BV(LED1) | _BV(LED2) | _BV(LED3) | _BV(LED4);
  LED_PORT |= _BV(LED1) | _BV(LED2) | _BV(LED3) | _BV(LED4);
}

int main(){
  initTimer0();
  initUSART();
  initDisplay();
  enableButtons();
  enableLedPort();
  enableBuzzer();
  disableBuzzer();

  DDRB |= _BV(PB2) | _BV(PB3) | _BV(PB4) | _BV(PB5);  // LEDs in output zetten
  // Enable the interrupt for the button pin
  PCICR |= _BV(PCIE1);   // Enable Pin Change Interrupt 1
  PCMSK1 |= _BV(PCINT10);   // Enable interrupt for PC2 (Button pin)

  // Allocate memory for the logbook array
  int maxTime = 150;
  struct LogEntry* logbook = malloc(maxTime * sizeof(struct LogEntry));
  if (logbook == NULL) {
    printf("Memory allocation failed.\n");
    return 1;
  }

  int second = 0;
  while (distance > 3)
  {
    /*
    float counterFl = (float)counter/65.81; //63 -> 64sec gaat het fout
    int nCounter = floor(counterFl*10);
    int newCounter = floor(nCounter/10); 
    printf("Zoveel seconden zijn er voorbij: %d ", i);
    printf("En zoveel interrupts zijn er gebeurd: %d\n", newCounter);
    printf("Counter %ld\n", counter);*/
    showParameters();
    //_delay_ms(1000);
    logbook[second].burstConsumption = burstConsumption;
    //situatie
    if (buttonPressTime != 0)
    {
      if (fuelReserve > 1)
      {
      currentSpeed +=  gravity - burstConsumption / 5;
      fuelReserve -= burstConsumption;
      }
      buttonPressTime = 0;
      burstFuelConsumption = 0;
      burstConsumption = 0;
    }else{
      currentSpeed += gravity;
    }
    
    distance -= currentSpeed;

    // Store values in the logbook
    logbook[second].distance = distance;
    logbook[second].currentSpeed = (int)currentSpeed;
    logbook[second].fuelReserve = fuelReserve;
    second++;
    if (second >= maxTime)
    {
      printf("Je bent te traag! Je spaceship is ontploft!\n");
      break;
    }
  }
  // Disable the interrupt for the button pin
  PCICR &= ~_BV(PCIE1);       // Clear Pin Change Interrupt 1
  PCMSK1 &= ~_BV(PCINT10);   // Clear interrupt for PC2 (Button pin)

  int succesfullLanding = 0;
  if (distance <= 3 && currentSpeed <= 5)
  {
    succesfullLanding = 1;
  }

  // Print the logbook at the end of the program
  for (int i = 0; i < second; i++) {
    printf("Second: %3d - Distance: %4dm, Speed: %3dm/s, Burst: %2dl, Fuel: %4dl\n",
           i, logbook[i].distance, logbook[i].currentSpeed,
           logbook[i].burstConsumption, logbook[i].fuelReserve);
  }
  // Free the memory allocated for the logbook
  free(logbook);

  if (succesfullLanding)
  {
    
    while (1)
    {
      for (int i = 0; i < 500/20; i++)
      {
        writeNumberToSegment(0, 28);
        _delay_ms(5);
        writeNumberToSegment(1, 10);
        _delay_ms(5);
        writeNumberToSegment(2, 15);
        _delay_ms(5);
        writeNumberToSegment(3, 14);
        _delay_ms(5);
      }
      //Laat deuntje spelen op buzzer voor zachte landing
      buzzerOn();
       // Play a soft tune of relief
      playTone(500, 200);
      _delay_ms(250);
      playTone(600, 200);
      _delay_ms(250);
      playTone(700, 200);
      _delay_ms(250);
      playTone(800, 300);
      _delay_ms(350);
      playTone(900, 300);
      _delay_ms(350);
      playTone(1000, 400);
    }
    
  }else{
    
    while (1)
    {
      for (int i = 0; i < 500/20; i++)
      {
        writeNumberToSegment(0, 12);
        _delay_ms(5);
        writeNumberToSegment(1, 27);
        _delay_ms(5);
        writeNumberToSegment(2, 28);
        _delay_ms(5);
        writeNumberToSegment(3, 17);
        _delay_ms(5);
      }
      //Laat buzzer luid diep geluitje voor crash
      buzzerOn();
      playTone(400, 100);
      _delay_ms(50);
      playTone(300, 200);
      _delay_ms(100);
      playTone(200, 300);
      _delay_ms(150);
    }  
  }
}

