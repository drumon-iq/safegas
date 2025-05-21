#include <WiFi.h>

const char ssid[] = "blyeat";
const char pass[] = "123456789";

WiFiServer server(80);

void setup()
{
  delay(5000);
  Serial.begin(115200);

  Serial.println("Setting up Access Point");
  WiFi.softAP(ssid, pass);

  Serial.println("Access Point created as: ");
  Serial.println(ssid);
  Serial.println(pass);
  Serial.println("AP IP Address: ");

  IPAddress IP = WiFi.softAPIP();
  Serial.println(IP);

  server.begin();
  delay(5000);
}

void loop()
{
  WiFiClient client = server.available();

  if (client) {
    Serial.println("New client found");
    String currentline = "";
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        currentline += c;
        if (c == '\n') {
          if (currentline.length() == 2) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            client.println("<html><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"></head>");
            client.println("<body><h1>Blyaat</h1><p>orange</p></body></html>");
            client.println();
            break;
          } else {
              currentline = "";
          }
        } else if (c != '\r') {
          currentline += c;
        }
      }
    }
    client.stop();
    Serial.println("Connection with client gone");
  }
}
