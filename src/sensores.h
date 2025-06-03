#define sensor_fogo A0
#define sensor_gas A1
#define LED 7;
#define buzzer 5;

int pinoPIR = 3;
bool movimentoAnterior = false;

void setup_sensores();

bool flameDetected();
bool gasDetected();
bool movementDetected();
