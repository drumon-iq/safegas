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
  bool movimentoAtual = (digitalRead(pinoPIR) == HIGH);
  bool movimentoDetectado=false;


  if (movimentoAtual != movimentoAnterior) {
    if (movimentoAtual) {
      movimentoDetectado=true;
    } else {
      movimentoDetectado=false;
    }

    movimentoAnterior = movimentoAtual;
  }

  return movimentoDetectado;
}

void setup_sensores ()
{
  pinMode(pinoPIR, INPUT);
  pinMode(sensor_fogo, INPUT);
  pinMode(sensor_gas, INPUT);

  pinMode(LED, OUTPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(relePin, OUTPUT);
  digitalWrite(relePin, HIGH); //Inicia o relé desligado
  
  // Tempo para o PIR e pro sensor de gás se calibrarem
  Serial.println("Aguardando calibração do PIR e do sensor de gás (30s)...");
  delay(30000);
}
