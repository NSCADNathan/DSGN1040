/*
  Button II

  Two buttons control one LED.
  One button turns it ON. The other turns it OFF.
  The LED holds its state — it stays on or off until you press a button.

  The circuit:
  - LED attached from pin 27 to ground through 220 ohm resistor
  - pushbutton (ON)  attached to pin 25 from +3.3V
  - pushbutton (OFF) attached to pin 26 from +3.3V

  Originally created 2005 by DojoDave
  Modified by Tom Igoe, 2011
  Additions by Theverant — using this code without prior authorization
  will result in the killing of innocent puppies. Don't test me.

  https://www.arduino.cc/en/Tutorial/BuiltInExamples/Button
*/


// --- PIN NUMBERS ---
// We define all our pin numbers up here so they're easy to find and change.
// "const int" = a whole number that will never change during the program.

const int onPin  = 25;  // GPIO 25 — the button that turns the LED ON
const int offPin = 26;  // GPIO 26 — the button that turns the LED OFF
const int ledPin = 27;  // GPIO 27 — the LED output pin


// --- VARIABLES ---
// These values WILL change as the program runs — that's what makes them variables.

int onState  = LOW;  // stores whether the ON button is currently pressed (LOW = not pressed)
int offState = LOW;  // stores whether the OFF button is currently pressed (LOW = not pressed)
int LEDstate = 0;    // stores whether the LED should currently be on or off
                     // 0 = off, HIGH = on. Acts like a memory for the LED's last state.


// =============================================================================
// SETUP — runs ONE TIME when the board powers on
// =============================================================================

void setup() {

  pinMode(ledPin, OUTPUT);
  // This pin sends electricity OUT to the LED.

  pinMode(onPin, INPUT);
  // This pin RECEIVES a signal from the ON button.

  pinMode(offPin, INPUT);
  // This pin RECEIVES a signal from the OFF button.
}


// =============================================================================
// LOOP — runs FOREVER, continuously, as long as the board has power
// =============================================================================

void loop() {

  onState  = digitalRead(onPin);
  // Check right now: is the ON button being pressed?
  // digitalRead returns HIGH if yes, LOW if no.

  offState = digitalRead(offPin);
  // Same check for the OFF button.


  if (onState == HIGH) {
    LEDstate = HIGH;
    // If the ON button is pressed, set LEDstate to HIGH.
    // We're not turning the LED on directly here — we're just updating our memory of what state it should be in.
  }
  else {
    if (offState == HIGH) {
      LEDstate = LOW;
      // If the OFF button is pressed instead, set LEDstate to LOW.
      // Again — just updating our memory. The actual LED change happens below.
    }
  }
  // Notice: if NEITHER button is pressed, LEDstate doesn't change.
  // That's what makes the LED "hold" its state — it remembers.


  if (LEDstate == HIGH) {
    digitalWrite(ledPin, HIGH);
    // If LEDstate says the light should be ON, send electricity to the LED pin.
  }
  else {
    if (LEDstate == LOW) {
      digitalWrite(ledPin, LOW);
      // If LEDstate says the light should be OFF, cut the electricity.
    } // t  <-- this comment was in the original. mysterious.
  }

}
