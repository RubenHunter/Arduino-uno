#include "display.h"

#include <avr/io.h>
#include <util/delay.h>

/* Segment byte maps for numbers 0 to 9, dit uitbreiden voor alfabet */
const uint8_t SEGMENT_MAP[] = {
  0xC0, // 0
  0xF9, // 1
  0xA4, // 2
  0xB0, // 3
  0x99, // 4
  0x92, // 5
  0x82, // 6
  0xF8, // 7
  0x80, // 8
  0x90, // 9
  0x88, // A 10
  0x83, // B 11
  0xC6, // C 12
  0xA1, // D 13
  0x86, // E 14
  0x8E, // F 15
  0xC2, // G 16
  0x89, // H 17
  0xF9, // I 18
  0x71, // J 19
  0x75, // K 20 not
  0xC7, // L 21 works
  0xC8, // M 22
  0xC7, // N 23
  0xCF, // O 24
  0x8F, // P 25
  0xC5, // Q 26
  0x88, // R 27
  0x92, // S 28
  0x87, // T 29
  0xF1, // U 30
  0xC1, // V 31
  0xE9, // W 32
  0xC9, // X 33
  0x8A, // Y 34
  0xA4,  // Z 35 works
  0xA3,  // ? 36 not
  0x23
};


/* Byte maps to select digit 1 to 4 */
const uint8_t SEGMENT_SELECT[] = {0xF1, 0xF2, 0xF4, 0xF8};

void initDisplay() {
  sbi(DDRD, LATCH_DIO);
  sbi(DDRD, CLK_DIO);
  sbi(DDRB, DATA_DIO);
}

// loop through seven segments of LED display and shift the correct bits in the
// data register
void shift(uint8_t val, uint8_t bitorder) {
  uint8_t bit;

  for (uint8_t i = 0; i < NUMBER_OF_SEGMENTS; i++) {
    if (bitorder == LSBFIRST) {
      bit = !!(val & (1 << i));
    } else {

      //0xc0 (getal 0 --> byte 1100 0000)
      //i == 0

      //bit = !!(val & (1 << (7 - i)));
      //bit = !!(val & (1 << 7));
      //bit = !!(val & (00000001 << 7));
      //bit = !!(val & 1000 0000);
      //bit = !!(1100 0000 & 1000 0000);
      //bit = !!(1000 0000);
      //!!(0) = 0    !!(!0) = 1  --> maakt alles een boolean (0 of 1);

      //bit = 1;
      bit = !!(val & (1 << (7 - i)));
    }
    // write bit to register
    if (bit == HIGH)
      sbi(PORTB, DATA_DIO);
    else
      cbi(PORTB, DATA_DIO);

    // Trigger the clock pin so the display updates
    sbi(PORTD, CLK_DIO);
    cbi(PORTD, CLK_DIO);
  }
}

//Schrijft cijfer naar bepaald segment. Segment 0 is meest linkse.
void writeNumberToSegment(uint8_t segment, uint8_t value) {
  cbi(PORTD, LATCH_DIO);
  shift(SEGMENT_MAP[value], MSBFIRST);
  shift(SEGMENT_SELECT[segment], MSBFIRST);
  sbi(PORTD, LATCH_DIO);
}

/*//voor de dots
void writeNumberToSegment(uint8_t segment, uint8_t value, uint8_t dot) {
  cbi(PORTD, LATCH_DIO);
  shift(SEGMENT_MAP[value], MSBFIRST);  // Display the value
  shift(SEGMENT_SELECT[segment], MSBFIRST);

  // Control the dot segment
  if (dot) {
    shift(0x01 << segment, MSBFIRST);
  } else {
    shift(0x00, MSBFIRST);  // Turn off the dot segment
  }

  sbi(PORTD, LATCH_DIO);
}*/


/*
void writeNumberToSegment(uint8_t segment, uint8_t value, uint8_t flicker) {
  cbi(PORTD, LATCH_DIO);
  if (segment == 1 && flicker) {
    // Flicker effect: Turn the segment off and on continuously
    while (1) {
      for (uint8_t i = 0; i < NUMBER_OF_SEGMENTS; i++) {
        if (i == segment) {
          shift(SEGMENT_MAP[value], MSBFIRST);  // Display the value on the specified segment
        } else {
          shift(0x00, MSBFIRST);  // Keep other segments turned off
        }
        shift(SEGMENT_SELECT[i], MSBFIRST);
        sbi(PORTD, LATCH_DIO);
        _delay_ms(5);
      }
    }
  } else {
    shift(SEGMENT_MAP[value], MSBFIRST);  // Display the value
    shift(SEGMENT_SELECT[segment], MSBFIRST);
    sbi(PORTD, LATCH_DIO);
  }
  sbi(PORTD, LATCH_DIO);
}*/

//Schrijft getal tussen 0 en 9999 naar de display. Te gebruiken in een lus...
void writeNumber(int number) {
  if (number < 0 || number > 9999) return;
  writeNumberToSegment(0, number / 1000);
  writeNumberToSegment(1, (number / 100) % 10);
  writeNumberToSegment(2, (number / 10) % 10);
  writeNumberToSegment(3, number % 10);
}

//Schrijft getal tussen 0 en 9999 naar de display en zorgt dat het er een bepaald aantal milliseconden blijft staan.
//Opgelet: de timing is "ongeveer", er wordt geen rekening gehouden met de tijd writeNumberToSegment in beslag neemt...
void writeNumberAndWait(int number, int delay) {
  if (number < 0 || number > 9999) return;
  for (int i = 0; i < delay / 20; i++) {
    writeNumberToSegment(0, number / 1000);
    _delay_ms(5);
    writeNumberToSegment(1, (number / 100) % 10);
    _delay_ms(5);
    writeNumberToSegment(2, (number / 10) % 10);
    _delay_ms(5);
    writeNumberToSegment(3, number % 10);
    _delay_ms(5);
  }
}
