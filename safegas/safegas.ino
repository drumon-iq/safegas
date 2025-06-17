#include <WiFi.h>
#include "sensores.h"
#include "util.h"

#define SSID "blyeat"
#define PASS "123456789"

bool doDebug = false;
bool estadoValvula = false;
bool ultimoEstadoBotao = false;


char client_c;
char old_client_c = '\0';

WiFiServer server(80);
WiFiClient client;

void setup() {
  Serial.begin(115200);

  setup_sensores();

  set_duration_min(doDebug ? 1 : 5);
  timer = 0;
  old_millis = 0;

  WiFi.softAP(SSID, PASS);
  server.begin();
  WiFiClient client = server.available();
}

void loop() {

  bool estadoBotao = digitalRead(BOTAO_PIN);

  if (estadoBotao == HIGH && ultimoEstadoBotao == LOW) {
    estadoValvula = !estadoValvula;
    toggleValvula(estadoValvula);
  }

  ultimoEstadoBotao = estadoBotao;

  unsigned long int curr_millis = millis();

  digitalWrite(LED, LOW);
  tone(buzzer, 0);

  Serial.print("Timer: ");
  Serial.println(timer);

  if (doDebug || (!movementDetected() && flameDetected() && !estadoValvula))
    timer += curr_millis - old_millis;
  else
    timer = 0;

  if (timer >= 2 * duration) {
    estadoValvula = false;
    toggleValvula(false);
  } else if (timer >= duration) {
    doAlert = !doAlert; //flip pra depois flop
    digitalWrite(LED, doAlert ? HIGH : LOW);
    tone(buzzer, doAlert ? 4000 : 0);
  }

  if (client) {
    if (client.available() && client.connected()) {

      client_c = client.read();
      Serial.write(old_client_c);

      if (old_client_c == '\r' && client_c == '\n') {
        client.println("HTTP/1.1 200 OK");
        client.println("Content-type:text/html");
        client.println("Connection: close");
        client.println();
        client.println("HELLO WORLD");
        client.println();

        client.stop();
      }
      old_client_c = client_c;
    }
  } else {
    client = server.available();
    old_client_c = 0;
  }

  old_millis = curr_millis;
}
