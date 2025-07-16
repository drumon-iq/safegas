#include <WiFi.h>
#include "sensores.h"
#include "util.h"
#include "webpage.h"

#define SSID "blyeat"
#define PASS "123456789"

bool doDebug = true;

char client_c;
char old_client_c = '\0';
String Sbuffer;

WiFiServer server(80);
WiFiClient client;

void setup() {
  Serial.begin(115200);

  if (!doDebug)
    setup_sensores();

  set_duration_min(doDebug ? 1 : 5);
  timer = 0;
  old_millis = 0;

  WiFi.softAP(SSID, PASS);
  server.begin();
  WiFiClient client = server.available();
}

void loop() {
  unsigned long int curr_millis = millis();

  digitalWrite(LED, LOW);
  tone(buzzer, 0);

  if (doDebug || (!movementDetected() && flameDetected()))
    timer += curr_millis - old_millis;
  else
    timer = 0;

  if (timer >= 2 * duration)
    setValvula(false);
  else if (timer >= duration) {
    doAlert = !doAlert; //flip pra depois flop
    digitalWrite(LED, doAlert ? HIGH : LOW);
    tone(buzzer, doAlert ? 4000 : 0);
  }

  // Maybe loop the client reading before moving on?
  if (client) {
    if (client.available() && client.connected()) {

      client_c = client.read();
      Serial.write(old_client_c);
      Sbuffer += client_c;

      // End of HTTP request, an \r followed by an \n
      if (client_c == '\n' && old_client_c == '\r') {
        client.println("HTTP/1.1 200 OK");
        client.println("Content-type:text/html");
        client.println("Connection: close");
        client.println();
        // Decide what to print
        // Holy mother of inefficiency
        if (Sbuffer.indexOf("gasValue") != -1){
          client.println(analogRead(sensor_gas));
        } else if (Sbuffer.indexOf("flameValue") != -1){
          client.println(analogRead(sensor_fogo));
        } else if (Sbuffer.indexOf("valveValue") != -1) {
          client.println(valvulaVal ? "1" : "0");
        } else if (Sbuffer.indexOf("toggleValve") != -1) {
          setValvula(!valvulaVal);
          client.println(webpage);
        } else {
          client.println(webpage);
        }
        client.println();
        Sbuffer.clear();
        client.stop();
      }
      old_client_c = client_c;
    }
  } else {
    client = server.available(); // Idk why we can keep asking the server for new clients
    old_client_c = 0;
  }

  old_millis = curr_millis;
}
