// Pin definitions for buttons and LEDs
#define BUTTON0 2  // INT0 - Button connected to pin 2 (interrupt 0)
#define BUTTON1 3  // INT1 - Button connected to pin 3 (interrupt 1)
#define BUTTON2 21 // INT2 - Button connected to pin 21 (interrupt 2)

#define LED_INT0 8    // LED for Button 0 (interrupt)
#define LED_INT1 9    // LED for Button 1 (interrupt)
#define LED_INT2 10   // LED for Button 2 (interrupt)
#define LED_TIMER 11  // LED for Timer interrupt
#define LED_SENSOR 12 // LED for Sensor trigger
#define TRIG_PIN 6    // Trigger pin for ultrasonic sensor
#define ECHO_PIN 7    // Echo pin for ultrasonic sensor

// Flags to handle different interrupts
volatile bool timerInterruptFlag = false;
volatile bool button0InterruptFlag = false;
volatile bool button1InterruptFlag = false;
volatile bool button2InterruptFlag = false;
volatile bool sensorInterruptFlag = false;

// Variables for LED timing and blinking
unsigned long previousMillisLED0 = 0;
unsigned long previousMillisLED1 = 0;
unsigned long previousMillisLED2 = 0;
unsigned long previousMillisTimer = 0;
unsigned long previousMillisSensor = 0;

// Blink interval (in milliseconds) and debounce delay (in milliseconds)
const unsigned long blinkInterval = 100;
const unsigned long debounceDelay = 200;

// Variables for LED states and blink counts
int led0State = 0, led1State = 0, led2State = 0, sensorLedState = 0;
int blinkCount0 = 0, blinkCount1 = 0, blinkCount2 = 0, sensorBlinkCount = 0;

// Variables for managing interrupt timing and debouncing
unsigned long lastInterruptTime0 = 0;
unsigned long lastInterruptTime1 = 0;
unsigned long lastInterruptTime2 = 0;

void setup() {
    // Initialize button pins with pull-up resistors
    pinMode(BUTTON0, INPUT_PULLUP);
    pinMode(BUTTON1, INPUT_PULLUP);
    pinMode(BUTTON2, INPUT_PULLUP);
    
    // Initialize LED pins as output
    pinMode(LED_INT0, OUTPUT);
    pinMode(LED_INT1, OUTPUT);
    pinMode(LED_INT2, OUTPUT);
    pinMode(LED_TIMER, OUTPUT);
    pinMode(LED_SENSOR, OUTPUT);
    
    // Initialize ultrasonic sensor pins
    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);
    
    // Attach interrupts to buttons for falling edge detection
    attachInterrupt(digitalPinToInterrupt(BUTTON0), handleButton0, FALLING);
    attachInterrupt(digitalPinToInterrupt(BUTTON1), handleButton1, FALLING);
    attachInterrupt(digitalPinToInterrupt(BUTTON2), handleButton2, FALLING);
    
    // Set up Timer1 to trigger an interrupt every 1 second
    setupTimer1();
}

void setupTimer1() {
    noInterrupts();           // Disable interrupts temporarily
    TCCR1A = 0;               // Clear Timer/Counter Control Register A
    TCCR1B = 0;               // Clear Timer/Counter Control Register B
    TCNT1 = 0;                // Reset Timer1 counter
    OCR1A = 15624;            // Set compare value for 1 second interrupt
    TCCR1B |= (1 << WGM12);   // Set Timer mode to CTC (Clear Timer on Compare Match)
    TCCR1B |= (1 << CS12) | (1 << CS10); // Set prescaler to 1024
    TIMSK1 |= (1 << OCIE1A);  // Enable interrupt on compare match
    interrupts();             // Re-enable interrupts
}

// Timer1 compare match interrupt service routine (ISR)
ISR(TIMER1_COMPA_vect) {
    timerInterruptFlag = true;  // Set flag when Timer1 interrupt occurs
}

void handleButton0() {
    unsigned long interruptTime = millis();
    if (interruptTime - lastInterruptTime0 > debounceDelay) { // Debounce button press
        button0InterruptFlag = true;  // Set flag when Button0 is pressed
    }
    lastInterruptTime0 = interruptTime; // Update last interrupt time
}

void handleButton1() {
    unsigned long interruptTime = millis();
    if (interruptTime - lastInterruptTime1 > debounceDelay) { // Debounce button press
        button1InterruptFlag = true;  // Set flag when Button1 is pressed
    }
    lastInterruptTime1 = interruptTime; // Update last interrupt time
}

void handleButton2() {
    unsigned long interruptTime = millis();
    if (interruptTime - lastInterruptTime2 > debounceDelay) { // Debounce button press
        button2InterruptFlag = true;  // Set flag when Button2 is pressed
    }
    lastInterruptTime2 = interruptTime; // Update last interrupt time
}

void loop() {
    unsigned long currentMillis = millis(); // Get current time in milliseconds

    // Handle Timer interrupt: Blink the timer LED and trigger the sensor
    if (timerInterruptFlag) {
        timerInterruptFlag = false;
        previousMillisTimer = currentMillis;
        digitalWrite(LED_TIMER, HIGH);  // Turn on timer LED
        checkDistance();  // Check distance using ultrasonic sensor
    }
    if (currentMillis - previousMillisTimer >= blinkInterval) {
        digitalWrite(LED_TIMER, LOW);  // Turn off timer LED after blink interval
    }

    // Handle Button 0 press: Blink the LED connected to Button 0
    if (button0InterruptFlag) {
        button0InterruptFlag = false;
        blinkCount0 = 5;  // Set blink count for LED0
    }
    blinkLED(LED_INT0, previousMillisLED0, led0State, blinkCount0); // Call blink function

    // Handle Button 1 press: Blink the LED connected to Button 1
    if (button1InterruptFlag) {
        button1InterruptFlag = false;
        blinkCount1 = 5;  // Set blink count for LED1
    }
    blinkLED(LED_INT1, previousMillisLED1, led1State, blinkCount1); // Call blink function

    // Handle Button 2 press: Blink the LED connected to Button 2
    if (button2InterruptFlag) {
        button2InterruptFlag = false;
        blinkCount2 = 5;  // Set blink count for LED2
    }
    blinkLED(LED_INT2, previousMillisLED2, led2State, blinkCount2); // Call blink function

    // Handle Sensor interrupt: Blink the sensor LED
    if (sensorInterruptFlag) {
        sensorInterruptFlag = false;
        sensorBlinkCount = 5;  // Set blink count for sensor LED
    }
    blinkLED(LED_SENSOR, previousMillisSensor, sensorLedState, sensorBlinkCount); // Call blink function
}

void blinkLED(int ledPin, unsigned long &previousMillis, int &ledState, int &blinkCount) {
    if (blinkCount > 0) {
        unsigned long currentMillis = millis(); // Get current time in milliseconds
        if (currentMillis - previousMillis >= blinkInterval) {  // Check if it's time to blink
            previousMillis = currentMillis;
            ledState = !ledState;  // Toggle LED state
            digitalWrite(ledPin, ledState);  // Update LED pin
            if (ledState == LOW) {
                blinkCount--;  // Decrement blink count when LED turns off
            }
        }
    }
}

// Function to check the distance using an ultrasonic sensor
void checkDistance() {
    digitalWrite(TRIG_PIN, LOW);     // Make sure the trigger pin is low
    delayMicroseconds(2);            // Wait for a short time
    digitalWrite(TRIG_PIN, HIGH);    // Send a pulse to trigger the ultrasonic sensor
    delayMicroseconds(10);           // Wait for 10 microseconds
    digitalWrite(TRIG_PIN, LOW);     // Stop sending the pulse
    
    // Measure the pulse width of the echo signal
    long duration = pulseIn(ECHO_PIN, HIGH, 20000);  // Measure echo duration (max 20ms timeout)
    if (duration == 0) {
        duration = 20000;  // Assume max distance if no echo is received
    }
    
    // Calculate the distance in centimeters (speed of sound = 0.0343 cm/µs)
    float distance = duration * 0.0343 / 2;
    if (distance < 100) {  // If the distance is less than 100 cm, trigger the sensor LED
        sensorInterruptFlag = true;
    }
}
