#include <LowPower.h>

const int ledPin = 13;
const int buttonPin = 2; // INT0

volatile bool wakeUpFlag = false;

void wakeUp() {
  wakeUpFlag = true;
}

void setup() {
  pinMode(ledPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(buttonPin), wakeUp, FALLING);
}

void loop() {
  // Trepni LED-icom 5 puta
  for (int i = 0; i < 5; i++) {
    digitalWrite(ledPin, HIGH);
    delay(1000);
    digitalWrite(ledPin, LOW);
    delay(1000);
  }

  wakeUpFlag = false;

  // Sleep na ukupno 16 sekundi ili dok se ne pritisne tipkalo
  for (int i = 0; i < 2 && !wakeUpFlag; i++) {
    LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
  }
}
