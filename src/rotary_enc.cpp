#include "rotary_enc.h"

void timerTick() {
  encoder.service();
}

void RotEncInit(){
  Timer1.initialize(1000);
  Timer1.attachInterrupt(timerTick);

  encoder.setAccelerationEnabled(true);
}

uint8_t RotEncButtonState(){
    return encoder.getButton();
}

int16_t RotEncRotation(){
    return encoder.getValue();
}

