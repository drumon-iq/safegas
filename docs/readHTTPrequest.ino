// Checkout more at https://github.com/espressif/arduino-esp32/tree/master/libraries/WiFi
#include <WiFi.h>

const char ssid[] = "blyeat";
const char pass[] = "123456789";

WiFiServer server(80);

void setup()
{
  delay(5000); // So you can connect to the monitor before it starts to spill out info
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

  // dogshit if else mess, so sorry
  if (client) {
    Serial.println("> found client");
    String currentline = "";

    /* Main loop ->
     *  A client is found! (hooray), 
     *
     *  begins a inner loop with that client
     *  when client is no longer connected, we close the connection on our
     *  side and cleanup memory
     *
     *  when client is avaible, we can read data from them
     *  if client data has a \n\n (double new line characters) it means
     *  that is the end of the HTTP request, and we should send our 
     *  response
     *
     *  TODO: a less bad way to organize this control flow
     */

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
/* RANT:
* Why the hell does everyone uses goddamn object oriented dynamically stored 
* Strings?? Do you hate your microcontroller???
*/
