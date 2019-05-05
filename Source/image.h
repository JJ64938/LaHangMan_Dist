/*
  image.h
  Graphics are stored in program memory, and are
  in a simple (and inefficient) array of colours.
  
  Joshua: Only made use of the triangle used in the menu here.
*/

#ifndef IMAGE_H
#define IMAGE_H
#include <avr/pgmspace.h>
#include <stdint.h>
#include "lcd/svgrgb565.h"

//4x9
static uint16_t triangle_sprite[28] PROGMEM = {
    BLUE,BLACK,BLACK,BLACK,
    BLUE,BLUE,BLACK,BLACK,
    BLUE,BLUE,BLUE,BLACK,
    BLUE,BLUE,BLUE,BLUE,
    BLUE,BLUE,BLUE,BLACK,
    BLUE,BLUE,BLACK,BLACK,
    BLUE,BLACK,BLACK,BLACK,
};
#endif /* IMAGE_H */
