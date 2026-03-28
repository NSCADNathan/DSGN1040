/*
  Joystick 2

  Uses a joystick to move a single lit LED along a row of 8 LEDs connected
  through a shift register chip. Push the joystick up/down to move the light left or right.
  Press the joystick button to reset the light back to the middle.

  A shift register (74HC595) lets you control 8 output pins using only 3 pins on the ESP32.
  Think of it as a translator — you send it instructions in sequence, and it controls
  up to 8 LEDs (or other outputs) at once.

  The circuit:
  - Joystick SW  (switch) → GPIO 25
  - Joystick VRx (X axis) → GPIO 34
  - Joystick VRy (Y axis) → GPIO 35
  - Shift register: data=23, clock=18, latch=19
  - 8 LEDs connected to the shift register outputs

  Uses the ShiftRegister74HC595 library by Timo Denk.
  https://timodenk.com/blog/shift-register-arduino-library/

  Mashup by Theverant.
*/


#include <ShiftRegister74HC595.h>
// This line loads an external library — a pre-written set of code that handles
// the complexity of talking to the shift register chip.
// Without this library, we'd have to write a lot more low-level code.


// --- SHIFT REGISTER SETUP ---

// Create the shift register object and tell it which pins to use.
// Parameters: <1> = one shift register chip
// (data pin, clock pin, latch pin)
// ESP32: data=23, clock=18, latch=19 (avoids SPI flash pins 9/10/11)
ShiftRegister74HC595<1> sr(23, 18, 19);
// "sr" is the name we give this object. We'll use sr.set() and sr.setAllLow() later.
// Data pin:  sends the actual signal data to the chip
// Clock pin: a timing signal that tells the chip when to read each bit of data
// Latch pin: like pressing "send" — locks in all the data and outputs it at once


// --- PIN NUMBERS ---

const int SW_pin  = 25;  // GPIO 25 — joystick click button (INPUT_PULLUP: LOW = pressed)
const int VRx_pin = 34;  // GPIO 34 — analog X axis (we're not using X in this sketch)
const int VRy_pin = 35;  // GPIO 35 — analog Y axis (this controls left/right movement)
// Note: GPIO 34 and 35 are input-only ADC pins on the ESP32. Safe for analog reads.


// --- VARIABLES ---

int LEDpos = 3;   // which LED in the row is currently lit (0–7, starts in the middle-ish)
int joyPos = 0;   // stores the current joystick reading after we process it


// =============================================================================
// SETUP — runs ONE TIME when the board powers on
// =============================================================================

void setup() {

  pinMode(SW_pin, INPUT_PULLUP);
  // Set the joystick button as input with internal pull-up resistor.
  // This means: normally reads HIGH, reads LOW when the button is pressed.

  Serial.begin(9600);
  // Open a Serial Monitor connection for printing debug values.

  LEDpos = 3;
  // Start with LED #3 lit (counting from 0, so it's the 4th LED — roughly the middle).
}


// =============================================================================
// LOOP — runs FOREVER, continuously, as long as the board has power
// =============================================================================

void loop() {

  joyPos = analogRead(VRy_pin);
  // Read the Y axis of the joystick.
  // Returns a value from 0 to 4095 on the ESP32 (12-bit ADC).
  // Center position is roughly 2048.
  // Pushed one way: closer to 0. Pushed the other: closer to 4095.

  joyPos = map(joyPos, 0, 4095, 4095, 0);
  // map() takes a number from one range and converts it to another range.
  // Here we're INVERTING the reading — swapping 0 and 4095.
  // This flips the direction so pushing "up" moves the LED in the direction we want.
  // If the LED moves the wrong way on your joystick, comment this line out.
  // ESP32 ADC is 12-bit (0–4095); thresholds below are scaled from original Arduino 10-bit values.


  if (joyPos > 2100) {
    // If the joystick is pushed past the upper threshold (roughly 51% of range)...
    LEDpos = --LEDpos;
    // Move the active LED one position to the LEFT.
    // "--" means subtract 1. So LEDpos goes from, say, 4 to 3.
    delay(150);
    // Wait 150 milliseconds before checking again.
    // Without this, pushing the joystick slightly would zip the LED all the way across instantly.
  } else {
    if (joyPos < 1800) {
      // If the joystick is pushed past the lower threshold (roughly 44% of range)...
      LEDpos = ++LEDpos;
      // Move the active LED one position to the RIGHT.
      // "++" means add 1.
    }
  }
  // The gap between 1800 and 2100 is the "dead zone" — the center range where nothing happens.
  // This prevents the LED from drifting when the joystick is just resting in the middle.


  // --- BOUNDARY CHECKS ---
  // Make sure LEDpos stays within the valid range of 0 to 7 (8 LEDs, counting from 0).

  if (LEDpos == 8) {
    LEDpos = 7;
    // If it tries to go past the last LED, snap it back to the last LED.
  }

  if (LEDpos < 0) {
    LEDpos = 0;
    // If it tries to go before the first LED, snap it back to the first LED.
  }


  if (digitalRead(SW_pin) == LOW) {
    // If the joystick button is pressed (LOW because of INPUT_PULLUP)...
    sr.setAllLow();
    // Turn off all 8 LEDs immediately.
    LEDpos = 3;
    // Reset the position back to the middle.
  }


  // --- OUTPUT TO LEDS ---

  sr.setAllLow();
  // Turn off all 8 LEDs first. We do this every loop so only ONE LED is ever lit at a time.

  sr.set(LEDpos, HIGH);
  // Turn on just the LED at position LEDpos.
  // sr.set() takes a pin number (0–7) and a state (HIGH or LOW).
  // So if LEDpos is 3, only the 4th LED turns on.

  delay(150);
  // Wait 150 milliseconds before the loop runs again.
  // This controls how responsive the joystick feels.
  // Smaller = faster/more twitchy. Larger = slower/more sluggish.

  Serial.print(joyPos);
  // Print the processed joystick value to the Serial Monitor.
  // Useful for seeing the raw numbers while you move the joystick around.

  Serial.print("\n");
  // Move to a new line after each reading.
}
