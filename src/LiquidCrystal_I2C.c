/*
 * LiquidCrystal_I2C.c - Liquid Crystal I2C (PCF8574) Library for STM32
 *
 *  Created on: March 12, 2020
 *      Author: S. Saeid Hosseini (sayidhosseini@hotmail.com)
 *      Ported from DFRobot: https://www.dfrobot.com/product-135.html
 *      GitHub Repo: https://github.com/SayidHosseini/STM32LiquidCrystal
 */

#include "LiquidCrystal_I2C.h"
#include <inttypes.h>
#include <Arduino.h>
#include <Wire.h>


uint8_t _addr;
uint8_t _displayfunction;
uint8_t _displaycontrol;
uint8_t _displaymode;
uint8_t _cols;
uint8_t _rows;
uint8_t _charsize;
uint8_t _backlightval;
uint8_t _row_offsets[4];

void LiquidCrystal_I2C(uint8_t lcd_addr, uint8_t lcd_cols, uint8_t lcd_rows) {
	_addr = lcd_addr;
	_cols = lcd_cols;
	_rows = lcd_rows;
	_charsize = LCD_5x8DOTS;
	_backlightval = LCD_BACKLIGHT;
}

void lcdBegin(void) {
	Wire.begin();
	_displayfunction = LCD_4BITMODE | LCD_1LINE | LCD_5x8DOTS;

	if (_rows > 1) {
		_displayfunction |= LCD_2LINE;
	}

	setRowOffsets(0x00, 0x40, 0x00 + _cols, 0x40 + _cols);

	// for some 1 line displays you can select a 10 pixel high font
	if ((_charsize != 0) && (_rows == 1)) {
		_displayfunction |= LCD_5x10DOTS;
	}

	// SEE PAGE 45/46 FOR INITIALIZATION SPECIFICATION!
	// according to datasheet, we need at least 40ms after power rises above 2.7V
	// before sending commands. Arduino can turn on way befer 4.5V so we'll wait 50
	delay(50);

	// Now we pull both RS and R/W low to begin commands
	write8bits(_backlightval);	// reset expanderand turn backlight off (Bit 8 =1)
	delay(1000);

	//put the LCD into 4 bit mode
	// this is according to the hitachi HD44780 datasheet
	// figure 24, pg 46

	// we start in 8bit mode, try to set 4 bit mode
	write4bits(0x03 << 4);
	delayMicroseconds(4500); // wait min 4.1ms

	// second try
	write4bits(0x03 << 4);
	delayMicroseconds(4500); // wait min 4.1ms

	// third go!
	write4bits(0x03 << 4);
	delayMicroseconds(150);

	// finally, set to 4-bit interface
	write4bits(0x02 << 4);

	// set # lines, font size, etc.
	command(LCD_FUNCTIONSET | _displayfunction);

	// turn the display on with no cursor or blinking default
	_displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
	lcdDisplay();

	// clear it off
	lcdClear();

	// Initialize to default text direction (for roman languages)
	_displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;

	// set the entry mode
	command(LCD_ENTRYMODESET | _displaymode);

	lcdHome();
}

/********** high level commands, for the user! */
void lcdClear(void) {
	command(LCD_CLEARDISPLAY);// clear display, set cursor position to zero
	delayMicroseconds(2000);  // this command takes a long time!
}

void lcdHome(void) {
	command(LCD_RETURNHOME);  // set cursor position to zero
	delayMicroseconds(2000);  // this command takes a long time!
}

// Turn the backlight off/on
void lcdNoBacklight(void) {
	_backlightval = LCD_NOBACKLIGHT;
	write8bits(0);
}
void lcdBacklight(void) {
	_backlightval = LCD_BACKLIGHT;
	write8bits(0);
}

// Turn the display on/off (quickly)
void lcdNoDisplay(void) {
	_displaycontrol &= ~LCD_DISPLAYON;
	command(LCD_DISPLAYCONTROL | _displaycontrol);
}
void lcdDisplay(void) {
	_displaycontrol |= LCD_DISPLAYON;
	command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// Turn on and off the blinking cursor
void lcdNoBlink(void) {
	_displaycontrol &= ~LCD_BLINKON;
	command(LCD_DISPLAYCONTROL | _displaycontrol);
}
void lcdBlink(void) {
	_displaycontrol |= LCD_BLINKON;
	command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// Turns the underline cursor on/off
void lcdNoCursor(void) {
	_displaycontrol &= ~LCD_CURSORON;
	command(LCD_DISPLAYCONTROL | _displaycontrol);
}
void lcdCursor(void) {
	_displaycontrol |= LCD_CURSORON;
	command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// These commands scroll the display without changing the RAM
void lcdScrollDisplayLeft(void) {
	command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT);
}
void lcdScrollDisplayRight(void) {
	command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT);
}

// This is for text that flows Left to Right
void lcdLeftToRight(void) {
	_displaymode |= LCD_ENTRYLEFT;
	command(LCD_ENTRYMODESET | _displaymode);
}

// This is for text that flows Right to Left
void lcdRightToLeft(void) {
	_displaymode &= ~LCD_ENTRYLEFT;
	command(LCD_ENTRYMODESET | _displaymode);
}

// This will 'right justify' text from the cursor
void lcdAutoscroll(void) {
	_displaymode |= LCD_ENTRYSHIFTINCREMENT;
	command(LCD_ENTRYMODESET | _displaymode);
}

// This will 'left justify' text from the cursor
void lcdNoAutoscroll(void) {
	_displaymode &= ~LCD_ENTRYSHIFTINCREMENT;
	command(LCD_ENTRYMODESET | _displaymode);
}

// Allows us to fill the first 8 CGRAM locations
// with custom characters
void lcdCreateChar(uint8_t location, uint8_t charmap[]) {
	location &= 0x7; // we only have 8 locations 0-7
	command(LCD_SETCGRAMADDR | (location << 3));
	for (int i = 0; i < 8; i++) {
		lcdWrite(charmap[i]);
	}
}

void lcdSetCursor(uint8_t col, uint8_t row) {
	const size_t max_lines = sizeof(_row_offsets) / sizeof(*_row_offsets);
	if ( row >= max_lines ) {
		row = max_lines - 1;    // we count rows starting w/0
	}
	if ( row >= _rows ) {
		row = _rows - 1;   		// we count rows starting w/0
	}

	command(LCD_SETDDRAMADDR | (col + _row_offsets[row]));
}

// This will print character string to the LCD
size_t lcdPrint(const char str[]) {
	if (str == NULL) return 0;

	const uint8_t *buffer = (const uint8_t *)str;
	size_t size = strlen(str);
	size_t n = 0;

	while (size--) {
	if (lcdWrite(*buffer++)) n++;
	else break;
	}
	return n;
}

/*********** mid level commands, for sending data/cmds */
inline size_t lcdWrite(uint8_t value) {
	send(value, Rs);
	return 1;
}

inline void command(uint8_t value) {
	send(value, 0);
}

/************ low level data pushing commands **********/
void setRowOffsets(int row0, int row1, int row2, int row3) {
	_row_offsets[0] = row0;
	_row_offsets[1] = row1;
	_row_offsets[2] = row2;
	_row_offsets[3] = row3;
}

void pulseEnable(uint8_t _data) {
	Write8bits(_data | En);		// En high
	delayMicroseconds(1);		// enable pulse must be >450ns

	write8bits(_data & ~En);	// En low
	delayMicroseconds(50);		// commands need > 37us to settle
}

// write either command or data
void send(uint8_t value, uint8_t mode) {
	uint8_t highnib = value & 0xf0;
	uint8_t lownib = (value << 4) & 0xf0;
	write4bits((highnib) | mode);
	write4bits((lownib) | mode);
}

void write4bits(uint8_t value) {
	write8bits(value);
	pulseEnable(value);
}

void write8bits(uint8_t _data){
	Wire.beginTransmission(_addr);
	Wire.write((int)(_data) | _backlightval);
	Wire.endTransmission();
}
