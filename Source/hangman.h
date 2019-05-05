/* La HangMan, by Joshua Gregory 

A simple hangman game, whereby you can play to win against the compute with randomly chosen words, 
or against a human who can input a word of their choice to guess.


This application contains adapted code from Giacomo Meanti's Space Invaders game, which he included under the Creative Commons Attribution License.
Any method in hangman.c commented with "adapted method" has been adapted.
His Keyboard library was also used, along with my own adapted methods for single characters.
The general structure for the menu and interrupts for drawing and movement have also been adapted.

Licence: This work is licensed under the Creative Commons Attribution License.
         View this license at http://creativecommons.org/about/licenses/
*/

#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <avr/eeprom.h>
#include "lcd/lcd.h"
#include "encoder/encoder.h"
#include "image.h"
#include "kb/keyboard.h"
#include "lcd/svgrgb565.h"

#define LED_INIT    DDRB  |=  _BV(PINB7)
#define LED_ON      PORTB |=  _BV(PINB7)
#define LED_OFF     PORTB &= ~_BV(PINB7)

#define FALSE 0
#define TRUE  1

#define STATE_HOME           0
#define STATE_PLAYING        1
#define STATE_INPUT_WORD     2
#define STATE_GAME_END       3

#define HOME_SCREEN_ITEMS    2

#define FALSE 0
#define TRUE 1

uint8_t is_drawn;
uint8_t word_guessed;
uint8_t game_end;
char guessing_word[11];
char word_to_guess[11];
char letter;
char guessed_chars[27];
uint8_t hang_counter;
uint8_t guessed_line;
uint8_t drawn_char;
uint8_t vs_comp;

volatile uint8_t selected_item;
volatile int8_t last_selected_item;
volatile uint8_t game_state;

uint16_t random_seed;

void draw_home_screen(void);
void home_screen_movement(void);
void draw_playing(void);
void playing_movement(void);
void draw_input_word(void);
void input_word_movement(void);
void draw_game_end(void);
void game_end_movement(void);

uint16_t rand_init(void);
uint16_t rand(void);

void os_init(void);

uint8_t is_value_inarray(char, char *);
void update_guessing_word(char);
void update_spaces(void);
uint8_t is_word_guessed(void);
void pick_rand_word(void);
void set_word(char *);
void draw_guessed_char(char);
void append(char*, char);
void display_guessing_lines(void);
void update_word_lines(char);
void setup_guessed_chars(void);
void draw_hangman_piece(void);