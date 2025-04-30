/**
 * @file sketch.cpp
 * @author Hamin i Mlinaric
 * @brief Main code for the project
 * @version 0.95
 * @date 2025-04-27
 * 
 * @copyright Copyright (c) 2025
 */

 #include <Wire.h>
 #include <MPU6050.h>
 
 MPU6050 mpu;
 
 // --- Pin configuration ---
 const int ledLeft = 9;          /**< Yellow LED (left) */
 const int ledRight = 10;         /**< Yellow LED (right) */
 const int ledBlue = 4;           /**< Blue LED (calibration/sleep) */
 const int ledRed = 5;            /**< Red LED (temperature warning) */
 const int ledGreen = 6;          /**< Green LED (normal operation) */
 const int buttonPin = 2;         /**< Button */
 const int buzzerPin = 8;         /**< Buzzer */
 const int batteryPin = A0;       /**< Battery voltage simulation */
 const int barGraphPins[] = {3, 7, 11, 12, 13, A1, A2, A3, 0, 1}; /**< LED bar graph pins */
 const int barGraphSize = 10;     /**< Number of segments in LED bar graph */
 
 // --- Sleep mode variables ---
 bool isSleep = false;
 bool isPoweredDown = false;
 unsigned long lastMotionTime = 0;
 
 // --- Calibration offsets ---
 float accX_offset = 0;
 float accZ_offset = 0;
 
 // --- Moving average filter ---
 #define FILTER_SIZE 10
 float tiltBuffer[FILTER_SIZE];
 float runningSum = 0;
 int bufferIndex = 0;
 
 // --- Button debounce ---
 unsigned long lastButtonPressTime = 0;
 const unsigned long debounceDelay = 200;
 
 // --- Long press detection ---
 bool buttonHeld = false;
 unsigned long buttonPressedTime = 0;
 const unsigned long longPressDuration = 2000; /**< 2 seconds long press duration */
 
 // --- Blinking for turns ---
 unsigned long lastBlinkTime = 0;
 const unsigned long blinkInterval = 500; /**< Blinking interval (ms) */
 bool blinkState = false;
 
 // --- Temperature monitoring ---
 const unsigned long tempErrorDuration = 10000; /**< Temperature error timeout (ms) */
 unsigned long tempErrorStartTime = 0;
 bool tempErrorActive = false;
 
 // --- Accelerometer sleep check ---
 const unsigned long accelCheckInterval = 500; /**< Accelerometer check interval (ms) */
 const int accelThreshold = 200; /**< Acceleration threshold */
 
 // --- Serial print timer ---
 unsigned long lastPrintTime = 0;
 const unsigned long printInterval = 1000; /**< Print every 1 second */
 
 /**
  * @brief Updates the LED bar graph based on battery voltage.
  * 
  * @param voltage Current battery voltage.
  */
 void updateBatteryBarGraph(float voltage);
 
 /**
  * @brief Initializes system components (MPU6050, pins, serial, etc.).
  */
 void initializeSystem();
 
 /**
  * @brief Arduino setup function.
  */
 void setup();
 
 /**
  * @brief Arduino main loop function.
  */
 void loop();
 
 /**
  * @brief Calibrates MPU6050 by calculating offsets.
  */
 void calibrateMPU();
 
 /**
  * @brief Applies moving average filter to tilt values.
  * 
  * @param tilt Raw tilt input.
  * @return Filtered tilt value.
  */
 float filterTilt(float tilt);
 
 /**
  * @brief Puts the system into sleep mode.
  */
 void enterSleepMode();
 
 /**
  * @brief Checks for a long press on the button to power off manually.
  */
 void checkLongPress();
 
 /**
  * @brief Powers down the system permanently (manual or temperature error).
  */
 void powerDownForever();
 
 // --- Implementations ---
 
 void updateBatteryBarGraph(float voltage) {
   int batteryPercentage = (voltage / 5.0) * 100;
   batteryPercentage = constrain(batteryPercentage, 0, 100);
   int segmentsToLight = batteryPercentage / 10;
   if (batteryPercentage > 0 && segmentsToLight == 0) segmentsToLight = 1;
 
   for (int i = 0; i < barGraphSize; i++) {
     digitalWrite(barGraphPins[i], i < segmentsToLight ? HIGH : LOW);
   }
 
   if (millis() - lastPrintTime >= printInterval) {
     Serial.print("Battery Voltage: "); Serial.print(voltage, 2);
     Serial.print("V | Percentage: "); Serial.print(batteryPercentage);
     Serial.print("% | Segments: "); Serial.println(segmentsToLight);
   }
 }
 
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
 
   for (int i = 0; i < barGraphSize; i++) {
     pinMode(barGraphPins[i], OUTPUT);
     digitalWrite(barGraphPins[i], LOW);
   }
 
   Serial.println("Testing LED bar graph...");
   for (int i = 0; i < barGraphSize; i++) {
     digitalWrite(barGraphPins[i], HIGH);
     delay(200);
     digitalWrite(barGraphPins[i], LOW);
   }
   Serial.println("Test complete.");
 
   for (int i = 0; i < FILTER_SIZE; i++) {
     tiltBuffer[i] = 0;
   }
 
   if (!mpu.testConnection()) {
     digitalWrite(ledRed, HIGH);
     Serial.println("MPU6050 connection error!");
     while (1);
   }
 
   digitalWrite(ledBlue, HIGH);
   calibrateMPU();
   digitalWrite(ledBlue, LOW);
   digitalWrite(ledGreen, HIGH);
 
   lastMotionTime = millis();
 }
 
 void setup() {
   initializeSystem();
 }
 
 void loop() {
   // Main operational logic
 }
 
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
 
 float filterTilt(float tilt) {
   runningSum -= tiltBuffer[bufferIndex];
   tiltBuffer[bufferIndex] = tilt;
   runningSum += tilt;
   bufferIndex = (bufferIndex + 1) % FILTER_SIZE;
   return runningSum / FILTER_SIZE;
 }
 
 void enterSleepMode() {
   Serial.println(">>> Entering sleep mode <<<");
   Serial.flush();
   Serial.end();
 
   isSleep = true;
   digitalWrite(ledGreen, LOW);
   digitalWrite(ledBlue, HIGH);
   digitalWrite(ledLeft, LOW);
   digitalWrite(ledRight, LOW);
   for (int i = 0; i < barGraphSize; i++) {
     digitalWrite(barGraphPins[i], LOW);
   }
 
   delay(50);
   while (digitalRead(buttonPin) == LOW);
   delay(50);
 
   mpu.setRate(7);
 }
 
 void checkLongPress() {
   static bool lastButtonState = HIGH;
   bool currentState = digitalRead(buttonPin);
 
   if (lastButtonState == HIGH && currentState == LOW) {
     buttonPressedTime = millis();
     buttonHeld = true;
   } else if (lastButtonState == LOW && currentState == HIGH) {
     unsigned long pressDuration = millis() - buttonPressedTime;
     buttonHeld = false;
 
     if (pressDuration >= longPressDuration) {
       Serial.println(">>> Manual shutdown <<<");
       tone(buzzerPin, 1000, 500);
       delay(600);
       powerDownForever();
     } else {
       Serial.println("Short press detected, no action taken.");
     }
   }
 
   lastButtonState = currentState;
 }
 
 void powerDownForever() {
   Serial.println(">>> Shutting down <<<");
   Serial.flush();
   digitalWrite(ledRed, HIGH);
   digitalWrite(ledGreen, LOW);
   digitalWrite(ledLeft, LOW);
   digitalWrite(ledRight, LOW);
   digitalWrite(ledBlue, LOW);
   for (int i = 0; i < barGraphSize; i++) {
     digitalWrite(barGraphPins[i], LOW);
   }
 
   mpu.setSleepEnabled(true);
   isPoweredDown = true;
 }
 
