/*
$Id:$

ST7565 LCD library!

Copyright (C) 2010 Limor Fried, Adafruit Industries

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

*/

//#include <avr/io.h>
#include <stdlib.h>
#include <at89c51ed2.h>
//#include <avr/pgmspace.h>
#include "glcd.h"
#include "string.h"
#include "Serial.h"
#include <stdio.h>

extern unsigned char font[];

// the most basic function, set a single pixel
void setpixel(unsigned int *buff, unsigned int x, unsigned int y, unsigned int color) {
  if ((x >= LCDWIDTH) || (y >= LCDHEIGHT))
    return;

  // x is which column
//  if (color)
//    buff[x+ (y/8)*128] |= 7-(y%8);
//  else
//    buff[x+ (y/8)*128] &= ~(7-(y%8));

    if (color)
    buff[x+ (y/8)*128] |= 0x01;
  else
    buff[x+ (y/8)*128] &= 0x00;


//    printf("%02x",(buff[x+(y/8)*128]));
}

void drawbitmap(unsigned char *buff, unsigned char x, unsigned char y,
		const unsigned char bitmap, unsigned char w, unsigned char h,
		unsigned char color) {
		    unsigned char j=0,i=0;
  for ( j=0; j<h; j++) {
    for ( i=0; i<w; i++ ) {
      if ((bitmap + i + (j/8)*w) &(j%8)) {
	setpixel(buff, x+i, y+j, color);
      }
    }
  }


}

void drawstring(unsigned char *buff, unsigned char x, unsigned char line, unsigned char *c) {
  while (c[0] != 0) {
    putchar(c[0]);
    drawchar(buff, x, line, c[0]);
    c++;
    x += 6; // 6 pixels wide
    if (x + 6 >= LCDWIDTH) {
      x = 0;    // ran out of this line
      line++;
    }
    if (line >= (LCDHEIGHT/8))
      return;        // ran out of space :(
  }

}

// the most basic function, clear a single pixel
void clearpixel(unsigned char *buff, unsigned char x, unsigned char y) {
  // x is which column
  buff[x+ (y/8)*128] &= ~(7-(y%8));
}


// bresenham's algorithm - thx wikpedia
void drawline(unsigned int *buff,
	      unsigned int x0, unsigned int y0, unsigned int x1, unsigned int y1,
	      unsigned int color) {

  unsigned char steep = abs(y1 - y0) > abs(x1 - x0);unsigned char dx=0, dy;unsigned char err = dx / 2;
  unsigned char ystep;
  if (steep) {
    swap(x0, y0);
    swap(x1, y1);
  }

  if (x0 > x1) {
    swap(x0, x1);
    swap(y0, y1);
  }


  dx = x1 - x0;
  dy = abs(y1 - y0);



  if (y0 < y1) {
    ystep = 1;
  } else {
    ystep = -1;}

  for (; x0<x1; x0++) {
    if (steep) {
      setpixel(buff, y0, x0, color);
    } else {
      setpixel(buff, x0, y0, color);
    }
    err -= dy;
    if (err < 0) {
      y0 += ystep;
      err += dx;
    }
  }
}


// filled rectangle
void fillrect(unsigned char *buff,
	      unsigned char x, unsigned char y, unsigned char w, unsigned char h,
	      unsigned char color) {
    unsigned char i,j;
  // stupidest version - just pixels - but fast with internal buffer!
  for (i=x; i<x+w; i++) {
    for (j=y; j<y+h; j++) {
      setpixel(buff, i, j, color);
    }
  }
}


void drawchar(unsigned char *buff, unsigned char x, unsigned char line, unsigned char c) {
    unsigned char i;
  for (i =0; i<5; i++ ) {
    buff[x + (line*128) ] = font[(c*5)+i];
    x++;
  }
}


// draw a rectangle
/**************************************************************************
buff - buffer to be stored
x - co-ordinate
y - co-ordinate
w- width of rectangle
h - height of rectangle
color - 1 pixel on
        0 pixel off

**************************************************************************/
void drawrect(unsigned char *buff,
	      unsigned char x, unsigned char y, unsigned char w, unsigned char h,
	      unsigned char color) {
	          unsigned char i;
  for (i=x; i<x+w; i++) {
    setpixel(buff, i, y, color);
    setpixel(buff, i, y+h-1, color);
  }
  for (i=y; i<y+h; i++) {
    setpixel(buff, x, i, color);
    setpixel(buff, x+w-1, i, color);
  }
}


// draw a circle
void drawcircle(unsigned char *buff,
	      unsigned char x0, unsigned char y0, unsigned char r,
	      unsigned char color) {
  unsigned char f = 1 - r;
  unsigned char ddF_x = 1;
  unsigned char ddF_y = -2 * r;
  unsigned char x = 0;
  unsigned char y = r;

  setpixel(buff, x0, y0+r, color);
  setpixel(buff, x0, y0-r, color);
  setpixel(buff, x0+r, y0, color);
  setpixel(buff, x0-r, y0, color);

  while (x<y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;

    setpixel(buff, x0 + x, y0 + y, color);
    setpixel(buff, x0 - x, y0 + y, color);
    setpixel(buff, x0 + x, y0 - y, color);
    setpixel(buff, x0 - x, y0 - y, color);

    setpixel(buff, x0 + y, y0 + x, color);
    setpixel(buff, x0 - y, y0 + x, color);
    setpixel(buff, x0 + y, y0 - x, color);
    setpixel(buff, x0 - y, y0 - x, color);

  }
}


// draw a circle
void fillcircle(unsigned char *buff,
	      unsigned char x0, unsigned char y0, unsigned char r,
	      unsigned char color) {
  unsigned char f = 1 - r;
  unsigned char ddF_x = 1;
  unsigned char ddF_y = -2 * r;
  unsigned char x = 0;
  unsigned char y = r;
  unsigned char i;

  for (i=y0-r; i<=y0+r; i++) {
    setpixel(buff, x0, i, color);
  }

  while (x<y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;

    for (i=y0-y; i<=y0+y; i++) {
      setpixel(buff, x0+x, i, color);
      setpixel(buff, x0-x, i, color);
    }
    for (i=y0-x; i<=y0+x; i++) {
      setpixel(buff, x0+y, i, color);
      setpixel(buff, x0-y, i, color);
    }
  }
}


// clear everything
void clear_buffer(unsigned char *buff) {
  memset(buff, 0, 1024);
}
