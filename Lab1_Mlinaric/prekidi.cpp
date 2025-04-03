/**
 * @file main.cpp
 * @brief Program za upravljanje LED-ovima, tipkama i ultrazvučnim senzorom
 *
 * Ovaj program koristi prekide za detekciju pritiska tipki, timer za redovite intervale
 * i ultrazvučni senzor za mjerenje distance. Također uključuje LED-ove koji trepću
 * na temelju različitih događaja.
 */

#define BUTTON0 2  /**< Pin za prvu tipku koja koristi prekid INT0. */
#define BUTTON1 3  /**< Pin za drugu tipku koja koristi prekid INT1. */
#define BUTTON2 21 /**< Pin za treću tipku koja koristi prekid INT2. */

#define LED_INT0 8    /**< Pin za LED povezanu s prekidom INT0. */
#define LED_INT1 9    /**< Pin za LED povezanu s prekidom INT1. */
#define LED_INT2 10   /**< Pin za LED povezanu s prekidom INT2. */
#define LED_TIMER 11  /**< Pin za LED povezanu s timera. */
#define LED_SENSOR 12 /**< Pin za LED povezanu s ultrazvučnim senzorom. */
#define TRIG_PIN 6    /**< Pin za trig funkciju ultrazvučnog senzora. */
#define ECHO_PIN 7    /**< Pin za echo funkciju ultrazvučnog senzora. */

volatile bool timerInterruptFlag = false;         /**< Signalizira da je timer interrupt aktiviran. */
volatile bool button0InterruptFlag = false;      /**< Signalizira da je Button0 interrupt aktiviran. */
volatile bool button1InterruptFlag = false;      /**< Signalizira da je Button1 interrupt aktiviran. */
volatile bool button2InterruptFlag = false;      /**< Signalizira da je Button2 interrupt aktiviran. */
volatile bool sensorInterruptFlag = false;       /**< Signalizira da je senzor interrupt aktiviran. */

unsigned long previousMillisLED0 = 0;            /**< Vrijeme proteklo od posljednjeg LED blinkanja za LED_INT0. */
unsigned long previousMillisLED1 = 0;            /**< Vrijeme proteklo od posljednjeg LED blinkanja za LED_INT1. */
unsigned long previousMillisLED2 = 0;            /**< Vrijeme proteklo od posljednjeg LED blinkanja za LED_INT2. */
unsigned long previousMillisTimer = 0;           /**< Vrijeme proteklo od posljednjeg timer interrupta. */
unsigned long previousMillisSensor = 0;          /**< Vrijeme proteklo od posljednjeg senzor interrupta. */
const unsigned long blinkInterval = 100;         /**< Interval za blinksanje LED-a u milisekundama. */
const unsigned long debounceDelay = 200;         /**< Debounce interval za tipke u milisekundama. */
int led0State = 0, led1State = 0, led2State = 0, sensorLedState = 0; /**< Trenutna stanja LED-ova. */
int blinkCount0 = 0, blinkCount1 = 0, blinkCount2 = 0, sensorBlinkCount = 0; /**< Broj treptanja LED-ova. */

unsigned long lastInterruptTime0 = 0;            /**< Vrijeme posljednjeg interrupta za Button0, za debouncing. */
unsigned long lastInterruptTime1 = 0;            /**< Vrijeme posljednjeg interrupta za Button1, za debouncing. */
unsigned long lastInterruptTime2 = 0;            /**< Vrijeme posljednjeg interrupta za Button2, za debouncing. */

/**
 * @brief Inicijalizira pinove, prekide i timer.
 *
 * Postavlja pinMode za tipke, LED-ove i senzor, te povezuje prekide za tipke.
 */
void setup() {
    pinMode(BUTTON0, INPUT_PULLUP); /**< Postavljanje BUTTON0 kao ulaz s pull-up otpornikom. */
    pinMode(BUTTON1, INPUT_PULLUP); /**< Postavljanje BUTTON1 kao ulaz s pull-up otpornikom. */
    pinMode(BUTTON2, INPUT_PULLUP); /**< Postavljanje BUTTON2 kao ulaz s pull-up otpornikom. */
    
    pinMode(LED_INT0, OUTPUT);     /**< Postavljanje LED_INT0 kao izlaz. */
    pinMode(LED_INT1, OUTPUT);     /**< Postavljanje LED_INT1 kao izlaz. */
    pinMode(LED_INT2, OUTPUT);     /**< Postavljanje LED_INT2 kao izlaz. */
    pinMode(LED_TIMER, OUTPUT);    /**< Postavljanje LED_TIMER kao izlaz. */
    pinMode(LED_SENSOR, OUTPUT);   /**< Postavljanje LED_SENSOR kao izlaz. */
    
    pinMode(TRIG_PIN, OUTPUT);    /**< Postavljanje TRIG_PIN kao izlaz. */
    pinMode(ECHO_PIN, INPUT);     /**< Postavljanje ECHO_PIN kao ulaz. */
    
    attachInterrupt(digitalPinToInterrupt(BUTTON0), handleButton0, FALLING); /**< Postavljanje prekida za BUTTON0. */
    attachInterrupt(digitalPinToInterrupt(BUTTON1), handleButton1, FALLING); /**< Postavljanje prekida za BUTTON1. */
    attachInterrupt(digitalPinToInterrupt(BUTTON2), handleButton2, FALLING); /**< Postavljanje prekida za BUTTON2. */
    
    setupTimer1(); /**< Postavljanje Timer1 za generiranje prekida svakih 1 sekundu. */
}

/**
 * @brief Postavlja Timer1 za generiranje prekida svakih 1 sekundu.
 *
 * Timer1 je postavljen u CTC mod i koristi prescaler od 1024.
 */
void setupTimer1() {
    noInterrupts();                /**< Onemogućavanje svih prekida. */
    TCCR1A = 0;                     /**< Postavljanje registra TCCR1A na 0. */
    TCCR1B = 0;                     /**< Postavljanje registra TCCR1B na 0. */
    TCNT1 = 0;                      /**< Postavljanje brojača na 0. */
    OCR1A = 15624;                  /**< Postavljanje usporedne vrijednosti za 1 Hz (1 sekunda). */
    TCCR1B |= (1 << WGM12);         /**< Postavljanje CTC moda za Timer1. */
    TCCR1B |= (1 << CS12) | (1 << CS10); /**< Postavljanje prescalera na 1024. */
    TIMSK1 |= (1 << OCIE1A);        /**< Omogućavanje prekida za usporedbu Timer1. */
    interrupts();                   /**< Omogućavanje prekida. */
}

/**
 * @brief ISR za Timer1 koji se aktivira na svako odbrojavanje Timer1.
 * 
 * Ovdje se postavlja signal da je timer interrupt aktiviran.
 */
ISR(TIMER1_COMPA_vect) {
    timerInterruptFlag = true; /**< Signalizira da je timer interrupt aktiviran. */
}

/**
 * @brief Funkcija koja obrađuje pritisak tipke 0.
 *
 * Ovdje se debounca interrupt i postavlja flag za Button0.
 */
void handleButton0() {
    unsigned long interruptTime = millis();
    if (interruptTime - lastInterruptTime0 > debounceDelay) {
        button0InterruptFlag = true; /**< Postavljanje flag-a za Button0 interrupt. */
    }
    lastInterruptTime0 = interruptTime; /**< Ažuriranje vremena posljednjeg interrupta za Button0. */
}

/**
 * @brief Funkcija koja obrađuje pritisak tipke 1.
 *
 * Ovdje se debounca interrupt i postavlja flag za Button1.
 */
void handleButton1() {
    unsigned long interruptTime = millis();
    if (interruptTime - lastInterruptTime1 > debounceDelay) {
        button1InterruptFlag = true; /**< Postavljanje flag-a za Button1 interrupt. */
    }
    lastInterruptTime1 = interruptTime; /**< Ažuriranje vremena posljednjeg interrupta za Button1. */
}

/**
 * @brief Funkcija koja obrađuje pritisak tipke 2.
 *
 * Ovdje se debounca interrupt i postavlja flag za Button2.
 */
void handleButton2() {
    unsigned long interruptTime = millis();
    if (interruptTime - lastInterruptTime2 > debounceDelay) {
        button2InterruptFlag = true; /**< Postavljanje flag-a za Button2 interrupt. */
    }
    lastInterruptTime2 = interruptTime; /**< Ažuriranje vremena posljednjeg interrupta za Button2. */
}

/**
 * @brief Glavna petlja programa.
 *
 * Ovdje se provode sve funkcionalnosti: obrađuju se interrupti, provodi se mjerenje distance i treptanje LED-a.
 */
void loop() {
    unsigned long currentMillis = millis(); /**< Pohranjivanje trenutnog vremena. */

    if (timerInterruptFlag) {
        timerInterruptFlag = false;
        previousMillisTimer = currentMillis;
        digitalWrite(LED_TIMER, HIGH); /**< Uključivanje LED_TIMER. */
        checkDistance(); /**< Provjera distance pomoću ultrazvučnog senzora. */
    }
    if (currentMillis - previousMillisTimer >= blinkInterval) {
        digitalWrite(LED_TIMER, LOW); /**< Isključivanje LED_TIMER. */
    }

    if (button0InterruptFlag) {
        button0InterruptFlag = false;
        blinkCount0 = 5; /**< Pokretanje treptanja LED_INT0. */
    }
    blinkLED(LED_INT0, previousMillisLED0, led0State, blinkCount0);

    if (button1InterruptFlag) {
        button1InterruptFlag = false;
        blinkCount1 = 5; /**< Pokretanje treptanja LED_INT1. */
    }
    blinkLED(LED_INT1, previousMillisLED1, led1State, blinkCount1);

    if (button2InterruptFlag) {
        button2InterruptFlag = false;
        blinkCount2 = 5; /**< Pokretanje treptanja LED_INT2. */
    }
    blinkLED(LED_INT2, previousMillisLED2, led2State, blinkCount2);

    if (sensorInterruptFlag) {
        sensorInterruptFlag = false;
        sensorBlinkCount = 5; /**< Pokretanje treptanja LED_SENSOR. */
    }
    blinkLED(LED_SENSOR, previousMillisSensor, sensorLedState, sensorBlinkCount);
}

/**
 * @brief Funkcija za treptanje LED-a.
 *
 * Ova funkcija upravlja LED pinom na temelju vremena proteklog od posljednjeg treptanja.
 *
 * @param ledPin Pin LED-a koji treba treptati.
 * @param previousMillis Pohranjuje posljednje vrijeme kada je LED trepnuo.
 * @param ledState Trenutno stanje LED-a (uključeno/isključeno).
 * @param blinkCount Broj preostalih treptanja LED-a.
 */
void blinkLED(int ledPin, unsigned long &previousMillis, int &ledState, int &blinkCount) {
    if (blinkCount > 0) {
        unsigned long currentMillis = millis();
        if (currentMillis - previousMillis >= blinkInterval) {
            previousMillis = currentMillis;
            ledState = !ledState;
            digitalWrite(ledPin, ledState);
            if (ledState == LOW) {
                blinkCount--; /**< Smanjenje broja preostalih treptanja. */
            }
        }
    }
}

/**
 * @brief Funkcija za mjerenje distance pomoću ultrazvučnog senzora.
 *
 * Aktivira trig pin, mjeri trajanje echo signala i izračunava udaljenost.
 * Ako je udaljenost manja od 100 cm, postavlja sensor interrupt flag.
 */
void checkDistance() {
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);
    
    long duration = pulseIn(ECHO_PIN, HIGH);
    long distance = duration * 0.0344 / 2;

    if (distance < 100) {
        sensorInterruptFlag = true; /**< Ako je udaljenost manja od 100 cm, postavlja sensor interrupt flag. */
    }
}
