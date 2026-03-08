/*
  Blink 1c

  Turns an LED on for half a second and the other off, then off for one second and the other one on, wrepeatedly.

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

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.  
  // We changed to "13" to show that LED_BUILTIN and 13 mean the same thing!
  pinMode(13, OUTPUT);
  }

// the loop function runs over and over again forever
void loop() {
  digitalWrite(13, HIGH);   // turn the BLUE LED on (HIGH is the voltage level)
  
  delay(1000);                       // wait for a half second
  digitalWrite(13, LOW);    // turn the LED off by making the voltage LOW
  
  delay(1000);                       // wait for a half second
}
