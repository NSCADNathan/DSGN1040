// Blink - ESP32 Wrover
// Original pins 11,12,13 → remapped to 32,33,25

#define PIN_A 25
#define PIN_B 32
#define PIN_C 33

void setup() {
  pinMode(PIN_A, OUTPUT);
  pinMode(PIN_B, OUTPUT);
  pinMode(PIN_C, OUTPUT);
}

void loop() {
  digitalWrite(PIN_B, HIGH);
  digitalWrite(PIN_C, HIGH);
  delay(100);
  digitalWrite(PIN_B, LOW);
  digitalWrite(PIN_C, LOW);
  delay(100);

  digitalWrite(PIN_A, HIGH);
  digitalWrite(PIN_B, HIGH);
  digitalWrite(PIN_C, HIGH);
  delay(100);
  digitalWrite(PIN_B, LOW);
  digitalWrite(PIN_C, LOW);
  delay(100);

  digitalWrite(PIN_B, HIGH);
  digitalWrite(PIN_C, HIGH);
  delay(100);
  digitalWrite(PIN_B, LOW);
  digitalWrite(PIN_C, LOW);
  delay(100);

  digitalWrite(PIN_A, LOW);
  digitalWrite(PIN_B, HIGH);
  digitalWrite(PIN_C, HIGH);
  delay(100);
  digitalWrite(PIN_B, LOW);
  digitalWrite(PIN_C, LOW);
  delay(100);
}
