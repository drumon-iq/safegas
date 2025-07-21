// Código com botões e buzzer no controle (dispositivo móvel)

#include <WiFi.h>         // Biblioteca para funcionalidades de WiFi
#include <esp_now.h>      // Biblioteca para comunicação ESP-NOW

// Endereço MAC do dispositivo ESP2 (destinatário dos comandos)
uint8_t endereco_mac_esp2[] = {0xDC, 0x06, 0x75, 0x67, 0xB4, 0x14};  // MAC da ESP2

// Definição dos pinos dos botões
const int botaoLiga = 4;
const int botaoVolMais = 3;
const int botaoVolMenos = 2;
const int botaoCHMais = 1;
const int botaoCHMenos = 0;

// Pino do buzzer
const int pinoBuzzer = 5;

int EA = 0;               // Estado atual do buzzer (0 = desligado, 1 = ligado)
String mensagem = "";     // Variável para armazenar mensagens recebidas

// Callback chamado quando uma mensagem é recebida via ESP-NOW
void OnDataRecv(const esp_now_recv_info_t *info, const uint8_t *incomingData, int len) {
  for (int i = 0; i < len; i++) {
    mensagem += (char)incomingData[i];  // Converte os dados recebidos para uma string
  }

  Serial.print("Mensagem recebida de volta: ");
  Serial.println(mensagem);  // Imprime a mensagem recebida no monitor serial
}

void setup() {
  Serial.begin(115200);  // Inicializa a comunicação serial

  // Configura os pinos dos botões como entradas com pull-up interno
  pinMode(botaoLiga, INPUT_PULLUP);
  pinMode(botaoVolMais, INPUT_PULLUP);
  pinMode(botaoVolMenos, INPUT_PULLUP);
  pinMode(botaoCHMais, INPUT_PULLUP);
  pinMode(botaoCHMenos, INPUT_PULLUP);

  // Configura o pino do buzzer como saída e o desliga inicialmente
  pinMode(pinoBuzzer, OUTPUT);
  digitalWrite(pinoBuzzer, LOW);

  // Configura o modo WiFi como estação (STA)
  WiFi.mode(WIFI_STA);

  // Inicializa o protocolo ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Erro ao iniciar ESP-NOW");
    return;
  }

  // Registra a função de callback para recepção de dados
  esp_now_register_recv_cb(OnDataRecv);  // Recebe de volta o comando "BZ"

  // Adiciona o peer (dispositivo ESP2) para envio de mensagens
  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, endereco_mac_esp2, 6);
  peerInfo.channel = 0;       // Canal padrão (pode ser ajustado)
  peerInfo.encrypt = false;   // Comunicação sem criptografia
  esp_now_add_peer(&peerInfo);

  Serial.println("ESP1 pronta!");
}

void loop() {
  // Verifica se o botão "Liga/Desliga" foi pressionado
  if (digitalRead(botaoLiga) == LOW) {
    const char* msg = "LD";
    esp_now_send(endereco_mac_esp2, (uint8_t*)msg, strlen(msg));  // Envia comando
    Serial.println("LD enviado");
    delay(500);  // Pequeno delay para evitar múltiplos envios
  }

  // Verifica se o botão "Volume +" foi pressionado
  if (digitalRead(botaoVolMais) == LOW) {
    const char* msg = "V+";
    esp_now_send(endereco_mac_esp2, (uint8_t*)msg, strlen(msg));
    Serial.println("V+ enviado");
    delay(500);
  }

  // Verifica se o botão "Volume -" foi pressionado
  if (digitalRead(botaoVolMenos) == LOW) {
    const char* msg = "V-";
    esp_now_send(endereco_mac_esp2, (uint8_t*)msg, strlen(msg));
    Serial.println("V- enviado");
    delay(500);
  }

  // Verifica se o botão "Canal +" foi pressionado
  if (digitalRead(botaoCHMais) == LOW) {
    const char* msg = "C+";
    esp_now_send(endereco_mac_esp2, (uint8_t*)msg, strlen(msg));
    Serial.println("C+ enviado");
    delay(500);
  }

  // Verifica se o botão "Canal -" foi pressionado
  if (digitalRead(botaoCHMenos) == LOW) {
    const char* msg = "C-";
    esp_now_send(endereco_mac_esp2, (uint8_t*)msg, strlen(msg));
    Serial.println("C- enviado");
    delay(500);
  }

  // Verifica se recebeu o comando "BZ" para ativar ou desativar o buzzer
  if (mensagem == "BZ" && EA == 0) {
    digitalWrite(pinoBuzzer, HIGH);  // Liga o buzzer
    EA = 1;                          // Atualiza estado para ligado
    mensagem = "";                   // Limpa a mensagem
  }
  if (mensagem == "BZ" && EA == 1) {
    digitalWrite(pinoBuzzer, LOW);   // Desliga o buzzer
    EA = 0;                           // Atualiza estado para desligado
    mensagem = "";                    // Limpa a mensagem
  }
}
