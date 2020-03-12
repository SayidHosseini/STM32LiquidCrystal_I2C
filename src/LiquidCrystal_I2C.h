/*
 * LiquidCrystal_I2C.h - Liquid Crystal I2C (PCF8574) Library for STM32
 *
 *  Created on: March 12, 2020
 *      Author: S. Saeid Hosseini (sayidhosseini@hotmail.com)
 *      Ported from DFRobot: https://www.dfrobot.com/product-135.html
 *      GitHub Repo: https://github.com/SayidHosseini/STM32LiquidCrystal
 */

#ifndef LIQUID_CRYSTAL_I2C_H
#define LIQUID_CRYSTAL_I2C_H

#include <inttypes.h>
#include <Print.h>

// commands
#define LCD_CLEARDISPLAY 0x01
#define LCD_RETURNHOME 0x02
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_CURSORSHIFT 0x10
#define LCD_FUNCTIONSET 0x20
#define LCD_SETCGRAMADDR 0x40
#define LCD_SETDDRAMADDR 0x80

// flags for display entry mode
#define LCD_ENTRYRIGHT 0x00
#define LCD_ENTRYLEFT 0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

// flags for display on/off control
#define LCD_DISPLAYON 0x04
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSORON 0x02
#define LCD_CURSOROFF 0x00
#define LCD_BLINKON 0x01
#define LCD_BLINKOFF 0x00

// flags for display/cursor shift
#define LCD_DISPLAYMOVE 0x08
#define LCD_CURSORMOVE 0x00
#define LCD_MOVERIGHT 0x04
#define LCD_MOVELEFT 0x00

// flags for function set
#define LCD_8BITMODE 0x10
#define LCD_4BITMODE 0x00
#define LCD_2LINE 0x08
#define LCD_1LINE 0x00
#define LCD_5x10DOTS 0x04
#define LCD_5x8DOTS 0x00

// flags for backlight control
#define LCD_BACKLIGHT 0x08
#define LCD_NOBACKLIGHT 0x00

#define En B00000100  // Enable bit
#define Rw B00000010  // Read/Write bit
#define Rs B00000001  // Register select bit

void LiquidCrystal_I2C(uint8_t, uint8_t, uint8_t);
void lcdBegin(void);
void lcdClear(void);
void lcdHome(void);
void lcdNoBacklight(void);
void lcdBacklight(void);
void lcdNoDisplay(void);
void lcdDisplay(void);
void lcdNoBlink(void);
void lcdBlink(void);
void lcdNoCursor(void);
void lcdCursor(void);
void lcdScrollDisplayLeft(void);
void lcdScrollDisplayRight(void);
void lcdLeftToRight(void);
void lcdRightToLeft(void);
void lcdAutoscroll(void);
void lcdNoAutoscroll(void);
void lcdCreateChar(uint8_t, uint8_t[]);
void lcdSetCursor(uint8_t, uint8_t);
inline size_t lcdWrite(uint8_t);
size_t lcdPrint(const char []);

#endif // LIQUID_CRYSTAL_I2C_H
