/*
  Blink

  Turns an LED on for one second, then off for one second, repeatedly.

  Most Arduinos have an on-board LED you can control. On the UNO, MEGA and ZERO
  it is attached to digital pin 13, on MKR1000 on pin 6. LED_BUILTIN is set to
  the correct LED pin independent of which board is used.
  If you want to know what pin the on-board LED is connected to on your Arduino
  model, check the Technical Specs of your board at:
  https://www.arduino.cc/en/Main/Products

  modified 8 May 2014
  by Scott Fitzgerald
  modified 2 Sep 2016
  by Arturo Guadalupi
  modified 8 Sep 2016
  by Colby Newman

  This example code is in the public domain.

  https://www.arduino.cc/en/Tutorial/BuiltInExamples/Blink
*/

// pins 11→27, 12→13, 13→12

#define PIN_A 12
#define PIN_B 13
#define PIN_C 27

void setup() {
  pinMode(PIN_A, OUTPUT);
  pinMode(PIN_B, OUTPUT);
  pinMode(PIN_C, OUTPUT);
}

void loop() {
  digitalWrite(PIN_B, HIGH);
  digitalWrite(PIN_A, HIGH);
  delay(100);
  digitalWrite(PIN_B, LOW);
  digitalWrite(PIN_A, LOW);
  delay(100);

  digitalWrite(PIN_C, HIGH);
  digitalWrite(PIN_B, HIGH);
  digitalWrite(PIN_A, HIGH);
  delay(100);
  digitalWrite(PIN_B, LOW);
  digitalWrite(PIN_A, LOW);
  delay(100);

  digitalWrite(PIN_B, HIGH);
  digitalWrite(PIN_A, HIGH);
  delay(100);
  digitalWrite(PIN_B, LOW);
  digitalWrite(PIN_A, LOW);
  delay(100);

  digitalWrite(PIN_C, LOW);
  digitalWrite(PIN_B, HIGH);
  digitalWrite(PIN_A, HIGH);
  delay(100);
  digitalWrite(PIN_B, LOW);
  digitalWrite(PIN_A, LOW);
  delay(100);
}
