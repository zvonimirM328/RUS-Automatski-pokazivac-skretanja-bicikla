#include <Wire.h>
#include <MPU6050.h>

MPU6050 mpu;

// Pinovi
const int ledLeft = 9;          // Žuta LED (lijevo)
const int ledRight = 10;       // Žuta LED (desno)
const int ledBlue = 4;         // Plava LED (kalibracija/sleep)
const int ledRed = 5;          // Crvena LED (temperatura)
const int ledGreen = 6;        // Zelena LED (normalno stanje)
const int buttonPin = 2;       // Gumb
const int buzzerPin = 8;       // Buzzer
const int batteryPin = A0;     // Simulacija napona baterije
const int barGraphPins[] = {3, 7, 11, 12, 13, A1, A2, A3, 0, 1}; // 10 pinova za LED bar graph
const int barGraphSize = 10;   // Broj segmenata

// Sleep varijable
bool isSleep = false;
bool isPoweredDown = false;
unsigned long lastMotionTime = 0;

// Kalibracija
float accX_offset = 0;
float accZ_offset = 0;

// Filter
#define FILTER_SIZE 10
float tiltBuffer[FILTER_SIZE];
float runningSum = 0;
int bufferIndex = 0;

// Debounce za gumb
unsigned long lastButtonPressTime = 0;
const unsigned long debounceDelay = 200;

// Dug pritisak
bool buttonHeld = false;
unsigned long buttonPressedTime = 0;
const unsigned long longPressDuration = 2000; // 2 sekunde

// Blinkanje za skretanje
unsigned long lastBlinkTime = 0;
const unsigned long blinkInterval = 500; // Treptanje svakih 500ms
bool blinkState = false;

// Temperatura
const unsigned long tempErrorDuration = 10000; // 10 sekundi za previsoku/prenisku temperaturu
unsigned long tempErrorStartTime = 0;
bool tempErrorActive = false;

// Sleep mod za akceleraciju
const unsigned long accelCheckInterval = 500; // Provjera akceleracije svakih 500ms
const int accelThreshold = 200; // Prag za značajnu promjenu akceleracije

// Timer za ispis
unsigned long lastPrintTime = 0;
const unsigned long printInterval = 1000; // Ispis svake 1 sekunde

// --- Ažuriranje LED bar grapha ---
void updateBatteryBarGraph(float voltage) {
  // Mapiranje napona (0-5V) na postotak (0-100%)
  int batteryPercentage = (voltage / 5.0) * 100;
  batteryPercentage = constrain(batteryPercentage, 0, 100);

  // Izračun broja segmenata za paljenje (1 segment = 10%)
  int segmentsToLight = batteryPercentage / 10;
  if (batteryPercentage > 0 && segmentsToLight == 0) segmentsToLight = 1; // Osiguraj minimalno 1 segment ako nije 0%

  // Upali odgovarajuće segmente
  for (int i = 0; i < barGraphSize; i++) {
    digitalWrite(barGraphPins[i], i < segmentsToLight ? HIGH : LOW);
  }

  // Ispis samo svake sekunde
  if (millis() - lastPrintTime >= printInterval) {
    Serial.print("Battery Voltage: "); Serial.print(voltage, 2);
    Serial.print("V | Percentage: "); Serial.print(batteryPercentage);
    Serial.print("% | Segments: "); Serial.println(segmentsToLight);
  }
}

// --- Inicijalizacija sustava ---
void initializeSystem() {
  Serial.begin(9600);
  Wire.begin();
  mpu.initialize();

  pinMode(ledLeft, OUTPUT);
  pinMode(ledRight, OUTPUT);
  pinMode(ledBlue, OUTPUT);
  pinMode(ledRed, OUTPUT);
  pinMode(ledGreen, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(buzzerPin, OUTPUT);
  pinMode(batteryPin, INPUT);

  // Inicijalizacija pinova za LED bar graph
  for (int i = 0; i < barGraphSize; i++) {
    pinMode(barGraphPins[i], OUTPUT);
    digitalWrite(barGraphPins[i], LOW); // Osiguraj da su svi segmenti ugašeni
  }

  // Test LED bar grapha
  Serial.println("Testing LED bar graph...");
  for (int i = 0; i < barGraphSize; i++) {
    digitalWrite(barGraphPins[i], HIGH);
    delay(200);
    digitalWrite(barGraphPins[i], LOW);
  }
  Serial.println("Test complete.");

  // Inicijalizacija filtra
  for (int i = 0; i < FILTER_SIZE; i++) {
    tiltBuffer[i] = 0;
  }

  if (!mpu.testConnection()) {
    digitalWrite(ledRed, HIGH);
    Serial.println("MPU6050 greška!");
    while (1); // Fatalna greška
  }

  // LED signalizacija kalibracije
  digitalWrite(ledBlue, HIGH);
  calibrateMPU();
  digitalWrite(ledBlue, LOW);

  digitalWrite(ledGreen, HIGH); // Normalno stanje
  lastMotionTime = millis();
}

void setup() {
  initializeSystem();
}

void loop() {
  if (isPoweredDown) {
    // Ako je isključeno, čekaj pritisak gumba
    if (digitalRead(buttonPin) == LOW) {
      unsigned long currentMillis = millis();
      if (currentMillis - lastButtonPressTime > debounceDelay) {
        Serial.println(">>> Ponovno paljenje <<<");
        isPoweredDown = false;
        initializeSystem(); // Ponovna inicijalizacija
        lastButtonPressTime = currentMillis;
      }
    }
    delay(100); // Debounce i simulacija isključenog stanja
    return; // Preskoči ostatak loopa
  }

  if (isSleep) {
    // Simulacija sleep moda u Wokwi-ju
    while (isSleep) {
      // Provjera gumba
      if (digitalRead(buttonPin) == LOW) {
        unsigned long currentMillis = millis();
        if (currentMillis - lastButtonPressTime > debounceDelay) {
          isSleep = false;
          mpu.setSleepEnabled(false);
          mpu.setRate(0); // Vraća normalnu frekvenciju
          Serial.begin(9600);
          Serial.println(">>> Buđenje iz sleep moda <<<");
          digitalWrite(ledBlue, LOW);
          digitalWrite(ledGreen, HIGH);
          lastMotionTime = millis();
          lastButtonPressTime = currentMillis;
          break;
        }
      }

      // Provjera akceleracije
      mpu.setSleepEnabled(false); // Aktivira MPU6050
      int16_t ax, ay, az;
      mpu.getAcceleration(&ax, &ay, &az);
      mpu.setSleepEnabled(true); // Vraća MPU6050 u sleep

      if (abs(ax) > accelThreshold || abs(ay) > accelThreshold || abs(az) > accelThreshold) {
        isSleep = false;
        mpu.setSleepEnabled(false);
        mpu.setRate(0); // Vraća normalnu frekvenciju
        Serial.begin(9600);
        Serial.println(">>> Buđenje iz sleep moda <<<");
        digitalWrite(ledBlue, LOW);
        digitalWrite(ledGreen, HIGH);
        lastMotionTime = millis();
        break;
      }

      // Ostani u sleep modu
      delay(accelCheckInterval); // Simulacija čekanja
    }
    return;
  }

  int16_t ax, ay, az, gx, gy, gz, tempRaw;
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
  tempRaw = mpu.getTemperature();

  float accX = (ax - accX_offset) / 16384.0;
  float accZ = (az - accZ_offset) / 16384.0;
  float rotZ = gz / 131.0;
  float tempC = tempRaw / 340.0 + 36.53;

  // TEMP ZAŠTITA
  if (tempC < 0 || tempC > 60) {
    if (!tempErrorActive) {
      tempErrorActive = true;
      tempErrorStartTime = millis();
    }
    digitalWrite(ledRed, (millis() % 500) < 250 ? HIGH : LOW); // Treptanje upozorenja
    if (millis() - tempErrorStartTime >= tempErrorDuration) {
      powerDownForever();
      return;
    }
  } else {
    tempErrorActive = false;
    digitalWrite(ledRed, LOW);
  }

  // KOČENJE
  if (accZ < 0.4) {
    digitalWrite(ledLeft, HIGH);
    digitalWrite(ledRight, HIGH);
  } else {
    float filteredTiltX = filterTilt(accX);

    // SKRETANJE (blinkanje)
    if (millis() - lastBlinkTime >= blinkInterval) {
      blinkState = !blinkState;
      lastBlinkTime = millis();
    }

    if (filteredTiltX > 0.5) {
      digitalWrite(ledRight, blinkState ? HIGH : LOW); // Desno skretanje
      digitalWrite(ledLeft, LOW);
    } else if (filteredTiltX < -0.5) {
      digitalWrite(ledLeft, blinkState ? HIGH : LOW); // Lijevo skretanje
      digitalWrite(ledRight, LOW);
    } else {
      digitalWrite(ledLeft, LOW);
      digitalWrite(ledRight, LOW);
    }
  }

  // BATERIJA (LED bar graph)
  float batteryVoltage = analogRead(batteryPin) * (5.0 / 1023.0); // Simulacija napona
  updateBatteryBarGraph(batteryVoltage);

  // SERIJSKI NADZOR (TiltX i Temp samo svake sekunde)
  if (millis() - lastPrintTime >= printInterval) {
    Serial.print("TiltX: "); Serial.print(accX, 2);
    Serial.print(" | Temp: "); Serial.print(tempC, 1);
    Serial.println();
    lastPrintTime = millis();
  }

  // DETEKCIJA POKRETA
  if (abs(rotZ) > 1 || abs(ax) > accelThreshold || abs(ay) > accelThreshold) {
    lastMotionTime = millis();
  }

  // SLEEP NAKON 10 SEKUNDI
  if (millis() - lastMotionTime > 120000) {
    enterSleepMode();
  }

  // Provjera dugog pritiska gumba
  checkLongPress();

  delay(100);
}

// --- Kalibracija MPU ---
void calibrateMPU() {
  long sumAx = 0, sumAz = 0;
  const int samples = 100;

  for (int i = 0; i < samples; i++) {
    int16_t ax, ay, az;
    mpu.getAcceleration(&ax, &ay, &az);
    sumAx += ax;
    sumAz += az;
    delay(5);
  }

  accX_offset = sumAx / (float)samples;
  accZ_offset = (sumAz / (float)samples) - 16384.0;
}

// --- Moving Average Filter ---
float filterTilt(float tilt) {
  runningSum -= tiltBuffer[bufferIndex];
  tiltBuffer[bufferIndex] = tilt;
  runningSum += tilt;
  bufferIndex = (bufferIndex + 1) % FILTER_SIZE;
  return runningSum / FILTER_SIZE;
}

// --- Sleep Mode ---
void enterSleepMode() {
  Serial.println(">>> Sleep mode <<<");
  Serial.flush();
  Serial.end();

  isSleep = true;
  digitalWrite(ledGreen, LOW);
  digitalWrite(ledBlue, HIGH);
  digitalWrite(ledLeft, LOW);
  digitalWrite(ledRight, LOW);
  for (int i = 0; i < barGraphSize; i++) {
    digitalWrite(barGraphPins[i], LOW); // Gasi LED bar graph
  }

  delay(50);
  while (digitalRead(buttonPin) == LOW); // Čekaj otpuštanje gumba
  delay(50);

  mpu.setRate(7); // Niskofrekventni mod
}

// --- Dug pritisak gumba ---
void checkLongPress() {
  static bool lastButtonState = HIGH;
  bool currentState = digitalRead(buttonPin);

  if (lastButtonState == HIGH && currentState == LOW) {
    buttonPressedTime = millis();
    buttonHeld = true;
  } else if (lastButtonState == LOW && currentState == HIGH) {
    // Gumb pušten
    unsigned long pressDuration = millis() - buttonPressedTime;
    buttonHeld = false;

    if (pressDuration >= longPressDuration) {
      Serial.println(">>> Ručno gašenje <<<");
      // Aktivacija buzzera
      tone(buzzerPin, 1000, 500); // 1kHz zvuk na 500ms
      delay(600); // Čekanje da buzzer završi
      powerDownForever(); // Ulazi u isključeno stanje
    } else {
      Serial.println("Kratki pritisak - ništa se ne mijenja.");
    }
  }

  lastButtonState = currentState;
}

// --- Isključenje (Temp Error ili ručno) ---
void powerDownForever() {
  Serial.println(">>> Gašenje... <<<");
  Serial.flush();
  digitalWrite(ledRed, HIGH);
  digitalWrite(ledGreen, LOW);
  digitalWrite(ledLeft, LOW);
  digitalWrite(ledRight, LOW);
  digitalWrite(ledBlue, LOW);
  for (int i = 0; i < barGraphSize; i++) {
    digitalWrite(barGraphPins[i], LOW); // Gasi LED bar graph
  }

  mpu.setSleepEnabled(true);
  isPoweredDown = true; // Ulazi u isključeno stanje
}
