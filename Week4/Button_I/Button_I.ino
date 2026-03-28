/*
  Button I

  Turns on and off a light emitting diode (LED) connected to pin 27,
  when pressing a pushbutton attached to pin 25.

  The circuit:
  - LED attached from pin 27 to ground through 220 ohm resistor
  - pushbutton attached to pin 25 from +3.3V

  Originally created 2005 by DojoDave
  Modified by Tom Igoe, 2011
  Refactored for ESP32 by Theverant

  https://www.arduino.cc/en/Tutorial/BuiltInExamples/Button
*/

// --- PIN NUMBERS ---
// Think of these like giving nicknames to specific physical pins on the board.
// Instead of writing "25" everywhere, we write "buttonPin" — much easier to read.
// "const" means this number will NEVER change while the program runs.
// "int" means it's a whole number (integer). No decimals.

const int buttonPin = 25;  // GPIO 25 on the ESP32 — this pin reads the button
const int ledPin = 27;     // GPIO 27 on the ESP32 — this pin controls the LED


// --- VARIABLES ---
// Unlike constants above, variables CAN change while the program runs.
// This one stores whether the button is currently pressed or not.

int buttonState = 0;  // 0 = not pressed. Will become 1 when the button is pressed.


// =============================================================================
// SETUP — runs ONE TIME when the board first turns on
// Use this section to configure your pins before the main loop begins
// =============================================================================

void setup() {

  pinMode(ledPin, OUTPUT);
  // pinMode tells the board how a pin will be used.
  // OUTPUT means this pin will SEND electricity OUT — it powers the LED.

  pinMode(buttonPin, INPUT);
  // INPUT means this pin will RECEIVE electricity — it listens to the button.
}


// =============================================================================
// LOOP — runs FOREVER, over and over, as long as the board has power
// This is the heartbeat of your program. Everything here repeats endlessly.
// =============================================================================

void loop() {

  buttonState = digitalRead(buttonPin);
  // digitalRead() checks whether electricity is flowing through a pin RIGHT NOW.
  // It returns one of two values:
  //   HIGH = electricity is flowing = button is pressed
  //   LOW  = no electricity = button is not pressed
  // We store that result in "buttonState" so we can make a decision below.


  if (buttonState == HIGH) {
    // IF the button is pressed (buttonState equals HIGH)...

    digitalWrite(ledPin, HIGH);
    // ...send electricity to the LED pin.
    // digitalWrite() either switches a pin ON (HIGH) or OFF (LOW).
    // HIGH = electricity flows = LED lights up.

  } else {
    // OTHERWISE — if the button is NOT pressed...

    digitalWrite(ledPin, LOW);
    // ...cut the electricity to the LED pin.
    // LOW = no electricity = LED goes dark.
  }

}
