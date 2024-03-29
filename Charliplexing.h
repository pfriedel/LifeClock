/*
  Charliplexing.h - Library for controlling the charliplexed led board
  from JimmiePRodgers.com
  Created by Alex Wenger, December 30, 2009.
  Modified by Matt Mets, May 28, 2010.
  Released into the public domain.
*/

#ifndef Charliplexing_h
#define Charliplexing_h

#include <inttypes.h>

#define SINGLE_BUFFER 0
#define DOUBLE_BUFFER 1
#define GRAYSCALE     2

#define DISPLAY_COLS 10     // Number of columns in the display
#define DISPLAY_ROWS 11     // Number of rows in the display
#define SHADES 8 // Number of distinct shades to display, including black, i.e. OFF

namespace LedSign
{
    extern void Init(uint8_t mode = SINGLE_BUFFER);
    extern void Set(uint8_t x, uint8_t y, uint8_t c = 1);
    extern void SetBrightness(uint8_t brightness);
    extern volatile unsigned int tcnt2;
    extern void Flip(bool blocking = false);
    extern void Clear(int set=0);
    extern void Horizontal(int y, int set=0);
    extern void Vertical(int x, int set=0);

    extern void drawLine(int8_t x0, int8_t y0, int8_t x1, int8_t y1, uint8_t color);
    extern void drawRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t color);
    extern void fillRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t color);
    extern void drawCircle(uint8_t x0, uint8_t y0, uint8_t r, uint8_t color);
    extern void fillCircle(uint8_t x0, uint8_t y0, uint8_t r, uint8_t color);
    
};

#endif
