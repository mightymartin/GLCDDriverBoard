#include "ks0108.h"

// ###############################################################################################
// # 																		   					 #
// # Low Level functions	(HW-LAYER)															 #
// #																							 #
// ###############################################################################################
static int16_t cursor_x;
static uint8_t cursor_y,invert=0; // cursor position
static uint8_t frameBuffer[SCREEN_BYTES] ;

static volatile uint8_t _D0, _D1, _D2, _D3, _D4, _D5, _D6, _D7, _EN, _DC, _CS1, _CS2, _RST, _RW;

// ----------------------
// Controller selection
// ----------------------
void ks0108_chip_select(uint8_t chip)
{
	if (chip == (CHIP1|CHIP2)){
		digitalWrite(_CS1,HIGH);
		digitalWrite(_CS2,HIGH);
		return;
	}

	if (chip == CHIP1){
		digitalWrite(_CS1,HIGH);
		digitalWrite(_CS2,LOW);
		return;
	}

	if (chip == CHIP2){ 							
	 	digitalWrite(_CS1,LOW);
		digitalWrite(_CS2,HIGH);
		return;
	}

	digitalWrite(_CS1,LOW);
	digitalWrite(_CS2,LOW);
	return;	
}



// ---------------------------------
// Set Command mode, and write data
// ---------------------------------
void ks0108_write_cmd(uint8_t cmd ,uint8_t chip)
{	
	ks0108_chip_select(chip);
	
	// we're in command mode
	digitalWrite(_DC,LOW);
	digitalWrite(_RW,LOW);

	ks0108_write(cmd);
}



// ---------------------------------
// Set Data mode, and write data
// ---------------------------------
void ks0108_write_data(uint8_t data ,uint8_t chip)
{
	if (invert!=0)	data = ~data;	
	ks0108_chip_select(chip);
	
	// we're in data mode
	digitalWrite(_DC,HIGH);
	digitalWrite(_RW,LOW);

	ks0108_write(data);
}



// ------------------------
// Write Data
// ------------------------
void ks0108_write(uint8_t val){
	digitalWrite(_D0,(val & (1 << 0)) ? HIGH : LOW);
	digitalWrite(_D1,(val & (1 << 1)) ? HIGH : LOW);
	digitalWrite(_D2,(val & (1 << 2)) ? HIGH : LOW);
	digitalWrite(_D3,(val & (1 << 3)) ? HIGH : LOW);
	digitalWrite(_D4,(val & (1 << 4)) ? HIGH : LOW);
	digitalWrite(_D5,(val & (1 << 5)) ? HIGH : LOW);
	digitalWrite(_D6,(val & (1 << 6)) ? HIGH : LOW);
	digitalWrite(_D7,(val & (1 << 7)) ? HIGH : LOW);
	
	digitalWrite(_EN,HIGH);
	delayMicroseconds(1);		
	digitalWrite(_EN,LOW);
	delayMicroseconds(1);			
}



//###############################################################################################
//# 																							#
//# Basic Functions																#
//#																								#
//###############################################################################################


// ----------------------
// Init LCD Controllers
// ----------------------
void ks0108_init(uint8_t D0, uint8_t D1, uint8_t D2, uint8_t D3, uint8_t D4, uint8_t D5, uint8_t D6, uint8_t D7, uint8_t EN, uint8_t DC, uint8_t CS1, uint8_t CS2, uint8_t RST, uint8_t RW){
	_D0 = D0;
	_D1 = D1;
	_D2 = D2;
	_D3 = D3;
	_D4 = D4;
	_D5 = D5;
	_D6 = D6;
	_D7 = D7;
	_EN = EN;
	_DC = DC;
	_CS1 = CS1;
	_CS2 = CS2;
	_RST = RST;
	_RW = RW;

	pinMode(_D0,OUTPUT);
	pinMode(_D1,OUTPUT);
	pinMode(_D2,OUTPUT);
	pinMode(_D3,OUTPUT);
	pinMode(_D4,OUTPUT);
	pinMode(_D5,OUTPUT);
	pinMode(_D6,OUTPUT);
	pinMode(_D7,OUTPUT);
	pinMode(_DC,OUTPUT);
	pinMode(_RW,OUTPUT);
	pinMode(_EN,OUTPUT);
	pinMode(_CS1,OUTPUT);
	pinMode(_CS2,OUTPUT);

	#ifndef LCD_NO_RES
		pinMode(_RST,OUTPUT);
	#endif

	ks0108_chip_select(CHIP1|CHIP2);

	#ifndef LCD_NO_RES
		digitalWrite(_RST,LOW);
		delay(30);
		digitalWrite(_RST,HIGH);		
	#endif

	ks0108_write_cmd(LCD_ON,CHIP1|CHIP2);
	ks0108_write_cmd(LCD_DISP_START|0,CHIP1|CHIP2);
	ks0108_write_cmd(LCD_SET_ADD|0,CHIP1|CHIP2);
	ks0108_write_cmd(LCD_SET_PAGE|0,CHIP1|CHIP2);

	

	//TODO ZEug !init framebuffer
	uint16_t byte=0;
	uint8_t x,y;
	cursor_x=0;cursor_y=0;	

	for (x=0;x<SCREEN_W;x++){
		for (y=0;y<8;y++){
			frameBuffer[byte++] = 0;			
		}	
	}
}



//----------------------------
// Writes Buffer to Display
//----------------------------
void ks0108_update(){
	// INFO:
	// data is organized page after page (0-to-127)
	// (LSB-to-MSB TOP-to-Bottom)
	// from 0 to 127 in eight lines (127*8=1024 Byte)

	uint16_t byte=0;
	uint8_t x,y;
	for (y=0;y<8;y++){
		ks0108_write_cmd(LCD_SET_PAGE|y,CHIP1|CHIP2);	//	start a new page
		ks0108_write_cmd(LCD_SET_ADD|0,CHIP1|CHIP2);	//  start at 0
		for (x=0;x<SCREEN_W;x++){
			if (x<(SCREEN_W/2)){			
				ks0108_write_data(frameBuffer[byte++],CHIP1);			
			}else{			
				ks0108_write_data(frameBuffer[byte++],CHIP2);			
			}			
		}	
	}
}



//----------------------------
// Clear the Display
//----------------------------
void ks0108_clear(){
	//init framebuffer
	uint16_t byte=0;
	uint8_t x,y;
	for (x=0;x<SCREEN_W;x++){
		for (y=0;y<8;y++){		
			frameBuffer[byte++] = 0;			
		}
	}
}


//----------------------------
// Set Pixel BLACK or WHITE
//----------------------------
void ks0108_set_pixel(int16_t x,uint8_t y, uint8_t color){	
	if ((x<SCREEN_W) && (y<SCREEN_H) && (x>=0) && (y>=0)){		// check position							
		uint16_t registerPos = (SCREEN_W * (y/8)) + x;		
		uint8_t temp= frameBuffer[registerPos];			// select page (row)

		uint8_t y_pos = (y%8);	

		if (color==BLACK){		
			temp |= (1<<y_pos);							// change pixel color to black
		}else{
			temp &= ~(1<<y_pos);						// change pixel color to white
		}

		frameBuffer[registerPos] = temp;			
	}	

}

void ks0108_set_8pixel(int16_t index, uint8_t value){	
	if ((index<SCREEN_BYTES) && (index>=0)){		// check position							
		frameBuffer[index] = value;			
	}	

}

// ---------------------------------------------
// inverts the screen and sets mode to inverted
// ---------------------------------------------
void ks0108_invert()
{
	invert++;
	invert %=2;
}


// ------------------------------------
// Draw an fullscreen Bitap 
// ------------------------------------
void ks0108_draw_fullscreen_bmp(const uint8_t* bitmap)
{  
	uint16_t byte=0;
	uint8_t x,y;
	for (y=0;y<8;y++){
		for (x=0;x<SCREEN_W;x++)		{			
				frameBuffer[byte] = pgm_read_byte(bitmap+(byte++));			
		}	
	}	
}




// ###############################################################################################
// # 																							 #
// # Text output																				 #
// #																							 #
// ###############################################################################################

#ifndef NO_TEXT
// ---------------------------------
// get the count of chars in font
// ---------------------------------
uint8_t ks0108_font_number_of_chars(const uint8_t* font)	
{	
	return(pgm_read_byte(font+5)); // return number of characters stored in this font	
}



// ---------------------------------
// get first char in font
// ---------------------------------
uint8_t ks0108_font_first_char(const uint8_t*font)	
{	
	return(pgm_read_byte(font+4)); // return the first used char of the font	
}



// ---------------------------------
// get width of given char
// ---------------------------------
uint8_t ks0108_font_char_width(const uint8_t* font,uint8_t chr)
{
	return(pgm_read_byte(font+6+(chr-ks0108_font_first_char(font))));	// return the width of the given character
}


// ----------------------------------------
// check if given char is present in font
// ----------------------------------------
uint8_t ks0108_font_char_present(const uint8_t* font,uint8_t chr)
{
	if ( (chr>= ks0108_font_first_char(font)) && (chr<=ks0108_font_first_char(font) + ks0108_font_number_of_chars(font)-1)) // check if char is present
		return(1);
	else
		return(0);
}

// ---------------------------------
// get height of font
// ---------------------------------
uint8_t ks0108_font_char_height(const uint8_t*font)
{
	return(pgm_read_byte(font+3)); // return the first used char of the font
}

// ---------------------------------
// get the start offset of char
// ---------------------------------
uint16_t ks0108_font_start_offset(const uint8_t*font,uint8_t chr) 
{
	uint16_t offset=(uint16_t)0x0000;
	uint8_t cnt;
	uint8_t height= (ks0108_font_char_height(font)-1)/8;

	for (cnt= ks0108_font_first_char(font);cnt<chr;cnt++)
	{
		offset+=ks0108_font_char_width(font,cnt) * (height+1); // width * (number of pages) needed for each char
	}	
	
	offset+=ks0108_font_number_of_chars(font)+6;				// add 6 for the header-bytes and the number of bytes need for the width-array

	return offset;
}

// ---------------------------------
// set cursor to x,y
// ---------------------------------
void ks0108_set_cursor(int16_t x, uint8_t y)
{
	if(x<SCREEN_W)	cursor_x=x;
	if(y<SCREEN_H) 	cursor_y=y;
}

// ---------------------------------
// put char to screen
// ---------------------------------
uint8_t ks0108_putc(const uint8_t* font,uint8_t chr, uint8_t withSpacerpixel, uint8_t inverted )
{
	int16_t x,page,bit,data,cnt=0;
	if (ks0108_font_char_present(font,chr)==1){	// print only if letter is present in font
		if ( ( (cursor_x + ks0108_font_char_width(font,chr)-1) <SCREEN_W)  && ((cursor_y+ks0108_font_char_height(font))<=SCREEN_H)){	// only print letter if it fits in screen
			uint16_t offset= ks0108_font_start_offset(font,chr);	// get the position of the first byte in font-array
			
			for(page=0;page<=(ks0108_font_char_height(font)-1)/8;page++){	// write all pages(rows)											
				for (x = cursor_x;x < cursor_x + ks0108_font_char_width(font,chr);x++){	//fill "pages"
					data= pgm_read_byte(font+offset+cnt++);

					uint8_t bitCount= 8;

					if ( page==ks0108_font_char_height(font)/8){		
						data>>=8-(ks0108_font_char_height(font)%8);	// if char height is bigger than 8 we have to remove some leading zeros					
						bitCount = 8-(ks0108_font_char_height(font)%8);
					}
					
					for (bit=0;bit<bitCount ;bit++){						
						if ((data&(1<<bit))!=0){							 	
							ks0108_set_pixel(x,cursor_y+page*8+bit,(inverted == 1 ? WHITE : BLACK));
						}else{
							ks0108_set_pixel(x,cursor_y+page*8+bit,(inverted == 1 ? BLACK : WHITE));
						}
					}						
					
				}							

				if(withSpacerpixel){
					for (bit=0;bit<8;bit++){
						ks0108_set_pixel(x,cursor_y+page*8+bit,(inverted == 1 ? BLACK : WHITE));
					}
				}
			}			
			cursor_x += ks0108_font_char_width(font,chr)-1;
			
			if(withSpacerpixel){				
				cursor_x++;
				cursor_x++;
			}
		}else{
			return 0;
		}
	}
	return 1;
}



// ---------------------------------
// put string to screen (from ram)
// ---------------------------------
void ks0108_puts(const uint8_t* font,char* string, uint8_t inverted ){
	while(*string){
		if(!ks0108_putc(font,*string++,1,inverted)){
			break;
		}
	}
}



// ---------------------------------------
// put string to screen (from flash)
// use: ks0108_puts_p(PSTR("HELLO WORLD"));
// ---------------------------------------
void ks0108_puts_p(const uint8_t* font,char* PROGMEM string, uint8_t inverted ){
	while(pgm_read_byte(string)){
		if(!ks0108_putc(font,pgm_read_byte(string++),1,inverted)){
			break;
		}
	}
}


// ---------------------------------------
// get String length in Pixel
// ---------------------------------------
uint16_t ks0108_string_width(const uint8_t* font,char* string){
	uint16_t width = 0;
	while(*string){
		width += ks0108_font_char_width(font,*string++) + 1;		
	}
	if(width){
		width--;
	}
	return width;
}


// ---------------------------------------
// get String length in Pixel(from flash)
// ---------------------------------------
uint16_t ks0108_string_width_p(const uint8_t* font,char* PROGMEM string){
	uint16_t width = 0;
	while(pgm_read_byte(string)){
		width += ks0108_font_char_width(font,pgm_read_byte(string++)) + 1;		
	}
	if(width){
		width--;
	}
	return width;
}


#endif //no-text


// ###############################################################################################
// # 																							 #
// # Graphics																					 #
// #																							 #
// ###############################################################################################
#ifndef NO_GRAPHICS


// ---------------------------------
// draw line on screen
// ---------------------------------
void ks0108_draw_line(uint8_t x1,uint8_t y1,uint8_t x2,uint8_t y2,uint8_t color)
{
	uint8_t cnt;

	#ifndef NO_BRESENHAM
	uint8_t dy,dx,x,y;
	int8_t	addx, addy;
	int16_t err;
	#endif

	if (x1==x2 || y1==y2)
	{
		if (x1==x2)
		{
			for (cnt=min(y1,y2);cnt<=max(y1,y2);cnt++)			
				ks0108_set_pixel(x1,cnt,color);			
		}
		else
		{
			for (cnt=min(x1,x2);cnt<=max(x1,x2);cnt++)			
				ks0108_set_pixel(cnt,y1,color);			
		}
	}
	#ifndef NO_BRESENHAM	// see header
	else
	{
   		dx = max(x1,x2)-min(x1,x2);
   		dy = max(y1,y2)-min(y1,y2);

   		x = x1;
   		y = y1;

	   	if(x1 > x2)
	      addx = -1;
	   	else
	      addx = 1;

	   	if(y1 > y2)
	      addy = -1;
	   	else
	      addy = 1;

	   	if(dx >= dy)
	   	{
	      err = 2*dy - dx;

	      for(cnt=0; cnt<=dx; ++cnt)
	      {
	         ks0108_set_pixel(x, y, color);

	         if(err < 0)
	         {
	            err += 2*dy;
	            x += addx;
	         }
	         else
	         {
	            err += 2*dy - 2*dx;
	            x += addx;
	            y += addy;
	         }
	      }
	   	}
	   	else
	   	{
	      err = 2*dx - dy;

	      for(cnt=0; cnt<=dy; ++cnt)
	      {
	         ks0108_set_pixel(x, y, color);

	         if(err < 0)
	         {
	            err += 2*dx;
	            y += addy;
	         }
	         else
	         {
	            err += 2*dx - 2*dy;
	            x += addx;
	            y += addy;
	         }
	      }
	  	}	
	}
	#endif //no_bresenham
}


// ---------------------------------
// draw rectangle on screen
// ---------------------------------
void ks0108_draw_rect(uint8_t x,uint8_t y,uint8_t width ,uint8_t height,uint8_t color)
{
	ks0108_draw_line(x,y,x+width-1,y,color);					// Top Line
	ks0108_draw_line(x+width-1,y,x+width-1,y+height-1,color); 	// Left Line
	ks0108_draw_line(x,y+height-1,x+width-1,y+height-1,color);	// Bottom Line
	ks0108_draw_line(x,y,x,y+height-1,color);					// Right Line
}


// ------------------------------------
// fills an rectangle with given color
// ------------------------------------
void ks0108_fill_rect(uint8_t x,uint8_t y,uint8_t width,uint8_t height,uint8_t color)
{
	uint8_t cnt_x,cnt_y;
		for (cnt_y=y;cnt_y<y+height;cnt_y++)				// fill screen using set_pixel (slow) but needs less memory
			for (cnt_x=x;cnt_x<x+width;cnt_x++)
				ks0108_set_pixel(cnt_x,cnt_y,color);

}


// ---------------------------------
// draw round rectangle on screen
// ---------------------------------
void ks0108_draw_round_rect(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t radius, uint8_t color) {
    int16_t tSwitch, x1 = 0, y1 = radius;
    tSwitch = 3 - 2 * radius;

  while (x1 <= y1) {
    ks0108_set_pixel(x+radius - x1, y+radius - y1, color);
    ks0108_set_pixel(x+radius - y1, y+radius - x1, color);

    ks0108_set_pixel(x+width-radius + x1, y+radius - y1, color);
    ks0108_set_pixel(x+width-radius + y1, y+radius - x1, color);

    ks0108_set_pixel(x+width-radius + x1, y+height-radius + y1, color);
    ks0108_set_pixel(x+width-radius + y1, y+height-radius + x1, color);

    ks0108_set_pixel(x+radius - x1, y+height-radius + y1, color);
    ks0108_set_pixel(x+radius - y1, y+height-radius + x1, color);

      if (tSwitch < 0) {
        tSwitch += (4 * x1 + 6);
      } else {
        tSwitch += (4 * (x1 - y1) + 10);
        y1--;
      }
      x1++;
  }

  ks0108_draw_line(x+radius, y, width-radius,y, color);      // top
  ks0108_draw_line(x+radius, y+height, width-radius,y+height,color);  // bottom
  ks0108_draw_line(x, y+radius, x,height-radius,color);      // left
  ks0108_draw_line(x+width, y+radius, x+width,height-radius,color);  // right
}
#endif //no-graphics

