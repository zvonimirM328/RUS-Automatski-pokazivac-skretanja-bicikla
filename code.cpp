#include <Wire.h>
#include <MPU6050.h>

MPU6050 mpu;

const int ledLeft = 9;
const int ledRight = 10;

void setup() {
  Serial.begin(9600);
  Wire.begin();
  mpu.initialize();

  pinMode(ledLeft, OUTPUT);
  pinMode(ledRight, OUTPUT);

  if (!mpu.testConnection()) {
    Serial.println("MPU6050 nije povezan!");
    while (1);
  }
}

void loop() {
  int16_t ax, ay, az;
  mpu.getAcceleration(&ax, &ay, &az);

  // Ugao nagiba po X osi (lijevo-desno)
  float tiltX = ax / 16384.0;

  if (tiltX > 0.3) {
    // Nagib udesno
    digitalWrite(ledRight, HIGH);
    digitalWrite(ledLeft, LOW);
  } else if (tiltX < -0.3) {
    // Nagib ulevo
    digitalWrite(ledLeft, HIGH);
    digitalWrite(ledRight, LOW);
  } else {
    // Ravno
    digitalWrite(ledLeft, LOW);
    digitalWrite(ledRight, LOW);
  }

  delay(200);
}
