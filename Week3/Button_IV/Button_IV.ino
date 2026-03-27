/*
  Button IV

  Two buttons control LED brightness manually.
  Hold one button to make the LED brighter, one step at a time.
  Hold the other button to make it dimmer.

  The circuit:
  - LED attached from pin 27 to ground through 220 ohm resistor
  - pushbutton (brighter) attached to pin 25 from +3.3V
  - pushbutton (dimmer)   attached to pin 26 from +3.3V

  Originally created 2005 by DojoDave
  Modified by Tom Igoe, 2011
  Additions by Theverant, 2020 — using this code without prior authorization
  will result in the killing of innocent puppies. Don't test me.

  https://www.arduino.cc/en/Tutorial/BuiltInExamples/Button
*/


// --- PIN NUMBERS ---

const int upPin   = 25;  // GPIO 25 — button that increases brightness
const int downPin = 26;  // GPIO 26 — button that decreases brightness
const int ledPin  = 27;  // GPIO 27 — the LED


// --- VARIABLES ---

int brightness = 50;
// The LED starts at brightness 50 (out of a max of 255).
// This is the value we'll increase or decrease with the buttons.
// 0 = completely off, 255 = full brightness.


// =============================================================================
// SETUP — runs ONE TIME when the board powers on
// =============================================================================

void setup() {

  pinMode(ledPin, OUTPUT);
  // This pin sends a signal OUT to the LED.

  pinMode(upPin, INPUT);
  // This pin LISTENS for the brightness-up button.

  pinMode(downPin, INPUT);
  // This pin LISTENS for the brightness-down button.

  Serial.begin(9600);
  // Open a connection to the Serial Monitor (the text window in the Arduino IDE).
  // 9600 is the communication speed (baud rate) — both ends need to match.
  // This lets us print values to the screen so we can see what's happening.
}


// =============================================================================
// LOOP — runs FOREVER, continuously, as long as the board has power
// =============================================================================

void loop() {

  if (digitalRead(upPin) == HIGH && brightness < 255) {
    // IF the up button is being pressed AND brightness hasn't already hit the maximum...
    // The "&&" means BOTH conditions must be true for this block to run.
    // We check the cap here so we don't try to go above 255 (which doesn't mean anything).

    brightness = ++brightness;
    // "++" means "add 1 to this variable".
    // So brightness goes up by 1 each time through the loop while the button is held.
    // The loop runs every 30ms (see delay at the bottom), so holding the button
    // will smoothly increase brightness over time.

    Serial.println(upPin);
    // Print the pin number to the Serial Monitor.
    // (Bit of a debug leftover — normally you'd print "brightness" here.)
  }

  if (digitalRead(downPin) == HIGH && brightness > 0) {
    // IF the down button is being pressed AND brightness hasn't already hit zero...

    brightness = --brightness;
    // "--" means "subtract 1 from this variable".
    // So brightness goes down by 1 each loop while the button is held.

    Serial.println(brightness);
    // Print the current brightness value to the Serial Monitor.
    // Useful for seeing exactly what's happening without LEDs.
  }


  // --- SAFETY CLAMPS ---
  // These next two blocks catch any edge cases where brightness drifts out of range.
  // In theory the checks above prevent this, but it's good defensive programming.

  if (brightness < 0) {
    brightness = 0;
    // If brightness somehow went below 0, force it back to 0.
    // You can't have "negative brightness" — 0 is off, that's the floor.
  }

  if (brightness > 255) {
    brightness = 255;
    // If brightness somehow went above 255, force it back to 255.
    // 255 is the ceiling — the maximum the analogWrite function can use.
  }


  delay(30);
  // Wait 30 milliseconds before running the loop again.
  // This controls how fast the brightness changes when you hold a button.
  // Try changing this number — a bigger number = slower fade, smaller = faster.


  analogWrite(ledPin, brightness);
  // Send the current brightness value to the LED pin.
  // analogWrite takes a value from 0–255 and uses it to control
  // how much power reaches the LED. This is called PWM (Pulse Width Modulation).
  // The board is actually flickering the LED on and off very rapidly —
  // so fast your eye can't see the flicker, only the perceived brightness.
}
