#include <WiFi.h>
#include <HTTPClient.h>

const char ssid[] = "Lips";
const char pass[] = "sala3086";

const char serverPath[] = "http://192.168.0.152:8000/dummyGET";

// delay in miliseconds
unsigned long timerDelay = 5000;
unsigned long lastTime = 0;

char charBuffer[30] = "\0";

void setup() 
{
  delay(10000);
  Serial.begin(9600);

  Serial.println("Connecting to wifi station");
  WiFi.begin(ssid, pass);
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Connected successfully to network as ");
  Serial.println(WiFi.localIP());

}

void loop() 
{
  if ((millis() - lastTime) > timerDelay) {
    if (WiFi.status() == WL_CONNECTED) {
      httpGETRequest(serverPath, charBuffer);
      Serial.println(charBuffer);
    } else
      Serial.print("Failed to connect to wifi");

    lastTime = millis();
  }
}

void httpGETRequest(const char* serverPath, char buffer[30]) 
{
  WiFiClient client;
  HTTPClient http;
  String httpAnswer;

  http.begin(client, serverPath);

  int responseCode = http.GET();

  if (responseCode>0) {
    Serial.print("Received http answer, response code is: ");
    Serial.println(responseCode);

    httpAnswer = http.getString();
    for (int i=0; i<29; i++) {
      buffer[i] = httpAnswer[i];
    }
  }
  else {
    Serial.print("Received http error, response code is: ");
    Serial.println(responseCode);
  }

  http.end();
}
