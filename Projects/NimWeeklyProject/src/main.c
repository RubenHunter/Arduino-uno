#define __DELAY_BACKWARD_COMPATIBLE__
#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>
#include <usart.h>
#include <display.h>
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

int gameStart = 1;
int startAantal = 21;
int beschikbaarAantal = 21;
int maxAantal = 3;
int minAantal = 1;
int currentAantal = 2;
int writeNumberArray[4];
int currentPlayer = 1;
int pcCount = 0;
typedef struct {
  int player;
  int pointsTaken;
  int totalPointsLeft;
  } RoundData;

ISR(PCINT1_vect)
{
  if (gameStart == 1)
  {
    if ((PINC & (1 << PC1)) == 0) 
    {
      printf("Start button pushed!\n");
      gameStart = 0;
    }
  }
  else{
    if (currentPlayer == 1)
    {
      if ((PINC & (1 << PC1)) == 0) 
      {
        //printf("Button1 ingedrukt\n");
        if (currentAantal <= 3 && currentAantal > 1)
        {
          currentAantal--;
          writeNumberArray[0] = currentAantal;
        }else{
          printf("Je mag maar min 1 stokjes wegnemen!\n");
        }
      
      }else if ((PINC & (1 << PC3)) == 0)
      {
        //printf("Button3 ingedrukt\n");
        if (currentAantal >= 1 && currentAantal < 3)
        {
          currentAantal++;
          writeNumberArray[0] = currentAantal;
        }else{
          printf("Je kan max 3 stokjes wegnemen!\n");
        }
      }else if ((PINC & (1 << PC2)) == 0)
      {
        printf("Je hebt %d stokjes van %d stokjes weggenomen ", currentAantal, beschikbaarAantal);
        beschikbaarAantal = beschikbaarAantal - currentAantal;
        //currentAantal = 2;
        //Veranderen van beurt
        currentPlayer = 2;
        printf("er zijn nu %d stokjes over in het spel.\n", beschikbaarAantal);
        //printf("currenplayer naar 2: %d\n", currentPlayer);
        int tensDigit = beschikbaarAantal / 10;
        int onesDigit = beschikbaarAantal % 10;
        //printf("tens: %d, ones: %d\n", tensDigit, onesDigit);
        writeNumberArray[2] = tensDigit;
        writeNumberArray[3] = onesDigit;
        writeNumberArray[0] = currentAantal;

      }
    }else if (currentPlayer == 2)
    {
      if ((PINC & (1 << PC2)) == 0){
        pcCount++;
        if (pcCount > 2)
        {
          pcCount = 0;
        }
        
        //printf("pcCount = %d\n", pcCount);
      }
    }else{
      printf("Er ging iets mis met het starten van beurt\n");
    }
  }
}

int playRound(){
  //printf("Currentplayer: %d\n", currentPlayer);
  if (currentPlayer == 1)
  {
    printf("Beurt aan jou\n");
    writeNumberArray[1] = 1;
    PCICR |= _BV(PCIE1);
    PCMSK1 |= _BV(BUTTON1) | _BV(BUTTON2) | _BV(BUTTON3);

    sei();
    for (int i = 0; i < 999999; i++)
    {
      //printf("NumberD: %d\n", getNumberDisplay());
      //printf("TotaalB: %d , currenAantal: %d\n", beschikbaarAantal, currentAantal);
      writeNumberAndWait(getNumberDisplay(), 1000);
      //printf("Currentplayer= %d\n", currentPlayer);
      //Hier interupt zetten, Knop 3 = verhoog links nummer met 1 max3, button 1 velagen met 1 min1, button 2 confirm
      if (currentPlayer == 2)
      {
        //CALLOC of MALLOC voor opslaan van keuzes en punten etc.
        return 2;
      }
    }
    
  }else if (currentPlayer == 2)
  {
    //Computer beurt
    writeNumberArray[1] = 2;
    int pcKeuze = 0;
    printf("Beurt aan de pc\n");

    PCICR |= _BV(PCIE1);
    PCMSK1 |= _BV(BUTTON2);

    sei();
    for (int i = 0; i < 999999; i++)
    {
      //read input
      //printf("NumberD1: %d\n", getNumberDisplay());
      writeNumberAndWait(getNumberDisplay(), 1000);
      //Hier interupt zetten, Knop 3 = verhoog links nummer met 1 max3, button 1 velagen met 1 min1, button 2 confirm
      if (pcCount == 1)
      {
        if (pcKeuze == 0)
        {
          pcKeuze = (beschikbaarAantal - 1) % (maxAantal + 1);
          if (pcKeuze == 0)
          {
            pcKeuze = (rand() % maxAantal) + 1;
          }
          printf("Pc Keuze: %d\n", pcKeuze);
          writeNumberArray[0] = pcKeuze;
        }
      }else if (pcCount == 2)
      {
        printf("De pc heeft %d stokjes van %d stokjes weggenomen ", pcKeuze, beschikbaarAantal);
        beschikbaarAantal = beschikbaarAantal - pcKeuze;
        //currentAantal = 2;
        printf("er zijn nu %d stokjes over in het spel.\n", beschikbaarAantal);
        int tensDigit = beschikbaarAantal / 10;
        int onesDigit = beschikbaarAantal % 10;
        //printf("tens: %d, ones: %d\n", tensDigit, onesDigit);
        writeNumberArray[2] = tensDigit;
        writeNumberArray[3] = onesDigit;
        writeNumberArray[0] = currentAantal;

        pcCount = 0;
        //Veranderen van beurt
        currentPlayer = 1;
        return 1;
      }
    }
  }else{
    printf("Er ging iets fout...");
    return 0;
  }
  return 1;
}

void enableLedPort(){
  LED_DDR |= _BV(LED1) | _BV(LED2) | _BV(LED3) | _BV(LED4);
  LED_PORT |= _BV(LED1) | _BV(LED2) | _BV(LED3) | _BV(LED4);
}
void enableButtons(){
  BUTTON_DDR &= ~_BV(BUTTON1) & ~_BV(BUTTON2) & ~_BV(BUTTON3);
  BUTTON_PORT |= _BV(BUTTON1) | _BV(BUTTON2) | _BV(BUTTON3);
}
void initADC()
{
    ADMUX |= (1 << REFS0);
    ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
    ADCSRA |= (1 << ADEN) /*| (1 << ADIE) dit nog straks terug aanzetten*/; //zegt gwn 'we gaan met interrupts werken' 
}

int getNumberDisplay(){
  int number = 0;
  for (int i = 0; i < 4; i++) {
    number *= 10;
    number += writeNumberArray[i];
  }
  return number;
}

void endSequence(){
  for (int i = 0; i < 10; i++)
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
  initDisplay();
  initADC();
  enableLedPort();
  enableButtons();

  
  RoundData** rounds = NULL;
  int numRounds = 0;

  /* //Allocate memory for 10 rounds
  numRounds = 20;
  rounds = malloc(numRounds * sizeof(RoundData*));

  // Allocate memory for each round individually
  for (int i = 0; i < numRounds; i++) {
    rounds[i] = malloc(sizeof(RoundData));
  }*/


  PCICR |= _BV(PCIE1); // Vector PCIE1 --> Poort C (Buttons & ADC)
  PCMSK1 |= _BV(BUTTON1) | _BV(BUTTON2) | _BV(BUTTON3);
  
  
  sei();
  printf("Draai aan de potentiometer en druk op knop 1 om te starten\n");
  //maak interrupt voor knop 1 in te drukken, zorg ervoor dat als er gedraaid word aan de meter het op het display komt staan
  uint16_t value;
  while (gameStart)
  {
    /* code */
    ADCSRA |= (1<<ADSC);//start de conversie analoog -> digitaal
    loop_until_bit_is_clear(ADCSRA,ADSC);//wacht tot conversie gebeurd is, hier zou je interrupt moeten staan.
    value = ADC;//lees het resultaat uit
    value = ADC * 9;
    //printf("Value:%d\n", value);
    writeNumberAndWait(value, 100);
  }
  srand(value);
  //printf("srand(%d)", value);
  printf("Speler 1 ben jij, Speler 2 is de computer (2de LED op je display zegt welke speler zijn beurt is). \n");
  int tensDigit = startAantal / 10;
  int onesDigit = startAantal % 10;
  //printf("tens: %d, ones: %d\n", tensDigit, onesDigit);
  writeNumberArray[2] = tensDigit;
  writeNumberArray[3] = onesDigit;
  writeNumberArray[1] = 0;
  writeNumberArray[0] = currentAantal;
  int number = getNumberDisplay();
  //printf("Number: %d\n", number);
  writeNumberAndWait(number, 1000);
 
  currentPlayer = (rand() % 2) + 1;
  //printf("startSpeler: %d\n" , currentPlayer);
  
  // Allocate memory for rounds dynamically
  int i = 0;
  while (beschikbaarAantal > 0)
  {
    i++;
    printf("New round, ronde %d\n", i);
    playRound();

    // Add a round every iteration
    numRounds++;
    rounds = realloc(rounds, numRounds * sizeof(RoundData*));
    rounds[numRounds - 1] = malloc(sizeof(RoundData));

    if (currentPlayer==1)
    {
      rounds[numRounds - 1]->player = 2;
    }else if (currentPlayer == 2)
    {
      rounds[numRounds - 1]->player = 1;
    }
    // Update round data
    rounds[numRounds - 1]->pointsTaken = currentAantal;
    rounds[numRounds - 1]->totalPointsLeft = beschikbaarAantal;

    currentAantal = 2;
  }

  printf("Speler %d heeft Gewonnen!!\n", currentPlayer);
  int winner;
  if (currentPlayer == 1)
  {
    winner = 1111;
  }else if (currentPlayer == 2)
  {
    winner = 2222;
  }
  
  // Print the table of rounds
  printf("Round\tPlayer\tPsTaken\tPsLeft\n");
  for (int j = 0; j < numRounds; j++) {
    printf("%d\t%d\t -%d\t%d\n", j + 1, rounds[j]->player, rounds[j]->pointsTaken, rounds[j]->totalPointsLeft);
  }

  // Free the allocated memory
  for (int j = 0; j < numRounds; j++) {
    free(rounds[j]);
  }
  free(rounds);

  for (int i = 0; i < 9999; i++)
  {
    
    writeNumberAndWait(winner, 200);
    if (i % 10 == 0)
    {
      endSequence();
    }
    
  }
  return 0;
}