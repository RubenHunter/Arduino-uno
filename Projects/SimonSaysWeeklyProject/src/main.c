#define __DELAY_BACKWARD_COMPATIBLE__
#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>
#include <usart.h>
#include <led.h>
#include <stdlib.h>
#define NUMBER_OF_LEDS 4
#define LED_PORT PORTB
#define BUTTON_PORT PORTC
#define BUTTON_PIN PINC
#define LED_DDR DDRB
#define BUTTON_DDR DDRC
#define BUTTON1 PC1
#define BUTTON2 PC2
#define BUTTON3 PC3
#define LED1 PB2
#define LED2 PB3
#define LED3 PB4
#define LED4 PB5
#define DELAY 500

int gameStart = 1;
int randomLed = 0;

ISR(PCINT1_vect){
    if (gameStart == 1)
    {
      if ((PINC & (1 << PC1)) == 0) {
      printf("Start button pushed!\n");
      gameStart = 0;
      }
    }
}

int * generatePuzzle(int reeks[]){
    for (int i = 0; i < 10; i++)
  {
    int randomGetal = rand() % 3;
    reeks[i] = randomGetal;
  }
  return reeks;
}

void printReeks(int reeks[], int aantal){
    printf("[ ");
    for (int i = 0; i < aantal; i++)
    {
        printf("%d ",reeks[i]);
    }
    printf("]\n");
}

void playPuzzle(int reeks[], int aantal){
    
        _delay_ms(1000);
        printf("Kijk nu naar de ledjes het begint!\n");
        _delay_ms(2000);
    
    
    for (int i = 0; i <= aantal; i++)
    {
        lightUpLed(reeks[i]);
        _delay_ms(DELAY);
        lightDownLed(reeks[i]);
        _delay_ms(DELAY);
    }

}

int readInput(){
    
        if (bit_is_clear(BUTTON_PIN, BUTTON1))
        {
            _delay_ms(500);
            if (bit_is_clear(BUTTON_PIN, BUTTON1))
            {
                printf("knop 0 is ingedrukt");
                return 0;
            } 
        }

        if (bit_is_clear(BUTTON_PIN, BUTTON2))
        {
            _delay_ms(500);
            if (bit_is_clear(BUTTON_PIN, BUTTON2))
            {
                printf("Knop 1 is ingedrukt");
                return 1; 
            } 
        }

        if (bit_is_clear(BUTTON_PIN, BUTTON3))
        {
            _delay_ms(500);
            if (bit_is_clear(BUTTON_PIN, BUTTON3))
            {
                printf("Knop 2 is ingedrukt");
                return 2; 
            } 
        }
        return -1;
}

int main(){
  initUSART();

  LED_DDR |= _BV(LED1) | _BV(LED2) | _BV(LED3) | _BV(LED4);
  LED_PORT |= _BV(LED1) | _BV(LED2) | _BV(LED3) | _BV(LED4);

  BUTTON_DDR &= ~_BV(BUTTON1) & ~_BV(BUTTON2) & ~_BV(BUTTON3);
  BUTTON_PORT |= _BV(BUTTON1) | _BV(BUTTON2) | _BV(BUTTON3);

  PCICR |= _BV(PCIE1);
  PCMSK1 |= _BV(BUTTON1) | _BV(BUTTON2) | _BV(BUTTON3);


  sei();
  int timer = 0;
  printf("Druk op de 1ste knop om te starten\n");
  while (gameStart)
  {
    lightUpLed(3);
    _delay_ms(20);
    lightDownLed(3);
    _delay_ms(20);
    timer++;
  }
  //printf("nu uit de loop\n");
  int reeks[10];
  srand(timer);
  generatePuzzle(reeks);
  //printReeks(reeks, 10);
  int input;
  int fout = 0;
  for (int i = 0; i < 9; i++)
  {
    printf("LOOP NR: %d \n", i);
    playPuzzle(reeks, i);
    printf("Ok druk nu de juiste volgorde knopjes in!\n");
    for (int j = 0; j <= i; j++)
    {
        
        while (1)
        {
            input = readInput();
            if (input != -1)
            {
                break;
            }
        }

        switch (input)
        {
        case 0:
            if (reeks[j] == 0)
            {
                printf(", correct.\n");
            }else{
                printf(", Fout!\n");
                fout = 1;
                break;
            }
            
            break;
        case 1:
            if (reeks[j] == 1)
            {
                printf(", correct.\n");
            }else{
                printf(", Fout!\n");
                fout = 1;
                break;
            }
            break;
        case 2:
            if (reeks[j] == 2)
            {
                printf(", correct.\n");
            }else{
                printf(", Fout!\n");
                fout = 1;
            }
            break;
        default:
        printf("Mislukt geen input %d", input);
            break;
        }
        //printReeks(reeks, j+1);
        if (fout)
        {
            break;
        }
        

    } 
    
    if (fout)
    {
            printf("Je hebt het tot level %d gehaald!\n", i+1);
            printf("Het juiste patroon was:");
            printReeks(reeks, i+1);
            break;

    } 
    if (i == 9)
    {
        printf("Proficiat, je bent de Simon Master!");
        break;
    }
    printf("We gaan naar level %d \n", i + 2);
  }

  return 0;
}