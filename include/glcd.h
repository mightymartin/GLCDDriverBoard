#ifndef GLCD_H
#define GLCD_H

#include <Arduino.h>
#include "font.h"
#include "loading_img.h"
#include "ks0108.h"

//######################################
//## Hardware Definition
//######################################
#define GLCD_PIN_D0     10
#define GLCD_PIN_D1     11
#define GLCD_PIN_D2     12
#define GLCD_PIN_D3     13
#define GLCD_PIN_D4     14
#define GLCD_PIN_D5     15
#define GLCD_PIN_D6     16
#define GLCD_PIN_D7     17
#define GLCD_PIN_EN     9    
#define GLCD_PIN_DC     7
#define GLCD_PIN_CS1    6
#define GLCD_PIN_CS2    5
#define GLCD_PIN_RST    4
#define GLCD_PIN_RW     8

#define GLCD_PIN_LED    19

//######################################
//## Font Config
//######################################
#define GLCD_COLS 21
#define GLCD_ROWS 7
#define GLCD_FONT_Y 7
#define GLCD_FONT_X 5


extern void GLCDInit();
extern void GLCDSetDirty();
extern void GLCDTick();
extern void GLCDdrawCharAt(uint8_t x_pos, uint8_t y_pos, uint8_t c, uint8_t inverted, uint8_t underline, uint8_t spacer);
extern void GLCDdrawStringAt(uint8_t x_pos, uint8_t y_pos, char* string, uint8_t inverted, uint8_t underline);
extern void GLCDInvert();
extern void GLCDClear();
extern void GLCDBacklight(uint8_t val);

#endif