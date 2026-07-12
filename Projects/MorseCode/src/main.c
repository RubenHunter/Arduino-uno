#define __DELAY_BACKWARD_COMPATIBLE__
#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>
#include <usart.h>
#include <led.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <display.h>

char morse[26][4] = {
    "SL",
    "LSSS",
    "LSLS",
    "LSS",
    "S",
    "SSLS",
    "LLS",
    "SSSS",
    "SS",
    "SLLL",
    "LSL",
    "SLSS",
    "LL",
    "LS",
    "LLL",
    "SLLS",
    "LLSL",
    "SLS",
    "SSS",
    "L",
    "SSL",
    "SSSL",
    "SLL",
    "LSSL",
    "LSLL",
    "LLSS"
    };
char alfabet[26] = {
  'A',
  'B',
  'C',
  'D',
  'E',
  'F',
  'G',
  'H',
  'I',
  'J',
  'K',
  'L',
  'M',
  'N',
  'O',
  'P',
  'Q',
  'R',
  'S',
  'T',
  'U',
  'V',
  'W',
  'X',
  'Y',
  'Z'
};

void startApplicatie(){
  _delay_ms(750);
  lightDownLed(3);
  _delay_ms(500);
  lightDownLed(2);
  _delay_ms(500);
  lightDownLed(1);
  _delay_ms(500);
  lightDownLed(0);
  _delay_ms(500);
}

void endSequence(){
  while (1)
  {
    for (int i = 0; i < 4; i++)
    {
      lightUpLed(i);
      _delay_ms(100);
      lightDownLed(i);
    }
    for (int i = 3; i >= 0; i--)
    {
      lightUpLed(i);
      _delay_ms(100);
      lightDownLed(i);
    }
  }
  
}

int main(){
  initUSART();
  enableAllLeds();

  /*UITBREIDING: Schrijf een functie die een string als parameter krijgt, en vervolgens dat woord in Morse-code toont op de leds...*/
  char woord[20] = {'A', 'P', 'P', 'E', 'L'};

  /*error woord inlezen, geen idee waarom
  printf("Geef een woord die je in morsecode wilt zien (max 20 letters): \n");
  fgets(woord, sizeof(woord), stdin);
  printf("Het woord dat je hebt gekozen is: %s\n", woord);*/


  
  for (int i = 0; i < strlen(woord); i++)
  {
    woord[i] = toupper(woord[i]);
  }
  
  //printf("%s", woord);

  _delay_ms(250);
  startApplicatie();

  char currentChar[5] = {'\0'};
  int currentIndex;
  //Vanaf hier weg commenten voor uitbreiding
  /*
  srand(0);
  int randomIndex;
  
  for (int i = 0; i < 10; i++)
  {
    randomIndex = rand() % 26;
    char currentLetter = alfabet[randomIndex];
    for (int j = 0; j < 4; j++)
    {
      strncat(currentChar, &morse[randomIndex][j], 1);
    }
  */
  /*UITBREIDING: Schrijf een functie die een string als parameter krijgt, en vervolgens dat woord in Morse-code toont op de leds...*/
  
  for (int i = 0; i < strlen(woord); i++)
  {
    //Vind de index van het alfabet zodat we die index in de morsecode array kunnen gebruiken
    for (int j = 0; j < 26; j++)
    {
      if (woord[i] == alfabet[j])
      {
        currentIndex = j;
      }
    }
    for (int j = 0; j < 4; j++)
    {
      strncat(currentChar, &morse[currentIndex][j], 1);
    }
    
    for (int d = 0; d < 4; d++)
    {
      /*printf(" %c ", currentChar[d]);*/
      if (currentChar[d] == 'S')
      {
        lightUpAllLeds();
        _delay_ms(200);
      } else if (currentChar[d] == 'L')
      {
        lightUpAllLeds();
        _delay_ms(1000);
      } else {
        /*printf("Error");*/
      }
      lightDownAllLeds();
      _delay_ms(700);
    }
    _delay_ms(4000);
    printf("Juiste antwoord: %c (%s)\n", woord[i], currentChar);
    // Reset currentChar to an empty string for the next iteration
    memset(currentChar, 0, sizeof(currentChar));
  }
  _delay_ms(2000);
  endSequence();
}
 
  