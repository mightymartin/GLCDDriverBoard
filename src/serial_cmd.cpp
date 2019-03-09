#include "serial_cmd.h"

static uint8_t handshake   = 0;	//default 0 da erst nach box Halo auf 1
static char    cmdBuffer[BUF_LEN] = {0};  
static uint8_t cmdComplete     = 0;    	
static uint8_t cmdBufferWPos    = 0;
static uint8_t cmdBufferRPos    = 0;


void SerCmdInit(){
    Serial.begin(9600);
}

char readBuffer(){
    return cmdBuffer[cmdBufferRPos++];
}

uint16_t toNumber(uint8_t maxStellen){
	uint16_t theRet = 0;
	uint8_t stelle = 0;
	uint16_t buffer[5];
	uint16_t tensPow[5] = {1,10,100,1000,10000};
	
	uint8_t c = 0;
	while(stelle < maxStellen){ 
		c = readBuffer();		
		if(c >= '0' && c <= '9'){
			buffer[stelle] = c -48;
			stelle++;
		}else{
			break;
		}				
	}
	
	uint8_t pos = 0;
	while(stelle > 0){
		theRet += buffer[pos] * tensPow[stelle-1];		
		stelle--;
		pos++;
	}
	return theRet;
}

void SerCmdSendCommands(void){    
    int16_t rotenc_r = RotEncRotation();
    if(rotenc_r != 0){
		if(rotenc_r > 0){
			Serial.write('L');
		}else{
			Serial.write('R');
		}			
	}

    uint8_t rotenc_b = RotEncButtonState();
    if(rotenc_b == ClickEncoder::Clicked){
        Serial.write('C');
    }
    if(rotenc_b == ClickEncoder::DoubleClicked){
        Serial.write('D');
    }
    if(rotenc_b == ClickEncoder::Released){
        Serial.write('P');
    }
}

void SerCmdGetCommands(void){
	uint8_t retValue        = 0;	
		
    uint8_t inverted        = 0;
	uint8_t underline       = 0;

    uint8_t xpos            = 0;
    uint8_t ypos            = 0;

    char displayBuffer[GLCD_COLS + 1] = {0}; 

    while(Serial.available() && cmdComplete == 0){
        cmdBuffer[cmdBufferWPos] = Serial.read();
        if(cmdBuffer[cmdBufferWPos] == '\n'){
            cmdComplete = 1;
            cmdBufferRPos = 0;
        }
        cmdBufferWPos++;
    }

    if(cmdComplete){        
        if(readBuffer() == 'B' && readBuffer() == 'G' && readBuffer() == 'N'){
            switch(readBuffer()){
					case 'H':
						//##########HANDSHAKE Request
						handshake = 1;						
						retValue = 1;
                        ks0108_clear();
						break;													
					case 'T':
						//##########SET TEXT AT X Y
						if(!handshake){
                            retValue = 2;
                            break; //-> no HANDSHAKE, no Service
                        } 
						
						if(readBuffer() == '1'){
							inverted = 1;							
						}

						if(readBuffer() == '1'){
							underline = 1;							
						}

						xpos = toNumber(2);
						if(xpos >= GLCD_COLS){
							retValue = 2;
							break;
						}

						ypos = toNumber(2);
						if(ypos >= GLCD_ROWS){
							retValue = 2;
							break;
						}

												
                        for(int i = 0; i <= cmdBufferWPos - cmdBufferRPos; i++){
                            if(cmdBuffer[i+cmdBufferRPos] != '\n' && i < GLCD_COLS){
                                displayBuffer[i] = cmdBuffer[i+cmdBufferRPos];
                            }else{
                                displayBuffer[i] = '\0';
                                break; 
                            }
                        }						
						GLCDdrawStringAt(xpos, ypos, displayBuffer, inverted, underline);
						retValue = 1;																			
						break;	
                    case 'B':
						//##########SET BACKLIGHT ON / OFF	
                        if(!handshake){
                            retValue = 2;
                            break; //-> no HANDSHAKE, no Service
                        } 
                        					
						if(readBuffer() == '1'){
							GLCDBacklight(1);														
							retValue = 1;
						}else{
							GLCDBacklight(0);														
							retValue = 1;
						}							
						break;	
                    case 'W':
						//##########show wait/Loading Screen
						if(!handshake){
                            retValue = 2;
                            break; //-> no HANDSHAKE, no Service
                        } 
						ks0108_clear();
                        ks0108_draw_fullscreen_bmp(loading_img);
						retValue = 1;
						break;   

					case 'I':
						//##########INVERT SCREEN
						if(!handshake){
                            retValue = 2;
                            break; //-> no HANDSHAKE, no Service
                        } 
						GLCDInvert();
						retValue = 1;
						break;
					case 'C':
						//########## CLEAR DISPLAY						
						if(!handshake){
                            retValue = 2;
                            break; //-> no HANDSHAKE, no Service
                        } 
						GLCDClear();
						retValue = 1;
						break;							
					default:
						break;
				} 
        }

        //Important
        cmdComplete = 0;
        cmdBufferWPos = 0;
        cmdBufferRPos = 0;
    }

	if(retValue == 1){
		Serial.write('A');
		GLCDSetDirty();
	}else if(retValue == 2){
		Serial.write('N');
	}
}


void SerCmdTick(){
    SerCmdGetCommands();
    SerCmdSendCommands();
}

