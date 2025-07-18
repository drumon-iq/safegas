#ifndef SENSORES_H

#define sensor_fogo 26
#define sensor_gas 2
#define LED 33
#define buzzer 32
#define pinoPIR 27
#define valvula 00
#define BOTAO 34

extern bool fogoAnterior;
extern bool valvulaVal;

void setup_sensores();
bool flameDetected();
bool gasDetected();
bool movementDetected();
void playSong(unsigned long durationMillis);
void setValvula(bool state);

#endif
