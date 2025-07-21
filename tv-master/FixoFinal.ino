#include <WiFi.h>             // Biblioteca para funções Wi-Fi
#include <esp_now.h>          // Biblioteca para comunicação via ESP-NOW
#include <IRremote.hpp>       // Biblioteca para recepção e envio de sinais infravermelhos
#include <Preferences.h>      // Biblioteca para armazenamento em memória flash (NVS)
#include <FastLED.h>          // Biblioteca para controle de LEDs endereçáveis
#include "leds.hpp"           // Arquivo externo para controle de LEDs visuais

// === CONFIGURAÇÕES ===
#define PIN_RECEPTOR_IR 3         // Pino do receptor infravermelho
#define PIN_BOTAO_GRAVACAO 9      // Pino do botão que ativa o modo de gravação IR
#define PIN_ENCONTRA 0            // Pino do botão para enviar o comando "BZ"
#define NUM_LEDS_IR 6             // Quantidade de LEDs emissores IR

// Pinos conectados aos emissores IR
const uint8_t pinosLEDIR[NUM_LEDS_IR] = {1, 2, 4, 5, 6, 7};
IRsend* emissoresIR[NUM_LEDS_IR]; // Vetor de ponteiros para emissores IR

Preferences preferences; // Objeto para armazenamento persistente (NVS)

// === ESTRUTURA DOS COMANDOS IR ===
struct ComandoIR {
  uint16_t address;       // Endereço do dispositivo IR
  uint8_t command;        // Comando IR (ex: aumentar volume)
  uint32_t protocol;      // Protocolo usado (NEC, SONY, etc.)
  uint32_t delayDepois;   // Tempo de espera antes do próximo comando
};

std::vector<ComandoIR> comandos;  // Lista dinâmica de comandos IR
bool modoGravacao = false;        // Indica se está em modo de gravação IR
int indexGravacao = 0;            // Índice de gravação (não usado diretamente aqui)
unsigned long ultimoTempoGravacao = 0; // Marca o tempo do último comando para calcular delays

// Endereço MAC da ESP1 (controle)
uint8_t endereco_mac_esp1[] = {0xDC, 0x06, 0x75, 0x67, 0x8E, 0xB8};

// === FUNÇÕES DE ARMAZENAMENTO EM FLASH ===
void salvarTodosComandos() {
  preferences.begin("comandos", false); // Acessa o namespace "comandos"
  preferences.putUInt("quantidade", comandos.size()); // Salva a quantidade total

  // Armazena cada campo do vetor de comandos
  for (size_t i = 0; i < comandos.size(); i++) {
    preferences.putUShort(("addr" + String(i)).c_str(), comandos[i].address);
    preferences.putUChar(("cmd" + String(i)).c_str(), comandos[i].command);
    preferences.putUInt(("prot" + String(i)).c_str(), comandos[i].protocol);
    preferences.putUInt(("dly" + String(i)).c_str(), comandos[i].delayDepois);
  }

  preferences.end(); // Finaliza acesso ao NVS
}

void carregarTodosComandos() {
  preferences.begin("comandos", false);
  int qtd = preferences.getUInt("quantidade", 0); // Lê quantos comandos foram salvos
  comandos.clear(); // Limpa a lista atual

  // Recarrega os dados salvos
  for (int i = 0; i < qtd; i++) {
    ComandoIR cmd;
    cmd.address = preferences.getUShort(("addr" + String(i)).c_str(), 0);
    cmd.command = preferences.getUChar(("cmd" + String(i)).c_str(), 0);
    cmd.protocol = preferences.getUInt(("prot" + String(i)).c_str(), (uint32_t)UNKNOWN);
    cmd.delayDepois = preferences.getUInt(("dly" + String(i)).c_str(), 1000);

    if (cmd.protocol != UNKNOWN) {
      comandos.push_back(cmd);
      Serial.printf("Comando %d carregado: Prot %s, Addr %X, Cmd %X\n", i,
                    getProtocolString((decode_type_t)cmd.protocol),
                    cmd.address, cmd.command);
    } else {
      Serial.printf("Comando %d inválido.\n", i);
    }
  }

  preferences.end();
}

// Envia um comando IR utilizando todos os emissores conectados
void enviarIR(ComandoIR cmd) {
  if (cmd.protocol == UNKNOWN) {
    Serial.println("Comando vazio ou inválido.");
    return;
  }

  decode_type_t protocolo = (decode_type_t)cmd.protocol;

  // Envia o comando para cada emissor IR da lista
  for (int i = 0; i < NUM_LEDS_IR; i++) {
    switch (protocolo) {
      case NEC:       emissoresIR[i]->sendNEC(cmd.address, cmd.command, 0); break;
      case SONY:      emissoresIR[i]->sendSony(cmd.address, cmd.command, 0); break;
      case RC5:       emissoresIR[i]->sendRC5(cmd.address, cmd.command, 0); break;
      case RC6:       emissoresIR[i]->sendRC6(cmd.address, cmd.command, 0); break;
      case SAMSUNG:   emissoresIR[i]->sendSamsung(cmd.address, cmd.command, 0); break;
      case PANASONIC: emissoresIR[i]->sendPanasonic(cmd.address, cmd.command, 0); break;
      case JVC:       emissoresIR[i]->sendJVC(cmd.address, cmd.command, 0); break;
      case LG:        emissoresIR[i]->sendLG(cmd.address, cmd.command, 0); break;
      case DENON:     emissoresIR[i]->sendDenon(cmd.address, cmd.command, 0); break;
      default:        Serial.println("Protocolo não suportado para envio."); break;
    }
    delay(10); // Evita conflito entre múltiplos envios simultâneos
  }

  Serial.println("Comando IR enviado em todos os pinos.");
}

// === RECEBE COMANDOS DA ESP FIXA VIA ESP-NOW ===
void onDataRecv(const esp_now_recv_info_t *info, const uint8_t *data, int len) {
  String msg = "";
  for (int i = 0; i < len; i++) {
    msg += (char)data[i];
  }

  Serial.print("Comando recebido via ESP-NOW: ");
  Serial.println(msg);

  // Mapeia mensagens para comandos IR gravados
  if (msg == "V+") enviarIR(comandos[0]);
  else if (msg == "V-") enviarIR(comandos[1]);
  else if (msg == "C+") enviarIR(comandos[2]);
  else if (msg == "C-") enviarIR(comandos[3]);
  else if (msg == "LD") {
    Serial.println("Executando sequência de comandos LD...");

    // Envia todos os comandos a partir do índice 4, com os respectivos delays
    for (size_t i = 4; i < comandos.size(); i++) {
      enviarIR(comandos[i]);
      if (comandos[i].delayDepois > 0) {
        delay(comandos[i].delayDepois);
      }
    }
  } else {
    Serial.println("Comando não reconhecido.");
  }
}

void setup() {
  Serial.begin(115200);
  delay(500);

  setupLeds(); // Inicializa os LEDs (externo, em leds.hpp)

  pinMode(PIN_BOTAO_GRAVACAO, INPUT_PULLUP);
  pinMode(PIN_ENCONTRA, INPUT_PULLUP);

  IrReceiver.begin(PIN_RECEPTOR_IR, ENABLE_LED_FEEDBACK); // Inicializa o receptor IR

  // Inicializa os emissores IR
  for (int i = 0; i < NUM_LEDS_IR; i++) {
    emissoresIR[i] = new IRsend(pinosLEDIR[i]);
    emissoresIR[i]->begin(pinosLEDIR[i]);
  }

  carregarTodosComandos(); // Carrega comandos da memória flash

  // Inicializa o WiFi no modo estação
  WiFi.mode(WIFI_STA);

  // Inicializa o ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Erro ao iniciar ESP-NOW");
    return;
  }

  // Registra a função de callback para receber dados
  esp_now_register_recv_cb(onDataRecv);

  // Adiciona o peer (ESP1) para enviar de volta o comando "BZ"
  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, endereco_mac_esp1, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;
  esp_now_add_peer(&peerInfo);

  Serial.println("Setup completo.");
}

void loop() {
  static bool botaoAnterior = HIGH;
  bool botaoAtual = digitalRead(PIN_BOTAO_GRAVACAO);

  updateLeds(); // Atualiza o feedback visual dos LEDs

  // Detecta borda de descida no botão de gravação (pressionamento)
  if (botaoAnterior == HIGH && botaoAtual == LOW) {
    modoGravacao = !modoGravacao; // Alterna o modo
    if (modoGravacao) {
      Serial.println("=== MODO GRAVAÇÃO ATIVADO ===");
      comandos.clear(); // Zera os comandos anteriores
    } else {
      Serial.println("=== MODO GRAVAÇÃO FINALIZADO ===");
      salvarTodosComandos(); // Salva os comandos gravados
    }
    delay(500); // Debounce
  }
  botaoAnterior = botaoAtual;

  // Modo de gravação de comandos IR
  if (modoGravacao) {
    if (IrReceiver.decode()) {
      IRData irData = IrReceiver.decodedIRData;
      if (irData.protocol != UNKNOWN) {
        ComandoIR novo;
        novo.protocol = irData.protocol;
        novo.address = irData.address;
        novo.command = irData.command;

        // Calcula o tempo desde o último comando para definir delay
        unsigned long agora = millis();
        if (comandos.size() > 0) {
          comandos.back().delayDepois = agora - ultimoTempoGravacao;
        }
        ultimoTempoGravacao = agora;

        comandos.push_back(novo); // Adiciona novo comando

        Serial.printf("Comando %d gravado: Prot %s, Addr %X, Cmd %X\n", comandos.size() - 1,
                      getProtocolString((decode_type_t)irData.protocol),
                      irData.address,
                      irData.command);
        Serial.println("Aguardando próximo botão...");

        iniciarAzulTemporario(); // Feedback visual azul (definido em leds.hpp)
        delay(2000); // Tempo entre gravações
      } else {
        Serial.println("Código IR inválido.");
      }
      IrReceiver.resume(); // Prepara para próxima leitura
    }
  }

  // Envia comando "BZ" para ESP1 se botão de localização for pressionado
  if (digitalRead(PIN_ENCONTRA) == LOW) {
    const char* comando = "BZ";
    esp_now_send(endereco_mac_esp1, (uint8_t*)comando, strlen(comando));
    Serial.println("Comando BZ enviado de volta para ESP1");
    delay(500);
  }
}
