#include <WiFi.h>

enum GPIO{
  pin26 = 26,
  pin27
};

const char ssid[] = "blyeat";
const char pass[] = "123456789";

WiFiServer server(80);

void
setup()
{
  Serial.begin(115200);

  pinMode(pin26, OUTPUT);
  pinMode(pin27, OUTPUT);

  digitalWrite(pin26, LOW);
  digitalWrite(pin27, LOW);

  WiFi.softAP(ssid, pass);

  server.begin();
}

void
loop()
{
  WiFiClient client = server.available();

  if (client) {
    String currentLine = "";
    String buffalo = "";

    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        if (c == '\n') {
          if (currentLine.length == 2) {

          }
        }
      }
    }
    client.stop();
    buffalu = "";
  }
}
