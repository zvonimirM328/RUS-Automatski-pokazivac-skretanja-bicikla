#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>

#define LED_PIN 13  
#define BUTTON_PIN 2 

volatile bool wake_up = false;
volatile uint8_t sleep_mode_type = 0; 
volatile bool button_pressed = false;

const float ACTIVE_CURRENT = 5.0;    
const float POWER_DOWN_CURRENT = 0.0001; 
const float IDLE_CURRENT = 1.5;      
const float STANDBY_CURRENT = 0.001; 

void setup() {

  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), wakeFromButton, FALLING);
  
  cli();
  wdt_reset();
  WDTCSR |= (1 << WDCE) | (1 << WDE);
  WDTCSR = (1 << WDIE) | (1 << WDP3); 
  sei();
  
  Serial.begin(9600);
  Serial.println("Sustav pokrenut. Potrosnja se simulira.");

}

void blinkLED() {

  Serial.print("Aktivno stanje - LED trepti. Potrosnja: ");
  Serial.print(ACTIVE_CURRENT);
  Serial.println(" mA");

  for (int i = 0; i < 5; i++) {
    digitalWrite(LED_PIN, HIGH);
    delay(500);
    digitalWrite(LED_PIN, LOW);
    delay(500);
  }
}

void wakeFromButton() {

  if (!button_pressed) {
    wake_up = true;
    button_pressed = true;
    Serial.println("Budenje putem tipke (INT0)");
  }
}

ISR(WDT_vect) {

  wake_up = true;
  Serial.println("Budenje putem Watchdog timera");

}

void enterSleep() {

  if (sleep_mode_type == 0) {
    Serial.print("Ulazak u Power-down mode. Potrosnja: ");
    Serial.print(POWER_DOWN_CURRENT, 4);
    Serial.println(" mA");
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  } else if (sleep_mode_type == 1) {
    Serial.print("Ulazak u Idle mode. Potrosnja: ");
    Serial.print(IDLE_CURRENT);
    Serial.println(" mA");
    set_sleep_mode(SLEEP_MODE_IDLE);
  } else { 
    Serial.print("Ulazak u Standby mode. Potrosnja: ");
    Serial.print(STANDBY_CURRENT, 4);
    Serial.println(" mA");
    set_sleep_mode(SLEEP_MODE_STANDBY);
  }
  
  sleep_enable();
  ADCSRA &= ~(1 << ADEN);
  sleep_cpu();
  sleep_disable();
  ADCSRA |= (1 << ADEN);

}

void loop() {

  blinkLED();
  
  wake_up = false;
  button_pressed = false;
  
  sleep_mode_type = 0;
  enterSleep();

  if (wake_up) {
    Serial.println("Mikrokontroler se probudio!");
    delay(100);
  }
  
  sleep_mode_type = 1;
  enterSleep();

  if (wake_up) {
    Serial.println("Mikrokontroler se probudio iz Idle moda!");
    delay(100);
  }
  
  sleep_mode_type = 2;
  enterSleep();

  if (wake_up) {
    Serial.println("Mikrokontroler se probudio iz Standby moda!");
    delay(100);
  }
}
