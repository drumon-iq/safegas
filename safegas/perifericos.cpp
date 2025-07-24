#include <Arduino.h>
#include <stdbool.h>
#include "perifericos.h"
#include "util.h"

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
    if (!doDummy)
	digitalWrite(VALVULA, state);

    valvulaVal = state;
    digitalWrite(SINGLE_LED, state);
    tone(BUZZER, 1000, 300);
    Serial.print("ESTADO da valvula alterado para: ");
    Serial.println(state);
}

bool flameDetected()
{
    return (analogRead(SENSOR_FOGO) < 600) ? true : false;
}

bool gasDetected()
{
    return (analogRead(SENSOR_GAS) > 200) ? true : false;
}

bool movementDetected()
{
    return digitalRead(PINOPIR);
}

void setup_sensores ()
{
    pinMode(PINOPIR, INPUT);
    pinMode(SENSOR_FOGO, INPUT);
    pinMode(SENSOR_GAS, INPUT);
    pinMode(LED, OUTPUT);
    pinMode(BUZZER, OUTPUT);
    pinMode(VALVULA, OUTPUT);
    pinMode(BOTAO, INPUT_PULLUP);
    pinMode(SINGLE_LED, OUTPUT);

    if (!doDummy)
	delay(30000); // Tempo para o PIR e pro sensor de gás se calibrarem (?? porque ??)

    setValvula(false);
}

void playSong(unsigned long durationMillis) {
    unsigned long startTime = millis();
    int i = 0;
    int numNotes = sizeof(odeNotes) / sizeof(odeNotes[0]);

    while (millis() - startTime < durationMillis) {
	int freq = odeNotes[i];
	int noteDuration = odeDurations[i];

	tone(BUZZER, freq);
	digitalWrite(LED, HIGH);
	delay(noteDuration / 2);   // Nota mais curta para dar pausa

	noTone(BUZZER);
	digitalWrite(LED, LOW);
	delay(noteDuration / 2);   // Pausa entre notas

	i = (i + 1) % numNotes;
    }

    noTone(BUZZER);
    digitalWrite(LED, LOW);
}
