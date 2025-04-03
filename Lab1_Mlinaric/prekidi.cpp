#define BUTTON0 2  // INT0
#define BUTTON1 3  // INT1
#define BUTTON2 21 // INT2

#define LED_INT0 8
#define LED_INT1 9
#define LED_INT2 10
#define LED_TIMER 11
#define LED_SENSOR 12
#define TRIG_PIN 6
#define ECHO_PIN 7

volatile bool timerInterruptFlag = false;
volatile bool button0InterruptFlag = false;
volatile bool button1InterruptFlag = false;
volatile bool button2InterruptFlag = false;
volatile bool sensorInterruptFlag = false;

unsigned long previousMillisLED0 = 0;
unsigned long previousMillisLED1 = 0;
unsigned long previousMillisLED2 = 0;
unsigned long previousMillisTimer = 0;
unsigned long previousMillisSensor = 0;
const unsigned long blinkInterval = 100;
const unsigned long debounceDelay = 200;
int led0State = 0, led1State = 0, led2State = 0, sensorLedState = 0;
int blinkCount0 = 0, blinkCount1 = 0, blinkCount2 = 0, sensorBlinkCount = 0;

unsigned long lastInterruptTime0 = 0;
unsigned long lastInterruptTime1 = 0;
unsigned long lastInterruptTime2 = 0;

void setup() {
    pinMode(BUTTON0, INPUT_PULLUP);
    pinMode(BUTTON1, INPUT_PULLUP);
    pinMode(BUTTON2, INPUT_PULLUP);
    
    pinMode(LED_INT0, OUTPUT);
    pinMode(LED_INT1, OUTPUT);
    pinMode(LED_INT2, OUTPUT);
    pinMode(LED_TIMER, OUTPUT);
    pinMode(LED_SENSOR, OUTPUT);
    
    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);
    
    attachInterrupt(digitalPinToInterrupt(BUTTON0), handleButton0, FALLING);
    attachInterrupt(digitalPinToInterrupt(BUTTON1), handleButton1, FALLING);
    attachInterrupt(digitalPinToInterrupt(BUTTON2), handleButton2, FALLING);
    
    setupTimer1();
}

void setupTimer1() {
    noInterrupts();
    TCCR1A = 0;
    TCCR1B = 0;
    TCNT1 = 0;
    OCR1A = 15624;
    TCCR1B |= (1 << WGM12);
    TCCR1B |= (1 << CS12) | (1 << CS10);
    TIMSK1 |= (1 << OCIE1A);
    interrupts();
}

ISR(TIMER1_COMPA_vect) {
    timerInterruptFlag = true;
}

void handleButton0() {
    unsigned long interruptTime = millis();
    if (interruptTime - lastInterruptTime0 > debounceDelay) {
        button0InterruptFlag = true;
    }
    lastInterruptTime0 = interruptTime;
}

void handleButton1() {
    unsigned long interruptTime = millis();
    if (interruptTime - lastInterruptTime1 > debounceDelay) {
        button1InterruptFlag = true;
    }
    lastInterruptTime1 = interruptTime;
}

void handleButton2() {
    unsigned long interruptTime = millis();
    if (interruptTime - lastInterruptTime2 > debounceDelay) {
        button2InterruptFlag = true;
    }
    lastInterruptTime2 = interruptTime;
}

void loop() {
    unsigned long currentMillis = millis();

    if (timerInterruptFlag) {
        timerInterruptFlag = false;
        previousMillisTimer = currentMillis;
        digitalWrite(LED_TIMER, HIGH);
        checkDistance();
    }
    if (currentMillis - previousMillisTimer >= blinkInterval) {
        digitalWrite(LED_TIMER, LOW);
    }

    if (button0InterruptFlag) {
        button0InterruptFlag = false;
        blinkCount0 = 5;
    }
    blinkLED(LED_INT0, previousMillisLED0, led0State, blinkCount0);

    if (button1InterruptFlag) {
        button1InterruptFlag = false;
        blinkCount1 = 5;
    }
    blinkLED(LED_INT1, previousMillisLED1, led1State, blinkCount1);

    if (button2InterruptFlag) {
        button2InterruptFlag = false;
        blinkCount2 = 5;
    }
    blinkLED(LED_INT2, previousMillisLED2, led2State, blinkCount2);

    if (sensorInterruptFlag) {
        sensorInterruptFlag = false;
        sensorBlinkCount = 5;
    }
    blinkLED(LED_SENSOR, previousMillisSensor, sensorLedState, sensorBlinkCount);
}

void blinkLED(int ledPin, unsigned long &previousMillis, int &ledState, int &blinkCount) {
    if (blinkCount > 0) {
        unsigned long currentMillis = millis();
        if (currentMillis - previousMillis >= blinkInterval) {
            previousMillis = currentMillis;
            ledState = !ledState;
            digitalWrite(ledPin, ledState);
            if (ledState == LOW) {
                blinkCount--;
            }
        }
    }
}

void checkDistance() {
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);
    
    long duration = pulseIn(ECHO_PIN, HIGH, 20000);
    if (duration == 0) {
        duration = 20000; // Assume max distance if no echo received
    }
    
    float distance = duration * 0.0343 / 2;
    if (distance < 100) {
        sensorInterruptFlag = true;
    }
}
