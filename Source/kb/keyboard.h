/*
  Keyboard.h
  On screen keyboard for AVR90USB1286
  
  Methods for single characters, some auxiliary methods have been added, 
  as well as slight edits of original methods.
*/
#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdint.h>

#define MAX_STRING_SIZE     10

//The string which is inputted from the keyboard.
extern char k_str[MAX_STRING_SIZE + 1];

/*
  Detects user motion on the keyboard (using encoder.h),
  and updates state. Should be called often. 
  Returns true if Enter was pressed.
*/
uint8_t move_keyboard(void);

/* - Joshua
  Detects user motion on the keyboard (using encoder.h),
  and updates state. Should be called often. 
  Returns 1 if character pressed, and 2 if quit is pressed.
*/
uint8_t move_keyboard_char(void);

/*
  Draw the keyboard on screen using lcd.h
  Call it every time you want to update the screen (often)
*/
void draw_keyboard(void);

/* - Joshua
  Draw the keyboard on screen using lcd.h
  Call it every time you want to update the screen (often).
  This excludes displaying the current string from the keyboard.
*/
void draw_keyboard_no_word(void);

/*
  Initialize the keyboard. Call before displaying it.
*/
void init_keyboard(void);

//method to reset the string for a single character.
void reset_k_str(void);

//method to reset the entire string.
void reset_k_str_full(void);

#endif /* KEYBOARD_H */