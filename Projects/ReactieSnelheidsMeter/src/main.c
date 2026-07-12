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
#define BUTTON1 PC1
#define BUTTON2 PC2
#define BUTTON3 PC3
#define LED1 PB2
#define LED2 PB3
#define LED3 PB4
#define LED4 PB5
#define NUM_MEASUREMENTS 10

uint64_t counter = 0;
uint64_t start = 0;
uint64_t reactie = 0;
int playAgain = 1;
int keuze = 0;
int currentKeuze = 1;
uint64_t reactieSnelheid = 0;
int randomNumber = 0;
int numberOfLevels = 5;
int* afgelegdeLevels = NULL;
int madeChoice = 0;
int distractions = 0;


struct LEVEL {
    int levelNr;
    uint64_t* metingen;
};

void initTimer()
{
    
    TCCR1B |= (1 << WGM12);

    TCCR1B |= (1 << CS11) | (1 << CS10);

    // Set compare value for 1ms interrupt
    OCR1A = F_CPU / 64 / 1000 - 1;

    TIMSK1 |= (1 << OCIE1A);
    sei();
}
ISR(TIMER1_COMPA_vect)
{
    counter++;
}

ISR(TIMER1_OVF_vect)
{
    if (distractions == 1)
    {  
        while (reactieSnelheid == 0)
        {
            int randomRepeat = rand() % 5;
            int randomDelay = 0;
            if (randomRepeat % 2 == 0)
            {
                for (int c = 0; c < randomRepeat; c++)
                {
                    randomDelay = rand() % 200;
                    lightUpAllLeds();
                    _delay_ms(randomDelay);
                    lightDownAllLeds();
                    _delay_ms(randomDelay);
                }
            }
            if (randomNumber % 3 == 0)
            {
                for (int c = 0; c < randomRepeat; c++)
                {
                    randomDelay = rand() % 200;
                    buzzerOn();
                    int randomTone = rand() % (1300 - 300 +1) + 300;
                    playTone(randomTone, randomDelay);
                    disableBuzzer();
                    _delay_ms(randomDelay);
                }
                
            }
        }
    }
}
//Kies level menu
void kiesLevel(){
    while (keuze == 0)
    {
        writeNumberToSegment(0, 21);
        writeNumberToSegment(1, 31);
        writeNumberToSegment(2, 21);
        writeNumberToSegment(3, currentKeuze);
    }
    printf("level %d gaat van start.\n", keuze);
}

void playLevel(struct LEVEL* level){
    //Elk level bestaat uit een reeks van 10 tests.
    for (int i = 1; i <= NUM_MEASUREMENTS; i++)
    {
        printf("Test %d :\n", i);
        int randomDelay = rand() % 7000;
        for (int j = 0; j < randomDelay / 20; j++)
        {
            writeNumberToSegment(0, i);
            _delay_ms(20);
        }
        
        randomNumber = rand() % 3;
        _delay_ms(randomDelay);
        start = counter;
        switch (keuze)
        {
        case 1:
            //Level1
            writeNumberToSegment(randomNumber, 0);
            start = counter;
            while (reactieSnelheid == 0)
            {
                writeNumberToSegment(randomNumber, 0);
                //laat de timer gaan totdat er een reactie is

            }
            

            break;
        
        case 2:
            //Level2
            switch (randomNumber)
            {
            case 0:
                while (reactieSnelheid == 0)
                {
                    writeNumberToSegment(0, 21);
                }
                
                break;
            case 1:
                while (reactieSnelheid == 0)
                {
                    writeNumberToSegment(0, 22);
                }
                
                break;
            case 2:
                while (reactieSnelheid == 0)
                {
                    writeNumberToSegment(0, 27);
                }
                
                break;
            default:
                break;
            }
            break;
        case 3:
            //Level3
            for (int j = 0; j < randomNumber; j++)
            {
                lightUpAllLeds();
                _delay_ms(200);
                lightDownAllLeds();
                _delay_ms(200);
            }
            while (reactieSnelheid == 0)
            {
                //laat timer lopen
            }
            
            
            break;
        case 4: 
            //Level 4
            enableBuzzer();
            for (int j = 0; i < randomNumber; j++)
            {
                
                playTone(600, 200);
                _delay_ms(250);
            }
            disableBuzzer();
            while (reactieSnelheid == 0)
            {
                //laat timer lopen
            }
            
            break;
        case 5:
            //Level 5
            distractions = 1;
            switch (randomNumber)
            {
            case 0:
                while (reactieSnelheid == 0)
                {
                    writeNumberToSegment(0, 21);
                }
                
                break;
            case 1:
                while (reactieSnelheid == 0)
                {
                    writeNumberToSegment(0, 22);
                }
                
                break;
            case 2:
                while (reactieSnelheid == 0)
                {
                    writeNumberToSegment(0, 27);
                }
                
                break;
            default:
                break;
            }
            break;
        default:
            break;
        }
        printf("je reactie snelheid was: %lldms\n", reactieSnelheid);
        level->metingen[i] = reactieSnelheid;
        reactieSnelheid = 0;
        
    }
    
}

void berekenReactieSnelheid(){
    reactie = counter;
    reactieSnelheid = reactie - start;
}

void opnieuw(){
    playAgain = 0;
    madeChoice = 0;
    while (madeChoice == 0)
    {
        writeNumberToSegment(0, 23);
        writeNumberToSegment(1, 24);
        writeNumberToSegment(2, 16);
        writeNumberToSegment(3, 36);
    }
    
}

ISR(PCINT1_vect){
    if (keuze == 0)
    {
        if ((PINC & (1 << PC1)) == 0)
        {
            if (playAgain == 0)
            {
                playAgain = 1;
                madeChoice = 1;
                return;
            }else{
                if (currentKeuze <= 5 && currentKeuze > 1)
                {
                    currentKeuze--;
                }else{
                    printf("LVL1 is het laagste level!\n");
                }
            }
            
        }else if ((PINC & (1 << PC3)) == 0)
        {
            if (currentKeuze >= 1 && currentKeuze < 5)
            {
                currentKeuze++;
            }else{
                printf("LVL5 is het hoogste level!\n");
            }
        }else if ((PINC & (1 << PC2)) == 0)
        {
            if (playAgain == 0)
            {
                playAgain = 0;
                madeChoice = 1;
                return;
            }else{
                int containsKeuze = 0;
                for (int i = 0; i < 5; i++) {
                    if (afgelegdeLevels[i] == keuze) {
                        containsKeuze = 1;
                        break;
                    }
                }
                if (containsKeuze) {
                    keuze = currentKeuze;
                } else {
                    keuze = 0;
                    printf("Dit level heb je al gespeeld!\n");
                } 
            }
            
            
        }
        int isEmpty = 1;
        for (int i = 0; i < 5; i++) {
            if (afgelegdeLevels[i] != 0) {
                isEmpty = 0;
                break;
            }
        }
        if (isEmpty) {
            // afgelegdeKeuzes is empty
            //We willen de seed maar 1 keer instellen
            srand(counter);
        }
    }else
    {
        if ((PINC & (1 << PC1)) == 0){
            if (randomNumber == 0)
            {
                berekenReactieSnelheid();
            }else{
                printf("Wrong Button\n");
            }
            
        }else if ((PINC & (1 << PC2)) == 0)
        {
            if (randomNumber == 1)
            {
                berekenReactieSnelheid();
            }else{
                printf("Wrong Button\n");
            }
        }else if ((PINC & (1 << PC3)) == 0)
        {
            if (randomNumber == 2)
            {
                berekenReactieSnelheid();
            }else{
                printf("Wrong Button\n");
            }
        }
        
        
    }
    
    
}


void enableButtons(){
  BUTTON_DDR &= ~_BV(BUTTON1) & ~_BV(BUTTON2) & ~_BV(BUTTON3);
  BUTTON_PORT |= _BV(BUTTON1) | _BV(BUTTON2) | _BV(BUTTON3);
}

int main()
{
    initUSART();
    initTimer();
    enableAllLeds();
    //lightDownAllLeds();
    //enableBuzzer();
    //disableBuzzer();
    initDisplay();

    PCICR |= _BV(PCIE1); // Vector PCIE1 --> Poort C (Buttons & ADC)
    PCMSK1 |= _BV(BUTTON1) | _BV(BUTTON2) | _BV(BUTTON3);

    
    printf("welkom bij de reactiesnelheidsmeter, Kies een level en bij elk level moet je zo snel mogenlijk reageren door op de juiste knop te drukken.\n");
    struct LEVEL* level = malloc(sizeof(struct LEVEL));
    if (level == NULL) {
        printf("Memory allocation levelStruct failed.\n");
        return 1;
    }

    afgelegdeLevels = calloc(numberOfLevels, sizeof(int));
    if (afgelegdeLevels == NULL) {
        printf("Memory allocation LvlArray failed.\n");
        return 1;
    }
    
    int i = 0;
    while (playAgain)
    {
        
        kiesLevel();
        //printf("Counter: %ld\n", counter);
        level->levelNr = keuze; // Set the level number
        // Allocate memory for the measured reaction times array
        level->metingen = malloc((NUM_MEASUREMENTS + 1) * sizeof(uint64_t));
        if (level->metingen == NULL) {
            printf("Memory allocation failed.\n");
            free(level);
            return 1;
        }
        playLevel(level);
        afgelegdeLevels[i] = keuze;
        /*if (counter == 2500)
        {
            printf("break");
            break;

        }*/
        keuze = 0;
        opnieuw();
        if (playAgain == 0)
        {
            break;
        }
        
        i++;
    }   

    //Print out the Level construct etc Age


    // Free the allocated memory
    free(level->metingen);
    free(level);
    return 0;
}