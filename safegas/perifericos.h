#ifndef SENSORES_H

#define SENSOR_FOGO 13
#define PINOPIR     18
#define LED	    5
#define BUZZER 	    4
#define SENSOR_GAS  15
#define VALVULA     14
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
