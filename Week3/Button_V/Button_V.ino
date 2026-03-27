/*
  Button V

  Two buttons control LED brightness in larger steps.
  One button jumps brightness UP by a defined step amount.
  One button jumps brightness DOWN by the same step amount.
  The step size is set by "brightStep" — change it to make bigger or smaller jumps.

  The circuit:
  - LED attached from pin 27 to ground through 220 ohm resistor
  - pushbutton (up)   attached to pin 25 from +3.3V
  - pushbutton (down) attached to pin 26 from +3.3V

  Theverant, 2022.
  Built on Button and Fade examples.

  https://www.arduino.cc/en/Tutorial/BuiltInExamples/Button
*/


// --- PIN NUMBERS ---

const int upPin   = 25;  // GPIO 25 — button that increases brightness
const int downPin = 26;  // GPIO 26 — button that decreases brightness
const int ledPin  = 27;  // GPIO 27 — the LED


// --- VARIABLES ---

int brightness = 50;
// The LED starts at brightness 50 (out of 255 max).
// This is the current brightness level. It changes when buttons are pressed.

int brightStep = 5;
// This is how much brightness changes each time you press a button.
// Unlike Button IV which changes by 1 per loop, this jumps by "brightStep" at once.
// Try changing this number — 50 would give you big dramatic jumps, 1 would be very fine control.


// =============================================================================
// SETUP — runs ONE TIME when the board powers on
// =============================================================================

void setup() {

  pinMode(ledPin, OUTPUT);
  // This pin sends a signal OUT to the LED.

  pinMode(upPin, INPUT);
  // This pin LISTENS for the up button.

  pinMode(downPin, INPUT);
  // This pin LISTENS for the down button.

  Serial.begin(9600);
  // Open the Serial Monitor connection so we can print values for debugging.
}


// =============================================================================
// LOOP — runs FOREVER, continuously, as long as the board has power
// =============================================================================

void loop() {

  if (digitalRead(upPin) == HIGH && brightness < 255) {
    // IF the up button is pressed AND we're not already at maximum brightness...
    // The "&&" means BOTH things must be true at the same time.

    brightness = brightness + brightStep;
    // Add the step amount to brightness.
    // If brightness is 50 and brightStep is 5, it becomes 55.
    // Each press bumps it up by the step amount, not just by 1.

    Serial.println(brightness);
    // Print the new brightness value to the Serial Monitor window.
    // This is just for checking — doesn't affect how the program runs.
  }

  if (digitalRead(downPin) == HIGH && brightness > 0) {
    // IF the down button is pressed AND we're not already at zero...

    brightness = brightness - brightStep;
    // Subtract the step amount from brightness.
    // If brightness is 50 and brightStep is 5, it becomes 45.

    Serial.println(brightness);
    // Print the new value so we can watch it change in the Serial Monitor.
  }


  // --- SAFETY CLAMPS ---
  // These prevent brightness from going out of the valid range (0 to 255).
  // Because we're adding/subtracting in steps, we might overshoot the limits.
  // Example: if brightness is 252 and we add a step of 5, we'd get 257 — too high.
  // These clamps catch that and snap it back to the limit.

  if (brightness < 0) {
    brightness = 0;
    // Floor: can't go below off.
  }

  if (brightness > 255) {
    brightness = 255;
    // Ceiling: can't go above full brightness.
  }


  delay(30);
  // Wait 30 milliseconds before running the loop again.
  // This gives you time to release the button between presses.
  // Without this delay, one button press would fire dozens of times.


  analogWrite(ledPin, brightness);
  // Send the current brightness to the LED.
  // analogWrite uses PWM — it rapidly flickers the LED at the right rate
  // to create the appearance of a specific brightness level.
  // 0 = off, 128 = half brightness, 255 = full on.
}
