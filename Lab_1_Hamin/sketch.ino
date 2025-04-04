#define BUTTON_HIGH 2    // Visoki prioritet (pin s prekidom)
#define BUTTON_MED 3     // Srednji prioritet (pin s prekidom)
#define BUTTON_LOW 4     // Niski prioritet (bez prekida, provjera u loop-u)
#define LED_HIGH 5       // LED za visoki prioritet
#define LED_MED 6        // LED za srednji prioritet
#define LED_LOW 7        // LED za niski prioritet
#define TRIG_PIN 9       // HC-SR04 Trig pin
#define ECHO_PIN 10      // HC-SR04 Echo pin
#define LED_DISTANCE 11  // LED za senzor udaljenosti

volatile bool highPressed = false;
volatile bool medPressed = false;
volatile bool timerFlag = false;

void setupTimer() {
  noInterrupts(); // Isključi prekide tijekom postavljanja
  TCCR1A = 0;     // Normalni način rada
  TCCR1B = 0;     // Zaustavi tajmer
  TCNT1 = 0;      // Resetiraj brojač
  OCR1A = 15624;  // 1 sekunda na 16MHz s prescalerom 1024
  TCCR1B |= (1 << WGM12); // CTC mod
  TCCR1B |= (1 << CS12) | (1 << CS10); // Prescaler 1024
  TIMSK1 |= (1 << OCIE1A); // Omogući prekid na podudaranje
  interrupts();   // Uključi prekide
}

void highPriorityISR() {
  highPressed = true;
}

void medPriorityISR() {
  medPressed = true;
}

ISR(TIMER1_COMPA_vect) {
  timerFlag = true;
}

long measureDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  long duration = pulseIn(ECHO_PIN, HIGH);
  return duration * 0.034 / 2; // Udaljenost u cm
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
  // Obrada visokog prioriteta
  if (highPressed) {
    digitalWrite(LED_HIGH, !digitalRead(LED_HIGH));
    highPressed = false;
    Serial.println("Visoki prioritet aktiviran!");
  }

  // Obrada srednjeg prioriteta
  if (medPressed) {
    digitalWrite(LED_MED, !digitalRead(LED_MED));
    medPressed = false;
    Serial.println("Srednji prioritet aktiviran!");
  }

  // Obrada niskog prioriteta (bez prekida)
  if (digitalRead(BUTTON_LOW) == LOW) {
    digitalWrite(LED_LOW, !digitalRead(LED_LOW));
    delay(200); // Debouncing
    Serial.println("Niski prioritet aktiviran!");
  }

  // Obrada tajmera
  if (timerFlag) {
    Serial.println("Tajmer: 1 sekunda!");
    timerFlag = false;
  }

  // Mjerenje udaljenosti
  long distance = measureDistance();
  if (distance < 10) {
    digitalWrite(LED_DISTANCE, HIGH);
    Serial.print("Udaljenost: ");
    Serial.print(distance);
    Serial.println(" cm - LED uključen!");
  } else {
    digitalWrite(LED_DISTANCE, LOW);
  }

  delay(100); 
}

