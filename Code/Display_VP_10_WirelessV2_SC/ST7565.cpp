/*
$Id:$

ST7565 LCD library!

Copyright (C) 2010 Limor Fried, Adafruit Industries

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

// some of this code was written by <cstone@pobox.com> originally; it is in the public domain.
*/

//#include <Wire.h>
#ifdef __AVR__
#include <avr/pgmspace.h>
#include <util/delay.h>
#endif

#ifndef _delay_ms
  #define _delay_ms(t) delay(t)
#endif

#ifndef _BV
  #define _BV(bit) (1<<(bit))
#endif


#include <stdlib.h>

#include "ST7565.h"
#include "8ptFont_5_23_40_23.h"
#include "24ptFont_16_20_30_5.h"
#include "48ptFont_23_8_58_20.h"
#include "16ptFont_16_22_5_57.h"
#define ST7565_STARTBYTES 1

uint8_t is_reversed = 0;

// a handy reference to where the pages are on the screen
const uint8_t pagemap[] = {7,6,5,4,3,2,1,0};//{ 3, 2, 1, 0, 7, 6, 5, 4 };

// a 5x7 font table
const extern uint8_t PROGMEM font[];

// the memory buffer for the LCD
uint8_t st7565_buffer[1024] = { 
0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 
0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 
0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 
0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 
0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 
0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 
0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 
0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 

0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 
0x0, 0x0, 0x0, 0x3, 0x7, 0xF, 0x1F, 0x1F, 0x3F, 0x3F, 0x3F, 0x3F, 0x7, 0x0, 0x0, 0x0, 
0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 
0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 
0x0, 0x0, 0x7F, 0x3F, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 
0x0, 0x0, 0x1F, 0x3F, 0x70, 0x70, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 
0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x6, 0x6, 0x0, 0x0, 0x0, 0x3, 0x3, 
0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 

0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0xF, 0x7, 0x7, 
0x7, 0x3F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x3E, 0x0, 0x0, 
0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xF, 0x3F, 
0x70, 0x60, 0x60, 0x60, 0x60, 0x30, 0x7F, 0x3F, 0x0, 0x0, 0x1F, 0x3F, 0x70, 0x60, 0x60, 0x60, 
0x60, 0x39, 0xFF, 0xFF, 0x0, 0x6, 0x1F, 0x39, 0x60, 0x60, 0x60, 0x60, 0x30, 0x3F, 0x7F, 0x0, 
0x0, 0x60, 0xFF, 0xFF, 0x60, 0x60, 0x0, 0x7F, 0x7F, 0x70, 0x60, 0x60, 0x40, 0x0, 0x7F, 0x7F, 
0x0, 0x0, 0x0, 0x0, 0x7F, 0x7F, 0x0, 0x0, 0x0, 0x7F, 0x7F, 0x0, 0x0, 0x60, 0xFF, 0xFF, 
0x60, 0x60, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 

0x80, 0xF8, 0xFC, 0xFE, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xEF, 0xE7, 0xE7, 0xE3, 
0xF3, 0xF9, 0xFF, 0xFF, 0xFF, 0xF7, 0x7, 0x1F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F, 0xFF, 
0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x3F, 0x3F, 0x1F, 0xF, 0x7, 0x3, 0x0, 0x0, 0x0, 0xC0, 
0xE0, 0x60, 0x20, 0x20, 0x60, 0xE0, 0xE0, 0xE0, 0x0, 0x0, 0x80, 0xC0, 0xE0, 0x60, 0x20, 0x60, 
0x60, 0xE0, 0xE0, 0xE0, 0x0, 0x0, 0x80, 0xC0, 0x60, 0x60, 0x20, 0x60, 0x60, 0xE0, 0xE0, 0x0, 
0x0, 0x0, 0xE0, 0xE0, 0x0, 0x0, 0x0, 0xE0, 0xE0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x80, 0xE0, 
0x60, 0x60, 0x60, 0x60, 0xE0, 0x80, 0x0, 0x0, 0x0, 0xE0, 0xE0, 0x0, 0x0, 0x0, 0xE0, 0xE0, 
0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 

0x0, 0x0, 0x0, 0x3, 0x7, 0x1F, 0x9F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFD, 0xF1, 0xE3, 
0xE3, 0xCF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF0, 0xFC, 0x7F, 0x3F, 0x3F, 0x3F, 0x3F, 0x7F, 0xFF, 0xFF, 
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0xFC, 0xF0, 0xE0, 0x80, 0x0, 0x0, 0x0, 0xC, 
0x1C, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 
0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x7F, 0x7F, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 
0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x7, 0x7, 0x0, 
0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1C, 0xC, 0x0, 0x0, 0x0, 0x0, 0x0, 
0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 

0x0, 0x7, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0xFE, 0xFE, 0xFE, 0xFC, 0xF8, 
0xF8, 0xF0, 0xFE, 0xFF, 0xFF, 0xFF, 0x7F, 0x3F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x1F, 
0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF, 
0xFF, 0x0, 0x0, 0x0, 0xFF, 0xFF, 0xE0, 0xC0, 0xC0, 0xC0, 0xFF, 0x7F, 0x0, 0x0, 0x1E, 0x7F, 
0xE1, 0xC0, 0xC0, 0xC0, 0xC0, 0x61, 0xFF, 0xFF, 0x0, 0x0, 0xFE, 0xFF, 0x1, 0x0, 0x0, 0x0, 
0xFF, 0xFF, 0x0, 0x0, 0x21, 0xF9, 0xF8, 0xDC, 0xCC, 0xCF, 0x7, 0x0, 0xC0, 0xFF, 0xFF, 0xC0, 
0x80, 0x0, 0xFF, 0xFF, 0xC0, 0xC0, 0x80, 0x0, 0x0, 0xFF, 0xFF, 0x0, 0x0, 0x1F, 0x7F, 0xF9, 
0xC8, 0xC8, 0xC8, 0xC8, 0x79, 0x39, 0x0, 0x0, 0x71, 0xF9, 0xD8, 0xCC, 0xCE, 0x47, 0x3, 0x0, 

0x0, 0x0, 0x0, 0x0, 0x80, 0x80, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 
0x0, 0x0, 0x0, 0x80, 0xC0, 0xE0, 0xF0, 0xF8, 0xF8, 0xFC, 0xFC, 0xFC, 0xFC, 0xF8, 0xF0, 0xC0, 
0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xC0, 
0xC0, 0x0, 0x0, 0x0, 0xC0, 0xC0, 0x0, 0x0, 0x0, 0x0, 0xC0, 0xC0, 0x0, 0x0, 0x0, 0x80, 
0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0x80, 0xC0, 0xC0, 0x0, 0x0, 0x0, 0x80, 0xC0, 0xC0, 0xC0, 0xC0, 
0xC0, 0x80, 0x0, 0x0, 0x80, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0x0, 0x0, 0x0, 0xC0, 0xC0, 0x0, 
0x0, 0x0, 0xC0, 0x80, 0x0, 0x0, 0x0, 0x0, 0x0, 0xC0, 0xC0, 0x0, 0x0, 0x0, 0x80, 0xC0, 
0xC0, 0xC0, 0xC0, 0xC0, 0x80, 0x80, 0x0, 0x0, 0x80, 0xC0, 0xC0, 0xC0, 0xC0, 0x80, 0x0, 0x0, 

0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,};

// reduces how much is refreshed, which speeds it up!
// originally derived from Steve Evans/JCW's mod but cleaned up and
// optimized
#define enablePartialUpdate

#ifdef enablePartialUpdate
static uint8_t xUpdateMin, xUpdateMax, yUpdateMin, yUpdateMax;
#endif



static void updateBoundingBox(uint8_t xmin, uint8_t ymin, uint8_t xmax, uint8_t ymax) {
#ifdef enablePartialUpdate
  if (xmin < xUpdateMin) xUpdateMin = xmin;
  if (xmax > xUpdateMax) xUpdateMax = xmax;
  if (ymin < yUpdateMin) yUpdateMin = ymin;
  if (ymax > yUpdateMax) yUpdateMax = ymax;
#endif
}

void ST7565::drawbitmap(uint8_t x, uint8_t y, 
			const uint8_t *bitmap, uint8_t w, uint8_t h,
			uint8_t color) {
  for (uint8_t j=0; j<h; j++) {
    for (uint8_t i=0; i<w; i++ ) {
      if (pgm_read_byte(bitmap + i + (j/8)*w) & _BV(j%8)) {
	      my_setpixel(x+i, y+j, color);
      }
    }
  }

  updateBoundingBox(x, y, x+w, y+h);
}

int ST7565::drawBigString_8(uint8_t x, uint8_t line, char *c, int maxX) {
  if (line >= (LCDHEIGHT/8)){
      return 0;        // ran out of space :(
  }
  if(x > maxX){
    return 0;
  }
  fillrect(x,line*8,maxX-x,8,0);
  int wid = 0;
  int startX = x;
  for(int i=0;i<30;i++){ //30, magic #, longest printable string
    if(c[i] == '\0'){
      return wid;
    }
    int wd = drawBigChar_8(x, line, c[i]);
    if(wd == 0){
      drawchar(x,line,c[i]);
      wd = 6;
    }
    x += wd;
    wid += wd;
    if (x >= LCDWIDTH) {
      return LCDWIDTH-startX;
    }
  }
  return wid;
}

int ST7565::drawBigString_16(uint8_t x, uint8_t line, char *c, int maxX) {
  if (line >= (LCDHEIGHT/8)){
      return 0;        // ran out of space :(
  }
  if(x > maxX){
    return 0;
  }
  fillrect(x,line*8,maxX-x,16,0);
  int wid = 0;
  int startX = x;
  for(int i=0;i<24;i++){ //24, magic #, longest printable string
    if(c[i] == '\0'){
      return wid;
    }
    int wd = drawBigChar_16(x, line, c[i]);
    x += wd;
    wid += wd;
    if (x >= LCDWIDTH) {
      return LCDWIDTH-startX;
    }
  }
  return wid;
}

int ST7565::drawBigString_24(uint8_t x, uint8_t line, char *c, int maxX) {
  if (line >= (LCDHEIGHT/8)){
      return 0;        // ran out of space :(
  }
  if(x > maxX){
    return 0;
  }
  fillrect(x,line*8,maxX-x,24,0);
  int wid = 0;
  int startX = x;
  for(int i=0;i<24;i++){
    if(c[i] == '\0'){
      return wid;
    }
    int wd = drawBigChar_24(x, line, c[i]);
    x += wd;
    wid += wd;
    if (x >= LCDWIDTH) {
      return LCDWIDTH-startX;
    }
  }
  return wid;
}
int ST7565::drawBigString_48(uint8_t x, uint8_t line, char *c,int maxX) {
  if (line >= (LCDHEIGHT/8)){
      return 0;        // ran out of space :(
  }
  if(x > maxX){
    return 0;
  }
  fillrect(x,line*8,maxX-x,48,0);
  int wid = 0;
  int startX = x;
  for(int i=0;i<24;i++){
    if(c[i] == '\0'){
      return wid;
    }
    int wd = drawBigChar_48(x, line, c[i]);
    x += wd;
    wid += wd;
    if (x >= LCDWIDTH) {
      return LCDWIDTH-startX;
    }
  }
  return wid;
}

int ST7565::drawString(uint8_t x, uint8_t line, char *c, int maxX) {
  if (line >= (LCDHEIGHT/8)){
      return 0;        // ran out of space :(
  }
  if(x > maxX){
    return 0;
  }
  fillrect(x,line*8,maxX-x,8,0);//TODO make all these loops non infinite
  int wd = 0;
  for(int i=0;i<24;i++){
    if(c[i] == '\0'){
      return wd;
    }
    drawchar(x, line, c[i]);
    x += 6; // 6 pixels wide
    wd += 6;
    if (x + 6 >= LCDWIDTH) {
      return wd;
    }
  }
  return wd;
}


void ST7565::drawString_P(uint8_t x, uint8_t line, const char *str) {
  while (1) {
    char c = pgm_read_byte(str++);
    if (! c)
      return;
    drawchar(x, line, c);
    x += 6; // 6 pixels wide
    if (x + 6 >= LCDWIDTH) {
      x = 0;    // ran out of this line
      line++;
    }
    if (line >= (LCDHEIGHT/8))
      return;        // ran out of space :(
  }
}

void  ST7565::drawchar(uint8_t x, uint8_t line, char c) {
  for (uint8_t i =0; i<5; i++ ) {
    st7565_buffer[x + (line*128) ] = pgm_read_byte(font+(c*5)+i);
    x++;
  }

  updateBoundingBox(x-5, line*8, x+5, line*8 + 8);
}

uint8_t  ST7565::drawBigChar_8(uint8_t x, uint8_t line, char c) {
  int ofst = fontOffsets_8[c-fontAsciiOffset_8];
  int wd = fontWidths_8[c-fontAsciiOffset_8];
  if((c < fontAsciiOffset_8) || (c >= fontAsciiOffset_8+fontNumChars_8) || (wd<=0)){
    return 0;
  }
  uint8_t retVal = (uint8_t)wd;
  for (uint8_t i =0; i<wd; i++ ) {
    if(x+i >= LCDWIDTH){
      retVal = i;
      break;
    }else{
      st7565_buffer[i+x+(line*128)] = fontData_8[ofst+i];
    }
  }
  updateBoundingBox(x, line*8, x+wd, line*8 + 8);
  return retVal;
}

uint8_t  ST7565::drawBigChar_16(uint8_t x, uint8_t line, char c) {
  int ofst = fontOffsets_16[c-fontAsciiOffset_16];
  int wd = fontWidths_16[c-fontAsciiOffset_16];
  if((c < fontAsciiOffset_16) || (c >= fontAsciiOffset_16+fontNumChars_16)){
    return 0;
  }
  uint8_t retVal = (uint8_t)wd;
  for(uint8_t j=0;j<2;j++){
    for (uint8_t i =0; i<wd; i++ ) {
      if(x+i >= LCDWIDTH){
        retVal = i;
        break;
      }else{
        st7565_buffer[i+x+((line+j)*128)] = fontData_16[ofst+i+((int)j*wd)];
      }
    }
  }
  updateBoundingBox(x, line*8, x+wd, line*8 + 16);
  return retVal;
}

uint8_t  ST7565::drawBigChar_24(uint8_t x, uint8_t line, char c) {
  int ofst = fontOffsets_24[c-fontAsciiOffset_24];
  int wd = fontWidths_24[c-fontAsciiOffset_24];
  if((c < fontAsciiOffset_24) || (c >= fontAsciiOffset_24+fontNumChars_24)){
    return 0;
  }
  uint8_t retVal = (uint8_t)wd;
  for(uint8_t j=0;j<3;j++){
    for (uint8_t i =0; i<wd; i++ ) {
      if(x+i >= LCDWIDTH){
        retVal = i;
        break;
      }else{
        st7565_buffer[i+x+((line+j)*128)] = fontData_24[ofst+i+((int)j*wd)];//pgm_read_byte(font+(c*5)+i);
      }
    }
  }
  updateBoundingBox(x, line*8, x+wd, line*8 + 24);
  return retVal;
}

uint8_t  ST7565::drawBigChar_48(uint8_t x, uint8_t line, char c) {
  int ofst = fontOffsets_48[c-fontAsciiOffset_48];
  int wd = fontWidths_48[c-fontAsciiOffset_48];
  if((c < fontAsciiOffset_48) || (c >= fontAsciiOffset_48+fontNumChars_48)){
    return 0;
  }
  uint8_t retVal = (uint8_t)wd;
  for(uint8_t j=0;j<6;j++){
    for (uint8_t i =0; i<wd; i++ ) {
      if(x+i >= LCDWIDTH){
        //updateBoundingBox(x, line*8, x+i, line*8 + 48);
        retVal = i;
        break;
      }
      st7565_buffer[i+x+((line+j)*128)] = fontData_48[ofst+i+(j*wd)];//pgm_read_byte(font+(c*5)+i);
    }
  }
  updateBoundingBox(x, line*8, x+retVal, line*8 + 48);
  return retVal;
}


// bresenham's algorithm - thx wikpedia
void ST7565::drawline(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, 
		      uint8_t color) {
  uint8_t steep = abs(y1 - y0) > abs(x1 - x0);
  if (steep) {
    STswap(x0, y0);
    STswap(x1, y1);
  }

  if (x0 > x1) {
    STswap(x0, x1);
    STswap(y0, y1);
  }

  // much faster to put the test here, since we've already sorted the points
  updateBoundingBox(x0, y0, x1, y1);

  uint8_t dx, dy;
  dx = x1 - x0;
  dy = abs(y1 - y0);

  int8_t err = dx / 2;
  int8_t ystep;

  if (y0 < y1) {
    ystep = 1;
  } else {
    ystep = -1;}

  for (; x0<=x1; x0++) {
    if (steep) {
      my_setpixel(y0, x0, color);
    } else {
      my_setpixel(x0, y0, color);
    }
    err -= dy;
    if (err < 0) {
      y0 += ystep;
      err += dx;
    }
  }
}

// filled rectangle
void ST7565::fillrect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, 
		      uint8_t color) {

  // stupidest version - just pixels - but fast with internal buffer!
  for (uint8_t i=x; i<x+w; i++) {
    for (uint8_t j=y; j<y+h; j++) {
      my_setpixel(i, j, color);
    }
  }

  updateBoundingBox(x, y, x+w, y+h);
}

// draw a rectangle
void ST7565::drawrect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, 
		      uint8_t color) {
  // stupidest version - just pixels - but fast with internal buffer!
  for (uint8_t i=x; i<x+w; i++) {
    my_setpixel(i, y, color);
    my_setpixel(i, y+h-1, color);
  }
  for (uint8_t i=y; i<y+h; i++) {
    my_setpixel(x, i, color);
    my_setpixel(x+w-1, i, color);
  } 

  updateBoundingBox(x, y, x+w, y+h);
}

// draw a circle outline
void ST7565::drawcircle(uint8_t x0, uint8_t y0, uint8_t r, 
			uint8_t color) {
  updateBoundingBox(x0-r, y0-r, x0+r, y0+r);

  int8_t f = 1 - r;
  int8_t ddF_x = 1;
  int8_t ddF_y = -2 * r;
  int8_t x = 0;
  int8_t y = r;

  my_setpixel(x0, y0+r, color);
  my_setpixel(x0, y0-r, color);
  my_setpixel(x0+r, y0, color);
  my_setpixel(x0-r, y0, color);

  while (x<y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;
  
    my_setpixel(x0 + x, y0 + y, color);
    my_setpixel(x0 - x, y0 + y, color);
    my_setpixel(x0 + x, y0 - y, color);
    my_setpixel(x0 - x, y0 - y, color);
    
    my_setpixel(x0 + y, y0 + x, color);
    my_setpixel(x0 - y, y0 + x, color);
    my_setpixel(x0 + y, y0 - x, color);
    my_setpixel(x0 - y, y0 - x, color);
    
  }



}

void ST7565::fillcircle(uint8_t x0, uint8_t y0, uint8_t r, 
			uint8_t color) {
  updateBoundingBox(x0-r, y0-r, x0+r, y0+r);

  int8_t f = 1 - r;
  int8_t ddF_x = 1;
  int8_t ddF_y = -2 * r;
  int8_t x = 0;
  int8_t y = r;

  for (uint8_t i=y0-r; i<=y0+r; i++) {
    my_setpixel(x0, i, color);
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
  
    for (uint8_t i=y0-y; i<=y0+y; i++) {
      my_setpixel(x0+x, i, color);
      my_setpixel(x0-x, i, color);
    } 
    for (uint8_t i=y0-x; i<=y0+x; i++) {
      my_setpixel(x0+y, i, color);
      my_setpixel(x0-y, i, color);
    }    
  }
}

void ST7565::my_setpixel(uint8_t x, uint8_t y, uint8_t color) {
  if ((x >= LCDWIDTH) || (y >= LCDHEIGHT))
    return;

  // x is which column
  if (color) 
    st7565_buffer[x+ (y/8)*128] |= _BV(7-(y%8));  
  else
    st7565_buffer[x+ (y/8)*128] &= ~_BV(7-(y%8)); 
}

// the most basic function, set a single pixel
void ST7565::setpixel(uint8_t x, uint8_t y, uint8_t color) {
  if ((x >= LCDWIDTH) || (y >= LCDHEIGHT))
    return;

  // x is which column
  if (color) 
    st7565_buffer[x+ (y/8)*128] |= _BV(7-(y%8));  
  else
    st7565_buffer[x+ (y/8)*128] &= ~_BV(7-(y%8)); 

  updateBoundingBox(x,y,x,y);
}


// the most basic function, get a single pixel
uint8_t ST7565::getpixel(uint8_t x, uint8_t y) {
  if ((x >= LCDWIDTH) || (y >= LCDHEIGHT))
    return 0;

  return (st7565_buffer[x+ (y/8)*128] >> (7-(y%8))) & 0x1;  
}

void ST7565::begin(uint8_t contrast) {
  st7565_init();
  st7565_command(CMD_DISPLAY_ON);
  st7565_command(CMD_SET_ALLPTS_NORMAL);
  st7565_set_brightness(contrast);
}

void ST7565::st7565_init(void) {
  // set pin directions
  pinMode(sid, OUTPUT);
  pinMode(sclk, OUTPUT);
  pinMode(a0, OUTPUT);
  pinMode(rst, OUTPUT);
  pinMode(cs, OUTPUT);

  // toggle RST low to reset; CS low so it'll listen to us
  if (cs > 0){
    digitalWrite(cs, LOW);
  }

  digitalWrite(rst, LOW);
  _delay_ms(500);
  digitalWrite(rst, HIGH);

  // LCD bias select
  st7565_command(CMD_SET_BIAS_9);//biases are 7 and 9
  // ADC select
  st7565_command(CMD_SET_ADC_NORMAL);
  // SHL select
  st7565_command(CMD_SET_COM_NORMAL);
  // Initial display line
  st7565_command(CMD_SET_DISP_START_LINE);

  // turn on voltage converter (VC=1, VR=0, VF=0)
  st7565_command(CMD_SET_POWER_CONTROL | 0x4);
  // wait for 50% rising
  _delay_ms(50);

  // turn on voltage regulator (VC=1, VR=1, VF=0)
  st7565_command(CMD_SET_POWER_CONTROL | 0x6);
  // wait >=50ms
  _delay_ms(50);

  // turn on voltage follower (VC=1, VR=1, VF=1)
  st7565_command(CMD_SET_POWER_CONTROL | 0x7);
  // wait
  _delay_ms(10);

  // set lcd operating voltage (regulator resistor, ref voltage resistor)
  st7565_command(CMD_SET_RESISTOR_RATIO | 0x6);//0x4?

  // initial display line
  // set page address
  // set column address
  // write display data

  // set up a bounding box for screen updates

  updateBoundingBox(0, 0, LCDWIDTH-1, LCDHEIGHT-1);
}

inline void ST7565::spiwrite(uint8_t c) {
    
    int8_t i;
    for (i=7; i>=0; i--) {
        digitalWrite(sclk, LOW);
        delayMicroseconds(1);      //need to slow down the data rate for Due and Zero
        if (c & _BV(i))
            digitalWrite(sid, HIGH);
        else
            digitalWrite(sid, LOW);
        delayMicroseconds(1);      //need to slow down the data rate for Due and Zero
        digitalWrite(sclk, HIGH);
    }
  /*
  int8_t i;
  for (i=7; i>=0; i--) {
    SCLK_PORT &= ~_BV(SCLK);
    if (c & _BV(i))
      SID_PORT |= _BV(SID);
    else
      SID_PORT &= ~_BV(SID);
    SCLK_PORT |= _BV(SCLK);
  }
  */

  /*
  // loop unwrapped! too fast doesnt work :(
 
  SCLK_PORT &= ~_BV(SCLK);
  if (c & _BV(7))
    SID_PORT |= _BV(SID);
  else
    SID_PORT &= ~_BV(SID);
  SCLK_PORT |= _BV(SCLK);

  SCLK_PORT &= ~_BV(SCLK);
  if (c & _BV(6))
    SID_PORT |= _BV(SID);
  else
    SID_PORT &= ~_BV(SID);
  SCLK_PORT |= _BV(SCLK);
 
  SCLK_PORT &= ~_BV(SCLK);
  if (c & _BV(5))
    SID_PORT |= _BV(SID);
  else
    SID_PORT &= ~_BV(SID);
  SCLK_PORT |= _BV(SCLK);

  SCLK_PORT &= ~_BV(SCLK);
  if (c & _BV(4))
    SID_PORT |= _BV(SID);
  else
    SID_PORT &= ~_BV(SID);
  SCLK_PORT |= _BV(SCLK);

  SCLK_PORT &= ~_BV(SCLK);
  if (c & _BV(3))
    SID_PORT |= _BV(SID);
  else
    SID_PORT &= ~_BV(SID);
  SCLK_PORT |= _BV(SCLK);

  SCLK_PORT &= ~_BV(SCLK);
  if (c & _BV(2))
    SID_PORT |= _BV(SID);
  else
    SID_PORT &= ~_BV(SID);
  SCLK_PORT |= _BV(SCLK);


  SCLK_PORT &= ~_BV(SCLK);
  if (c & _BV(1))
    SID_PORT |= _BV(SID);
  else
    SID_PORT &= ~_BV(SID);
  SCLK_PORT |= _BV(SCLK);

  SCLK_PORT &= ~_BV(SCLK);
  if (c & _BV(0))
    SID_PORT |= _BV(SID);
  else
    SID_PORT &= ~_BV(SID);
  SCLK_PORT |= _BV(SCLK);
*/

}
void ST7565::st7565_command(uint8_t c) {
  digitalWrite(a0, LOW);

  spiwrite(c);
}

void ST7565::st7565_data(uint8_t c) {
  digitalWrite(a0, HIGH);

  spiwrite(c);
}
void ST7565::st7565_set_brightness(uint8_t val) {
    st7565_command(CMD_SET_VOLUME_FIRST);
    st7565_command(CMD_SET_VOLUME_SECOND | (val & 0x3f));
}


void ST7565::display(void) {
  uint8_t col, maxcol, p;

  /*
  Serial.print("Refresh ("); Serial.print(xUpdateMin, DEC); 
  Serial.print(", "); Serial.print(xUpdateMax, DEC);
  Serial.print(","); Serial.print(yUpdateMin, DEC); 
  Serial.print(", "); Serial.print(yUpdateMax, DEC); Serial.println(")");
  */

  for(p = 0; p < 8; p++) {
    /*
      putstring("new page! ");
      uart_putw_dec(p);
      putstring_nl("");
    */
#ifdef enablePartialUpdate
    // check if this page is part of update
    if ( yUpdateMin >= ((p+1)*8) ) {
      continue;   // nope, skip it!
    }
    if (yUpdateMax < p*8) {
      break;
    }
#endif

    st7565_command(CMD_SET_PAGE | pagemap[p]);


#ifdef enablePartialUpdate
    col = xUpdateMin;
    maxcol = xUpdateMax;
#else
    // start at the beginning of the row
    col = 0;
    maxcol = LCDWIDTH-1;
#endif

    st7565_command(CMD_SET_COLUMN_LOWER | ((col+ST7565_STARTBYTES) & 0xf));
    st7565_command(CMD_SET_COLUMN_UPPER | (((col+ST7565_STARTBYTES) >> 4) & 0x0F));
    st7565_command(CMD_RMW);
    
    for(; col <= maxcol; col++) {
      //uart_putw_dec(col);
      //uart_putchar(' ');
      st7565_data(st7565_buffer[(128*p)+col]);
    }
  }

#ifdef enablePartialUpdate
  xUpdateMin = LCDWIDTH - 1;
  xUpdateMax = 0;
  yUpdateMin = LCDHEIGHT-1;
  yUpdateMax = 0;
#endif
}

// clear everything
void ST7565::clear(void) {
  memset(st7565_buffer, 0, 1024);
  updateBoundingBox(0, 0, LCDWIDTH-1, LCDHEIGHT-1);
}


// this doesnt touch the buffer, just clears the display RAM - might be handy
void ST7565::clear_display(void) {
  uint8_t p, c;
  
  for(p = 0; p < 8; p++) {
    /*
      putstring("new page! ");
      uart_putw_dec(p);
      putstring_nl("");
    */

    st7565_command(CMD_SET_PAGE | p);
    for(c = 0; c < 129; c++) {
      //uart_putw_dec(c);
      //uart_putchar(' ');
      st7565_command(CMD_SET_COLUMN_LOWER | (c & 0xf));
      st7565_command(CMD_SET_COLUMN_UPPER | ((c >> 4) & 0xf));
      st7565_data(0x0);
    }     
  }
}
