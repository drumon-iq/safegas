#ifndef SENSORES_H

#define sensor_fogo 13
#define pinoPIR     18
#define LED	    5
#define buzzer 	    4
#define sensor_gas  15
#define valvula     14
#define BOTAO 	    26
#define SINGLE_LED  27

extern bool fogoAnterior;
extern bool valvulaVal;

void setup_sensores();
bool flameDetected();
bool gasDetected();
bool movementDetected();
void playSong(unsigned long durationMillis);
void setValvula(bool state);

#endif
