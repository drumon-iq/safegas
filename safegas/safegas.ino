#include <WiFi.h>
#include "sensores.h"
#include "util.h"

bool doDebug = false;

void setup ()
{
  Serial.begin (115200);

  setup_sensores ();
  digitalWrite (relePin, HIGH);

  set_duration_min (doDebug ? 1 : 5);
  timer = 0;
  old_millis = 0;
}

void loop ()
{
  unsigned long int curr_millis = millis();

  digitalWrite (LED, LOW);
  tone (buzzer, 0);

  Serial.print ("Timer: ");
  Serial.println (timer);

  if (doDebug || (!movementDetected() && flameDetected()))
    timer += curr_millis - old_millis;
  else
    timer = 0;

  if (timer >= 2 * duration)
    toggleValvula(false);
  else if (timer >= duration) {
    doAlert = !doAlert; //flip pra depois flop
    digitalWrite (LED, doAlert ? HIGH : LOW);
    tone (buzzer, doAlert ? 4000 : 0);
  }

  delay (100);
  old_millis = curr_millis;
}
