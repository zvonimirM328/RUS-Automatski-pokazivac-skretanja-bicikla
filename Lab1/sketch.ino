#define LED_PIN 13
#define BUTTON_PIN 2

volatile bool buttonPressed = false;
unsigned long previousMillis = 0;
const long interval = 1000;

void setup() {
    pinMode(LED_PIN, OUTPUT);
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    
    attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), buttonISR, FALLING);
    Serial.begin(9600);
}

void loop() {
    if (buttonPressed) {
        Serial.println("Button pressed!");
        digitalWrite(LED_PIN, !digitalRead(LED_PIN));
        buttonPressed = false;
    }

    if (millis() - previousMillis >= interval) {
        previousMillis = millis();
        timerISR();
    }
}

void buttonISR() {
    buttonPressed = true;
}

void timerISR() {
    Serial.println("Timer Interrupt Triggered");
}
