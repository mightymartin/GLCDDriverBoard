#include <Arduino.h>
#include "glcd.h"
#include "rotary_enc.h"
#include "serial_cmd.h"

void setup(void) {
    GLCDInit();
    SerCmdInit();
    RotEncInit();     
}

void loop(void) {
    SerCmdTick();
    GLCDTick();
}

