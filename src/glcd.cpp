#include "glcd.h"

static uint8_t needsUpdate = 0;

void GLCDInit(){
    ks0108_init(GLCD_PIN_D0, GLCD_PIN_D1, GLCD_PIN_D2 ,GLCD_PIN_D3, GLCD_PIN_D4, GLCD_PIN_D5, GLCD_PIN_D6, GLCD_PIN_D7, GLCD_PIN_EN, GLCD_PIN_DC, GLCD_PIN_CS1, GLCD_PIN_CS2, GLCD_PIN_RST, GLCD_PIN_RW);
    ks0108_clear();
    ks0108_update();
    ks0108_draw_fullscreen_bmp(loading_img);
    ks0108_update();
}

void GLCDdrawCharAt(uint8_t x_pos, uint8_t y_pos, uint8_t c, uint8_t inverted, uint8_t underline, uint8_t spacer){	
	uint8_t x_pixel = x_pos * (GLCD_FONT_X+1) + 1;	//ein pixel abstand zwischen den buchstaben
	uint8_t y_pixel = y_pos * (GLCD_FONT_Y+2) + 1;    //zwei pixel abstand zwischen den zeilen
	
	if(x_pos < GLCD_COLS && y_pos < GLCD_ROWS){
		if(spacer){
			ks0108_draw_line(x_pixel,y_pixel - 1, x_pixel + GLCD_FONT_X, y_pixel - 1, ((inverted) ? BLACK : WHITE)); //um die spacerline über der zeile zu füllen
		}else{
			ks0108_draw_line(x_pixel,y_pixel - 1, x_pixel + GLCD_FONT_X - 1, y_pixel - 1, ((inverted) ? BLACK : WHITE)); //um die spacerline über der zeile zu füllen
		}
		ks0108_set_cursor(x_pixel, y_pixel);
		ks0108_putc(small_font,c,spacer,inverted);
		if(underline){
			if(spacer){
				ks0108_draw_line(x_pixel,y_pixel + GLCD_FONT_Y,x_pixel + GLCD_FONT_X,y_pixel + GLCD_FONT_Y, ((inverted) ? WHITE : BLACK));
			}else{
				ks0108_draw_line(x_pixel,y_pixel + GLCD_FONT_Y,x_pixel + GLCD_FONT_X - 1,y_pixel + GLCD_FONT_Y, ((inverted) ? WHITE : BLACK));
			}
		}
	}
}


void GLCDdrawStringAt(uint8_t x_pos, uint8_t y_pos, char* string, uint8_t inverted, uint8_t underline){	
	while(*string){
		GLCDdrawCharAt(x_pos++,y_pos,*string++, inverted, underline, 1);	
	}	
}

void GLCDInvert(){
    ks0108_invert();
}

void GLCDClear(){
    ks0108_clear();
}

void GLCDSetDirty(){
    needsUpdate = 1;
}

void GLCDTick(){
    if(needsUpdate == 1){
        ks0108_update();
        needsUpdate = 0;
    }
}