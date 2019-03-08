#ifndef KS0108_H
#define KS0108_H

#include <Arduino.h>

//######################################
//## CONSTS Definition
//######################################
#define SCREEN_H			64	
#define SCREEN_W			128	
#define SCREEN_BYTES        SCREEN_W * SCREEN_H / 8  //1024


#define CHIP1				0x01
#define CHIP2				0x02
#define LCD_ON				0x3F
#define LCD_OFF				0x3E
#define LCD_SET_ADD			0x40
#define LCD_SET_PAGE		0xB8
#define LCD_DISP_START		0xC0
#define LCD_BUSY			0x80
#define BLACK				0x01
#define WHITE				0x00

//######################################
//## Prototypes
//######################################
extern void     ks0108_init(uint8_t D0, uint8_t D1, uint8_t D2, uint8_t D3, uint8_t D4, uint8_t D5, uint8_t D6, uint8_t D7, uint8_t EN, uint8_t DC, uint8_t CS1, uint8_t CS2, uint8_t RST, uint8_t RW);
extern void     ks0108_update();
extern void     ks0108_clear();
extern void     ks0108_set_pixel(int16_t,uint8_t,uint8_t);
extern void     ks0108_set_8pixel(int16_t index, uint8_t value);
extern void     ks0108_invert();
extern void     ks0108_draw_fullscreen_bmp(const uint8_t*);

extern void     ks0108_chip_select(uint8_t);
extern void     ks0108_write_cmd(uint8_t,uint8_t);
extern void     ks0108_write_data(uint8_t,uint8_t);
extern void     ks0108_write(uint8_t);

#ifndef NO_TEXT
    extern uint8_t  ks0108_font_number_of_chars(const uint8_t*);
    extern uint8_t  ks0108_font_first_char(const uint8_t*);
    extern uint8_t  ks0108_font_char_width(const uint8_t*,uint8_t);
    extern uint8_t  ks0108_font_char_height();
    extern uint16_t ks0108_font_start_offset(const uint8_t*,uint8_t);
    extern uint8_t  ks0108_font_char_present(const uint8_t*,uint8_t);	
	extern void     ks0108_set_cursor(const int16_t, uint8_t);	
    extern uint8_t  ks0108_putc(const uint8_t*,uint8_t,uint8_t,uint8_t inverted );
    extern void     ks0108_puts(const uint8_t*, char*, uint8_t inverted  );
    extern void     ks0108_puts_p(const uint8_t*, char* PROGMEM, uint8_t inverted  );
    extern uint16_t ks0108_string_width(const uint8_t* ,char* );
    extern uint16_t ks0108_string_width_p(const uint8_t* ,char* PROGMEM );
#endif

#ifndef NO_GRAPHICS
	extern void ks0108_draw_line(uint8_t,uint8_t,uint8_t,uint8_t,uint8_t);
	extern void ks0108_draw_rect(uint8_t,uint8_t,uint8_t,uint8_t,uint8_t);
	extern void ks0108_fill_rect(uint8_t,uint8_t,uint8_t,uint8_t,uint8_t);
	extern void ks0108_draw_round_rect(uint8_t,uint8_t,uint8_t,uint8_t,uint8_t,uint8_t);
#endif

#endif