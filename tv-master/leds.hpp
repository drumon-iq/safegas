#include <FastLED.h>

// ====================================================================================
// ==              MÓDULO DE CONTROLE DE EFEITOS DE LED (VERSÃO ATUALIZADA)            ==
// ====================================================================================


// --- CONFIGURAÇÕES GERAIS DA FITA ---
#define LED_PIN          21
#define NUM_LEDS         21 
#define COLOR_ORDER      BRG 
#define CHIPSET          WS2811
#define BRIGHTNESS       150 // Brilho máximo dos efeitos (0-255) achei bão assim


// --- ESTADOS POSSÍVEIS DO LED ---
enum EstadoLed {
  DESLIGADO,
  ARCO_IRIS,
  ENTRANDO_CONFIG,
  PULSANDO_UMA_VEZ,
  PULSANDO_DUAS_VEZES,
  PULSANDO_TRES_VEZES,
  PULSANDO_QUATRO_VEZES,  
  PULSANDO_CINCO_VEZES,   
  ACESO_CONTINUO,
  AZUL_TEMPORARIO,
  // Estados de "uma só vez" (one-shot) que não precisam de lógica no loop
  CONFIRMACAO_GIRANDO,
  FINALIZACAO
};


// --- VARIÁVEIS INTERNAS DO MÓDULO ---
CRGB leds[NUM_LEDS]; //array de cores
EstadoLed estadoAtual = ARCO_IRIS; //começa com ARCO_IRIS (vai ficar assim normalmente) quando o dispositivo é ligado.
EstadoLed estadoAnterior = ARCO_IRIS;

unsigned long tempoInicioEfeito = 0;
uint8_t contadorDePulsos = 0;


// --- FUNÇÕES DE CONTROLE ---

void setEstadoLed(EstadoLed novoEstado);

void setupLeds() {
  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);
  FastLED.clear(true);
}

void iniciarModoConfig() {
  setEstadoLed(ENTRANDO_CONFIG);
}

void dispararConfirmacaoGeral() {
  estadoAnterior = estadoAtual;
  setEstadoLed(CONFIRMACAO_GIRANDO);

  unsigned long tempoInicioGiro = millis();
  uint8_t cometaPos = 0;

  while (millis() - tempoInicioGiro < 1000) {
    fadeToBlackBy(leds, NUM_LEDS, 40);
    cometaPos = (cometaPos + 1) % NUM_LEDS;
    leds[cometaPos] = CRGB::White;
    FastLED.show();
    FastLED.delay(10);
  }

  setEstadoLed(estadoAnterior);
}

void iniciarAzulTemporario() {
  estadoAnterior = estadoAtual;
  setEstadoLed(AZUL_TEMPORARIO);
}

void dispararFlashFinalizacao() {
  setEstadoLed(FINALIZACAO);
  for (int i = 0; i < 5; i++) {
    fill_solid(leds, NUM_LEDS, CRGB::Red); 
    FastLED.show();
    FastLED.delay(50);
    FastLED.clear();
    FastLED.show();
    FastLED.delay(50);
  }
  setEstadoLed(DESLIGADO);
}

// a cada ciclo do loop, a função updateLeds é executada para manter a animação fluindo.
void updateLeds() {
  switch (estadoAtual) {

    case DESLIGADO:
      FastLED.clear();
      break;

    case ARCO_IRIS: {
      static uint8_t startHue = 0;
      fill_rainbow(leds, NUM_LEDS, startHue++, 7);
      break;
    }

    case ENTRANDO_CONFIG: {
      static uint8_t chaseCounter = 0;
      FastLED.clear();
      for (int i = 0; i < NUM_LEDS; i++) {
        if ((i + chaseCounter) % 4 == 0) {
          leds[i] = CRGB::White;
        }
      }
      EVERY_N_MILLISECONDS(60) { chaseCounter++; }

      if (millis() - tempoInicioEfeito > 6000) {
        setEstadoLed(PULSANDO_UMA_VEZ);
      }
      break;
    }

    case PULSANDO_UMA_VEZ: {
      // Este efeito é uma onda senoidal contínua
      uint8_t brilho = beatsin8(15, 20, 200); 
      fill_solid(leds, NUM_LEDS, CRGB(brilho, 0, 0));
      break;
    }

    // <-- MUDANÇA: Bloco de case expandido para incluir os novos estados
    case PULSANDO_DUAS_VEZES:
    case PULSANDO_TRES_VEZES:
    case PULSANDO_QUATRO_VEZES:
    case PULSANDO_CINCO_VEZES: {
      
      uint8_t maxPulsos = 0;
      if (estadoAtual == PULSANDO_DUAS_VEZES)       maxPulsos = 2;
      else if (estadoAtual == PULSANDO_TRES_VEZES)  maxPulsos = 3;
      else if (estadoAtual == PULSANDO_QUATRO_VEZES)maxPulsos = 4;
      else if (estadoAtual == PULSANDO_CINCO_VEZES) maxPulsos = 5;

      uint16_t duracaoPulso = 400; // Duração de cada flash
      uint16_t duracaoPausa = 1500; // Pausa longa no final da sequência

      unsigned long tempoTotalSequencia = (maxPulsos * duracaoPulso) + duracaoPausa;
      unsigned long tempoCiclo = (millis() - tempoInicioEfeito) % tempoTotalSequencia;

      uint8_t brilho = 0; // Começa apagado (na pausa)

      // Verifica se estamos no período dos pulsos (não na pausa longa)
      if (tempoCiclo < (maxPulsos * duracaoPulso)) {
          // Usa uma onda senoidal curta para criar um pulso suave
          // A função `beat8` cria uma rampa de 0 a 255
          // A frequência (primeiro argumento) é ajustada pela duração do pulso
          brilho = beatsin8( (255 * 2) / (duracaoPulso/10) , 0, 255);
      }
      
      fill_solid(leds, NUM_LEDS, CRGB(brilho, 0, 0));
      break;
    }
    
    case ACESO_CONTINUO:
      fill_solid(leds, NUM_LEDS, CRGB::Green);
      break;

    case AZUL_TEMPORARIO:
      fill_solid(leds, NUM_LEDS, CRGB::Blue);
      if (millis() - tempoInicioEfeito > 5000) {
        setEstadoLed(estadoAnterior);
      }
      break;

    case CONFIRMACAO_GIRANDO:
    case FINALIZACAO:
      // Sem lógica aqui, são efeitos "one-shot"
      break;
  }

  FastLED.show();
}

// Toda vez que você quer mudar de um efeito para outro, você chama esta função.
void setEstadoLed(EstadoLed novoEstado) {
  if (estadoAtual == novoEstado && novoEstado != CONFIRMACAO_GIRANDO) return;

  estadoAnterior = estadoAtual;
  estadoAtual = novoEstado;
  tempoInicioEfeito = millis();
}