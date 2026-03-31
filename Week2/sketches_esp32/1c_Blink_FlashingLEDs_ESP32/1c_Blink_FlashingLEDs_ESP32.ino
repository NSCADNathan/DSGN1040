/*
  Blink 1c

  Turns an LED on for half a second and the other off, then off for one second and the other one on, repeatedly.  These can be any colour.

  modified 8 May 2014
  by Scott Fitzgerald
  modified 2 Sep 2016
  by Arturo Guadalupi
  modified 8 Sep 2016
  by Colby Newman
  modified 2022
  by Theverant

  This example code is in the public domain.

  original:
  https://www.arduino.cc/en/Tutorial/BuiltInExamples/Blink
*/

// pins 13→12, 12→13

#define LED_A 12
#define LED_B 13

void setup() {
  pinMode(LED_A, OUTPUT);
  pinMode(LED_B, OUTPUT);
}

void loop() {
  digitalWrite(LED_A, HIGH);  // turn LED_A on
  digitalWrite(LED_B, LOW);   // turn LED_B off
  delay(500);
  digitalWrite(LED_A, LOW);   // turn LED_A off
  digitalWrite(LED_B, HIGH);  // turn LED_B on
  delay(500);
}
