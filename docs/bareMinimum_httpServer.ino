/* Toda requisição HTTP termina com um carrige return e um carriage newline
 *  ( \r e \n nessa ordem )
 * 
 * O código espera o cliente terminar sua mensagem para mandar a nossa,
 * Como toda variavel em um GET segue o modelo var=valor é possivel simplesmente esperar pelo
 * caracter '=' para pegar as variaveis
 *
 * BOOM toma esses links:
 * https://developer.mozilla.org/en-US/docs/Web/HTTP/Guides/Messages
*/

#include <WiFi.h>

#define SSID "esp32 Access Point"
#define PASS "123456789"

WiFiServer server(80);

void setup()
{
  Serial.begin(115200);

  WiFi.softAP(SSID, PASS);

  // Mostra o IP interno, normalmente 192.168.4.1
  //IPAddress IP = WiFi.softAPIP();
  //Serial.println(IP);

  server.begin();
}

void loop()
{
  WiFiClient client = server.available();

  if (client) {
    char cur;
    char old = '\0';

    while (client.connected()) {
      if (client.available()) {
        Serial.println("Connection created");
        cur = client.read();

        if (old == '\r' && cur == '\n') {
          client.println("HTTP/1.1 200 OK");
          client.println("Content-type:text/html");
          client.println("Connection: close");
          client.println();
          client.println("HELLO WORLD");
          client.println();
          client.stop();
        }

        old = cur;
      }
    }
    client.stop();
    Serial.println("Connection closed");
  }
}
