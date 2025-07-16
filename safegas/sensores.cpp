#include <Arduino.h>
#include <stdbool.h>
#include "sensores.h"

bool movimentoAnterior = false;
bool fogoAnterior = false;
bool alertState = false;
bool valvulaVal = false;

int odeNotes[] = {
    262, 262, 294, 330, 330, 294, 262, 247, 
    220, 220, 247, 262, 262, 247, 247 
};

int odeDurations[] = {
    400, 400, 400, 400, 400, 400, 400, 400, 
    400, 400, 400, 400, 600, 200, 600
};

void setValvula(bool state)
{
    digitalWrite(valvula, state);
    valvulaVal = state;
}

bool flameDetected()
{
    return (analogRead(sensor_fogo) < 600) ? true : false;
}

bool gasDetected()
{
    return (analogRead(sensor_gas) > 200) ? true : false;
}

bool movementDetected()
{
    return digitalRead(pinoPIR);
}

void setup_sensores ()
{
    pinMode(pinoPIR, INPUT);
    pinMode(sensor_fogo, INPUT);
    pinMode(sensor_gas, INPUT);
    pinMode(LED, OUTPUT);
    pinMode(buzzer, OUTPUT);
    pinMode(valvula, OUTPUT);
    pinMode(BOTAO, INPUT);

    setValvula(false);

    // Tempo para o PIR e pro sensor de gás se calibrarem
    Serial.println("Aguardando calibração do PIR e do sensor de gás (30s)...");
    delay(30000);
}

void playSong(unsigned long durationMillis) {
    unsigned long startTime = millis();
    int i = 0;
    int numNotes = sizeof(odeNotes) / sizeof(odeNotes[0]);

    while (millis() - startTime < durationMillis) {
	int freq = odeNotes[i];
	int noteDuration = odeDurations[i];

	tone(buzzer, freq);
	digitalWrite(LED, HIGH);
	delay(noteDuration / 2);   // Nota mais curta para dar pausa

	noTone(buzzer);
	digitalWrite(LED, LOW);
	delay(noteDuration / 2);   // Pausa entre notas

	i = (i + 1) % numNotes;
    }

    noTone(buzzer);
    digitalWrite(LED, LOW);
}
