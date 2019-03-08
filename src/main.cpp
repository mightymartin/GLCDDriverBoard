#include <Arduino.h>
#include "glcd.h"
#include "rotary_enc.h"
#include "serial_cmd.h"

void setup(void) {
    SerCmdInit();
    delay(2);
    RotEncInit(); 
    GLCDInit();
}

void loop(void) {
    SerCmdTick();
    GLCDTick();
}

