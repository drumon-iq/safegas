// ================== DEFINIÇÕES DE PINOS ==================
#define sensor_fogo 26
#define LED 33
#define buzzer 32
int pinoPIR = 27; // Corrigido para evitar conflito com sensor de gás

// ================== VARIÁVEIS GERAIS ==================
bool movimentoAnterior = false;
bool fogoAnterior = false;  // Novo para detectar mudança no fogo
bool ledState = false;
bool buzzerState = false;

// Notas da melodia (C4 a C5)
int odeNotes[] = { 
  262, 262, 294, 330, 330, 294, 262, 247, 
  220, 220, 247, 262, 262, 247, 247 
};

int odeDurations[] = {
  400, 400, 400, 400, 400, 400, 400, 400, 
  400, 400, 400, 400, 600, 200, 600
};


// Variáveis para contagem do tempo
unsigned long startTime = 0;
const unsigned long duration = 10 * 1000; // 5 minutos em milissegundos
bool counting = false;
unsigned long elapsed = 0;

// ================== SETUP PRINCIPAL ==================
void setup() {
  Serial.begin(115200);
  setup_sensores(); // Inicializa sensores e toca música de calibração
}

// ================== LOOP PRINCIPAL ==================
void loop() {
  static bool ledOn = false;     // Tornar estático para manter estado entre loops
  static bool buzzerOn = false;  // Igual para buzzer

  // Lê os estados uma vez
  bool movimentoAtual = (digitalRead(pinoPIR) == HIGH);
  bool fogoAtual = (digitalRead(sensor_fogo) == HIGH);

  // Detecta mudança no movimento e printa
  if (movimentoAtual != movimentoAnterior) {
    movimentoAnterior = movimentoAtual;
    if (movimentoAtual) {
      Serial.println("Movimento detectado!");
    } else {
      Serial.println("Movimento cessou.");
    }
  }

  // Detecta mudança no fogo e printa
  if (fogoAtual != fogoAnterior) {
    fogoAnterior = fogoAtual;
    if (fogoAtual) {
      Serial.println("Fogo detectado!");
    } else {
      Serial.println("Fogo cessou.");
    }
  }

  // Início da contagem se não tem movimento, tem fogo, e não está contando
  if (!movimentoAtual && fogoAtual && !counting) {
    counting = true;
    startTime = millis();
    Serial.println("Iniciando contagem de 5 minutos...");
  }

  // Resetar contagem se detectar movimento e está contando
  if (movimentoAtual && counting) {
    counting = false;
    Serial.println("Contagem de 5 minutos resetada.");
  }

  // Se está contando...
  if (counting) {
    elapsed = millis() - startTime;

    // Após 5 minutos, ativar alerta (LED e buzzer piscando)
    if (elapsed >= duration) {
      ledState = !ledState;

      digitalWrite(LED, ledState ? HIGH : LOW);
      if(ledState) {
        tone(buzzer, 4000); // 1000 Hz, pode ajustar a frequência
      } else {
        noTone(buzzer);
      }

      // Após 10 minutos, desligar alerta
      if (elapsed >= 2 * duration) {
        Serial.println("10 minutos passaram. Desligando sistema por segurança.");
        digitalWrite(LED, LOW);
        digitalWrite(buzzer, LOW);
        counting = false;
      }
    }
  } else {
    // Garantir LED e buzzer desligados quando não contando
    digitalWrite(LED, LOW);
    digitalWrite(buzzer, LOW);
    ledOn = false;
    buzzerOn = false;
  }

  delay(200); // Reduz a frequência de checagem
}

// ================== INICIALIZAÇÃO DE SENSORES ==================
void setup_sensores() {
  pinMode(pinoPIR, INPUT);
  pinMode(sensor_fogo, INPUT);
  pinMode(LED, OUTPUT);
  pinMode(buzzer, OUTPUT);

  Serial.println("Aguardando calibração do PIR (30s)...");
  playSong(5000); // Música e LED durante calibração
}

// ================== FUNÇÃO PARA TOCAR MÚSICA ==================
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
