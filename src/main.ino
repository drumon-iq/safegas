//Variaveis para contagem do tempo
unsigned long startTime=0;
const unsigned long duration = 5 * 60 * 1000; // 5 minutos em milissegundos
bool counting = false;
unsigned long elapsed = 0;

void setup() {
  Serial.begin(115200);
  //startTime = millis(); //Inicia a contagem em milisec ao iniciar o programa

  setup_sensores();
}

void loop() {

  //Ler cada sensor
  //Se o sensor de movimento==0 e sensor de gás ou sensor de chama == 1
  //Contar 5 mins e mandar aviso
  //Se houver movimento parar a contagem
  //Caso não pare a contagem até 5 mins mandar aviso
  //Caso não pare a contagem em até 10 mins desligar tudo

  bool ledOn=false;
  bool buzzerOn=false;

  if(!movementDetected() && (gasDetected() || flameDetected()) && !counting){
    counting=true;
    startTime=millis();
    Serial.println("Iniciando contagem de 5 minutos...");
  }
  
  if(movementDetected() && counting){
    counting = false;
    Serial.println("Contagem de 5 minutos resetada.");
  }

  if(counting){
    elapsed = millis() - startTime;
    if(elapsed >= duration){
      //Passaram 5 mins
      //Avisar o usuario

      if(!ledOn){
        digitalWrite(LED, HIGH);
        ledOn=true;
      }else{
        digitalWrite(LED, LOW);
        ledOn=false;
      }

      if(!buzzerOn){
        digitalWrite(buzzer, HIGH);
        buzzerOn=true;
      }else{
        digitalWrite(buzzer, LOW);
        buzzerOn=false;
      }


      if(elapsed >= 2*duration){
        //Fechar a valvula de gás e avisar o usuario
        digitalWrite(relePin, LOW); //Liga o relé
      }
    }
  }

  delay(200); //Delay pra evitar sobrecarga porém o programa num todo para
}
