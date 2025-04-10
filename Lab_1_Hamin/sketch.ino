#define BUTTON_HIGH 2
#define BUTTON_MED 3
#define BUTTON_LOW 4
#define LED_HIGH 5
#define LED_MED 6
#define LED_LOW 7
#define TRIG_PIN 9
#define ECHO_PIN 10
#define LED_DISTANCE 11

volatile bool highPressed = false;
volatile bool medPressed = false;
volatile bool timerFlag = false;

void setupTimer() {
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

void highPriorityISR() { highPressed = true; }
void medPriorityISR() { medPressed = true; }
ISR(TIMER1_COMPA_vect) { timerFlag = true; }

long measureDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  long duration = pulseIn(ECHO_PIN, HIGH);
  return duration * 0.034 / 2;
}

void setup() {
  pinMode(BUTTON_HIGH, INPUT_PULLUP);
  pinMode(BUTTON_MED, INPUT_PULLUP);
  pinMode(BUTTON_LOW, INPUT_PULLUP);
  pinMode(LED_HIGH, OUTPUT);
  pinMode(LED_MED, OUTPUT);
  pinMode(LED_LOW, OUTPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(LED_DISTANCE, OUTPUT);

  attachInterrupt(digitalPinToInterrupt(BUTTON_HIGH), highPriorityISR, FALLING);
  attachInterrupt(digitalPinToInterrupt(BUTTON_MED), medPriorityISR, FALLING);

  setupTimer();
  Serial.begin(9600);
}

void loop() {
  bool highState = digitalRead(BUTTON_HIGH) == LOW;
  bool medState = digitalRead(BUTTON_MED) == LOW;
  bool lowState = digitalRead(BUTTON_LOW) == LOW;

  static bool lastMedState = HIGH;
  static bool lastLowState = HIGH;

  if (highState) {
    digitalWrite(LED_HIGH, !digitalRead(LED_HIGH));
    digitalWrite(LED_MED, LOW);
    digitalWrite(LED_LOW, LOW);
    Serial.println("Visoki prioritet aktivan - svi niži ugašeni!");
    highPressed = false;
  }
  else if (medState) {
    digitalWrite(LED_MED, !digitalRead(LED_MED));
    digitalWrite(LED_LOW, LOW);
    Serial.println("Srednji prioritet aktivan - niski ugašen!");
    medPressed = false;
  }
  else if (lowState && !medState && !highState) {
    digitalWrite(LED_LOW, !digitalRead(LED_LOW));
    Serial.println("Niski prioritet aktivan!");
  }

  if (medState && lowState && lastLowState != lowState) {
    Serial.println("Nije moguće uključiti niski prioritet jer je srednji prioritet uključen!");
  }
  if (highState && medState && lastMedState != medState) {
    Serial.println("Nije moguće uključiti srednji prioritet jer je visoki prioritet uključen!");
  }
  if (highState && lowState && lastLowState != lowState) {
    Serial.println("Nije moguće uključiti niski prioritet jer je visoki prioritet uključen!");
  }

  lastMedState = medState;
  lastLowState = lowState;

  if (timerFlag) {
    Serial.println("Tajmer: 1 sekunda!");
    timerFlag = false;
  }

  long distance = measureDistance();
  if (distance < 10) {
    digitalWrite(LED_DISTANCE, HIGH);
    Serial.print("Udaljenost: ");
    Serial.print(distance);
    Serial.println(" cm - LED uključen!");
  } else {
    digitalWrite(LED_DISTANCE, LOW);
  }

  delay(200);
}
