#ifndef ROTENC_H
#define ROTENC_H

#include <Arduino.h>
#include <ClickEncoder.h>
#include <TimerOne.h>

#define ROTARY_PINA 2
#define ROTARY_PINB 3
#define ROTARY_PINSW 18 //switch
#define ROTARY_STEPS 4

static ClickEncoder encoder(ROTARY_PINA, ROTARY_PINB, ROTARY_PINSW, ROTARY_STEPS);

extern void RotEncInit();
extern uint8_t RotEncButtonState();
extern int16_t RotEncRotation();

#endif