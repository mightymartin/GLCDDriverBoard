#ifndef SERIAL_CMD_H
#define SERIAL_CMD_H

#include <Arduino.h>
#include "glcd.h"
#include "rotary_enc.h"

#define BUF_LEN 10 + GLCD_COLS + 1 //prefix + max cols + terminator   

extern void SerCmdInit();
extern void SerCmdTick();

#endif