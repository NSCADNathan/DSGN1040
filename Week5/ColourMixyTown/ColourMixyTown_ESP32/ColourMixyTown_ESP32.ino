/*
  ╔═══════════════════════════════════════════════════════════════════════════╗
  ║               COLOUR MIXY TOWN  —  ESP32 WROVER Version                  ║
  ║                                                                           ║
  ║  What this sketch does:                                                   ║
  ║  - Uses a rotary encoder (a clickable knob) to adjust the brightness      ║
  ║    of the Red, Green, and Blue channels of an RGB LED                     ║
  ║  - A 16×2 LCD screen shows you the current R, G, B values in real time   ║
  ║  - Press the encoder's built-in button to switch between R, G, and B     ║
  ║  - A ">" marker on the LCD shows which channel is currently selected      ║
  ║                                                                           ║
  ║  Mix colours the same way you would in Photoshop or Illustrator —         ║
  ║  but in hardware!                                                         ║
  ╚═══════════════════════════════════════════════════════════════════════════╝

  ┌─────────────────────────────────────────────────────────────────────────┐
  │  HARDWARE REQUIRED                                                       │
  │  • Freenove ESP32-WROVER-DEV + GPIO Extension Board                      │
  │  • RGB LED — common-anode OR common-cathode (see #define below)          │
  │      How to tell them apart: the longest leg is the "common" pin.        │
  │        Common ANODE   → longest leg goes to 3.3V  (+ rail)              │
  │        Common CATHODE → longest leg goes to GND   (− rail)              │
  │  • KY-040 rotary encoder module  (5 pins: CLK, DT, SW, VCC, GND)         │
  │  • 16×2 LCD (HD44780 compatible) with trimmer pot for contrast           │
  │  • 3× 220Ω resistors  (one for each LED colour channel)                  │
  └─────────────────────────────────────────────────────────────────────────┘

  ┌─────────────────────────────────────────────────────────────────────────┐
  │  WIRING GUIDE                                                            │
  │                                                                          │
  │  Rotary Encoder                                                          │
  │    CLK  (clock)   ──► GPIO 2                                             │
  │    DT   (data)    ──► GPIO 4                                             │
  │    SW   (button)  ──► GPIO 5                                             │
  │    VCC            ──► breadboard 3.3V rail                               │
  │    GND            ──► breadboard GND rail                                │
  │                                                                          │
  │  RGB LED  (R, G, B legs each go through a 220Ω resistor to GPIO)        │
  │    R leg  ──► 220Ω resistor ──► GPIO 25                                  │
  │    G leg  ──► 220Ω resistor ──► GPIO 26                                  │
  │    B leg  ──► 220Ω resistor ──► GPIO 27                                  │
  │                                                                          │
  │  Common pin (longest leg):                                               │
  │    Common ANODE   → breadboard 3.3V rail  (+ rail)                      │
  │    Common CATHODE → breadboard GND rail   (− rail)                      │
  │                                                                          │
  │  ⚠️  COMMON ANODE logic is INVERTED:                                     │
  │     Current flows FROM the 3.3V rail, THROUGH the LED, TO the GPIO pin. │
  │     GPIO LOW  = LED on  (pin pulls current down to 0V)                  │
  │     GPIO HIGH = LED off (no voltage difference = no current flows)       │
  │     The #define COMMON_ANODE below handles this automatically.           │
  │                                                                          │
  │  16×2 LCD (4-bit parallel interface — uses 6 GPIO pins)                  │
  │    RS  ──► GPIO 13    (Register Select)                                  │
  │    EN  ──► GPIO 14    (Enable — triggers the LCD to read data)           │
  │    D4  ──► GPIO 18    (Data bit 4)                                       │
  │    D5  ──► GPIO 19    (Data bit 5)                                       │
  │    D6  ──► GPIO 21    (Data bit 6)                                       │
  │    D7  ──► GPIO 22    (Data bit 7)                                       │
  │    VSS ──► breadboard GND rail                                           │
  │    VDD ──► breadboard 3.3V rail                                          │
  │    V0  ──► middle pin of trimmer pot (adjusts display contrast)          │
  │    RW  ──► breadboard GND rail                                           │
  │    A   ──► breadboard 3.3V rail through 220Ω  (backlight +)             │
  │    K   ──► breadboard GND rail  (backlight −)                            │
  └─────────────────────────────────────────────────────────────────────────┘

  ┌─────────────────────────────────────────────────────────────────────────┐
  │  ESP32 WROVER vs ARDUINO UNO — WHAT CHANGED AND WHY                     │
  │                                                                          │
  │  1. NO analogWrite() — Instead we use LEDC (LED Control peripheral)     │
  │     The ESP32 has a dedicated PWM system called LEDC.                    │
  │     ledcAttach(pin, freq, bits)  sets up PWM on any GPIO pin             │
  │     ledcWrite(pin, value)        sets brightness  0 = off, 255 = full   │
  │                                                                          │
  │  2. DIFFERENT PIN NUMBERS — and some pins are off-limits!               │
  │     GPIO 6–11 on the WROVER are used internally for the flash memory    │
  │     chip. They are labeled on the extension board (CLK, SD0–SD3, CMD).  │
  │     Never connect anything to those pins.                                │
  │                                                                          │
  │  3. 3.3V LOGIC (not 5V!)                                                 │
  │     All GPIO pins on the ESP32 WROVER operate at 3.3V.                  │
  │     Connecting a 5V signal directly can damage the chip!                 │
  │                                                                          │
  │  4. SERIAL BAUD RATE is 115200 (Uno used 9600)                           │
  │     Open Serial Monitor and set the speed to 115200.                     │
  └─────────────────────────────────────────────────────────────────────────┘

  🄯 2022 – copyleft – modified for ESP32 WROVER
*/

// ─────────────────────────────────────────────────────────────────────────────
// LIBRARY INCLUDE
// The LiquidCrystal library drives the LCD using the 4-bit parallel interface.
// It works on both Arduino and ESP32 — no changes needed here.
// ─────────────────────────────────────────────────────────────────────────────
#include <LiquidCrystal.h>


// ─────────────────────────────────────────────────────────────────────────────
// RGB LED TYPE — SET THIS BEFORE UPLOADING
//
// Look at your RGB LED. The longest leg is the "common" pin.
//
//   Common ANODE   = longest leg connects to 3.3V  (+ rail)
//   Common CATHODE = longest leg connects to GND   (- rail)
//
// If your LED is COMMON ANODE  → leave the line below as-is
// If your LED is COMMON CATHODE → comment it out by adding // at the start:
//     // #define COMMON_ANODE
// ─────────────────────────────────────────────────────────────────────────────
#define COMMON_ANODE


// ─────────────────────────────────────────────────────────────────────────────
// PIN DEFINITIONS  —  Rotary Encoder
//
// The KY-040 encoder has three signal pins:
//   CLK  — produces a pulse train as the shaft rotates
//   DT   — also pulses, but slightly out of phase with CLK
//          Together, CLK and DT tell us direction: CW or CCW
//   SW   — the pushbutton inside the shaft (click to switch R/G/B)
// ─────────────────────────────────────────────────────────────────────────────
#define CLK 2   // Encoder clock output
#define DT  4   // Encoder data output
#define SW  5   // Encoder button (active LOW — reads LOW when pressed)


// ─────────────────────────────────────────────────────────────────────────────
// PIN DEFINITIONS  —  RGB LED
//
// GPIO 25, 26, 27 are a clean sequential block — easy to wire in order.
// A 220Ω resistor sits between each GPIO pin and the LED leg to limit current.
// Without it you could burn out the LED or the GPIO!
// ─────────────────────────────────────────────────────────────────────────────
const int rPin = 25;   // Red   channel → GPIO 25
const int gPin = 26;   // Green channel → GPIO 26
const int bPin = 27;   // Blue  channel → GPIO 27


// ─────────────────────────────────────────────────────────────────────────────
// PIN DEFINITIONS  —  LCD (16×2, 4-bit parallel)
//
// We only use 6 of the LCD's 16 pins for the data interface.
// The 4-bit mode sends data in two nibbles instead of one byte, saving pins.
// ─────────────────────────────────────────────────────────────────────────────
const int rs = 13;   // Register Select: 0=command, 1=character data
const int en = 14;   // Enable: a HIGH→LOW pulse tells the LCD to latch data
const int d4 = 18;   // \
const int d5 = 19;   //  ╠ The 4 data lines (upper nibble of HD44780 bus)
const int d6 = 21;   //  ║
const int d7 = 22;   // /


// ─────────────────────────────────────────────────────────────────────────────
// LEDC PWM SETTINGS
//
// LEDC = "LED Control" — the ESP32's built-in PWM peripheral.
// We use it to create pulse-width modulation signals for the RGB LED.
//
// PWM works by switching the pin ON and OFF very rapidly.
// The proportion of time it spends ON vs OFF is called the "duty cycle".
//   Duty cycle 0%   (value=0)   → LED is always OFF
//   Duty cycle 50%  (value=127) → LED is at half brightness
//   Duty cycle 100% (value=255) → LED is always ON (full brightness)
//
// Frequency: 5000 Hz means the pin switches 5000 times per second.
// Our eyes cannot see flicker above ~60 Hz, so 5000 Hz looks perfectly smooth.
//
// Resolution 8-bit: the duty cycle is set with a number from 0 to 255.
// This is the same range as Arduino's analogWrite(), so the rest of the
// code can use the same 0–255 values we'd use on an Uno.
// ─────────────────────────────────────────────────────────────────────────────
const int pwmFreq       = 5000;   // PWM frequency in Hz
const int pwmResolution = 8;      // Bit depth: 8 bits = values 0 to 255


// ─────────────────────────────────────────────────────────────────────────────
// LCD OBJECT
//
// We tell the LiquidCrystal library which pin is connected where.
// After this one line, we can call lcd.print(), lcd.setCursor(), etc.
// ─────────────────────────────────────────────────────────────────────────────
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);


// ─────────────────────────────────────────────────────────────────────────────
// GLOBAL VARIABLES
// ─────────────────────────────────────────────────────────────────────────────

int currentStateCLK;   // The CLK pin value read RIGHT NOW in this loop iteration
int lastStateCLK;      // The CLK pin value from the PREVIOUS loop iteration
                       // Detecting a change (current ≠ last) means the knob moved

// Current colour channel values (0 = fully off, 255 = fully on)
int rValue = 50;   // Red   — starts at ~20% brightness
int gValue = 50;   // Green — starts at ~20% brightness
int bValue = 50;   // Blue  — starts at ~20% brightness
                   // At 50/50/50 you get a dim, cool white glow.

unsigned long lastButtonPress = 0;
// millis() tracks how many milliseconds the board has been running.
// We record the time of each button press here so we can ignore
// spurious "bounces" — the rapid voltage oscillations that happen
// when a mechanical button makes or breaks contact.

int buttonCounter = 1;
// Which colour channel the encoder is currently controlling:
//   1 = Red
//   2 = Green
//   3 = Blue
// Pressing the encoder button increments this, wrapping 3 → 1.


// ═════════════════════════════════════════════════════════════════════════════
//  SETUP  — runs once when the board powers on or resets
// ═════════════════════════════════════════════════════════════════════════════
void setup() {

  // Open the Serial Monitor connection (Tools → Serial Monitor in Arduino IDE)
  // Always use 115200 baud on ESP32 — the chip's internal oscillator is tuned for it
  Serial.begin(115200);

  // ── LCD ──────────────────────────────────────────────────────────────────
  // Tell the library the size of our display: 16 columns, 2 rows
  lcd.begin(16, 2);

  // ── Encoder pins ─────────────────────────────────────────────────────────
  pinMode(CLK, INPUT);          // CLK and DT are pure inputs — we read them
  pinMode(DT,  INPUT);

  // INPUT_PULLUP activates the ESP32's internal pull-up resistor on SW.
  // This means the pin reads HIGH normally and LOW when the button is pressed.
  // It saves us needing an external 10kΩ pull-up resistor on the breadboard.
  pinMode(SW,  INPUT_PULLUP);

  // ── LEDC PWM setup ───────────────────────────────────────────────────────
  // ledcAttach(pin, frequency, resolution_bits)
  //
  // This single function call replaces the two-step setup from ESP32 core v2:
  //   ledcSetup(channel, freq, resolution);   ← old way (v2)
  //   ledcAttachPin(pin, channel);            ← old way (v2)
  //
  // With core v3+, we attach directly to the pin. No channel numbers needed!
  // Each pin gets its own automatically-assigned hardware timer channel.
  ledcAttach(rPin, pwmFreq, pwmResolution);   // Red   channel PWM
  ledcAttach(gPin, pwmFreq, pwmResolution);   // Green channel PWM
  ledcAttach(bPin, pwmFreq, pwmResolution);   // Blue  channel PWM

  // ── Encoder baseline ─────────────────────────────────────────────────────
  // Store the current CLK state so the first loop() iteration has
  // something to compare against
  lastStateCLK = digitalRead(CLK);

  // ── Initial LCD display ───────────────────────────────────────────────────
  // lcd.setCursor(column, row) — columns 0–15, rows 0–1, top-left is (0,0)
  lcd.setCursor(1, 0);
  lcd.print("R:");
  lcd.print(rValue);    // Top row left side:   "R:50"

  lcd.setCursor(9, 0);
  lcd.print("G:");
  lcd.print(gValue);    // Top row right side:  "G:50"

  lcd.setCursor(1, 1);
  lcd.print("B:");
  lcd.print(bValue);    // Bottom row left side: "B:50"
}


// ═════════════════════════════════════════════════════════════════════════════
//  LOOP  — runs over and over as fast as the processor can manage (~millions/sec)
// ═════════════════════════════════════════════════════════════════════════════
void loop() {

  // ───────────────────────────────────────────────────────────────────────────
  // SECTION 1 — ADJUST RED  (active only when buttonCounter == 1)
  // ───────────────────────────────────────────────────────────────────────────
  if (buttonCounter == 1) {

    currentStateCLK = digitalRead(CLK);

    // A rotation event = CLK changed state.
    // We only act when CLK transitions to HIGH (== 1) to prevent
    // counting the same rotation twice (once going LOW, once going HIGH).
    if (currentStateCLK != lastStateCLK && currentStateCLK == 1) {

      // DT is out of phase with CLK.
      // If DT ≠ CLK at the moment of the CLK edge: turned CCW → decrease
      // If DT = CLK at the moment of the CLK edge: turned CW  → increase
      if (digitalRead(DT) != currentStateCLK) {
        rValue--;   // Counter-clockwise = less red
      } else {
        rValue++;   // Clockwise         = more red
      }

      // Clamp to valid range.
      // Without clamping, rValue could overflow below 0 or above 255,
      // causing the LED to behave unpredictably.
      if (rValue > 255) rValue = 255;
      if (rValue < 0)   rValue = 0;

      Serial.print("R: ");
      Serial.println(rValue);

      // Redraw just the R value on the LCD.
      // Printing 5 spaces first erases the old 3-digit number before
      // we overwrite it — prevents old digits leaving ghost characters.
      lcd.setCursor(1, 0);
      lcd.print("     ");    // Erase old value
      lcd.setCursor(1, 0);
      lcd.print("R:");
      lcd.print(rValue);     // Write new value
    }

    // Draw the ">" selection cursor next to Red, clear it from G and B
    lcd.setCursor(0, 0);   // Column 0, Row 0 — just left of "R:"
    lcd.print(">");
    lcd.setCursor(8, 0);   // Column 8, Row 0 — just left of "G:"
    lcd.print(" ");        // Space clears any previous ">"
    lcd.setCursor(0, 1);   // Column 0, Row 1 — just left of "B:"
    lcd.print(" ");

    lastStateCLK = currentStateCLK;   // Remember state for next iteration
  }


  // ───────────────────────────────────────────────────────────────────────────
  // SECTION 2 — ADJUST GREEN  (active only when buttonCounter == 2)
  // ───────────────────────────────────────────────────────────────────────────
  if (buttonCounter == 2) {

    currentStateCLK = digitalRead(CLK);

    if (currentStateCLK != lastStateCLK && currentStateCLK == 1) {

      if (digitalRead(DT) != currentStateCLK) {
        gValue--;   // CCW = less green
      } else {
        gValue++;   // CW  = more green
      }

      if (gValue > 255) gValue = 255;
      if (gValue < 0)   gValue = 0;

      Serial.print("G: ");
      Serial.println(gValue);

      lcd.setCursor(9, 0);
      lcd.print("     ");
      lcd.setCursor(9, 0);
      lcd.print("G:");
      lcd.print(gValue);
    }

    // Move selection cursor to Green
    lcd.setCursor(8, 0);
    lcd.print(">");
    lcd.setCursor(0, 0);
    lcd.print(" ");
    lcd.setCursor(0, 1);
    lcd.print(" ");

    lastStateCLK = currentStateCLK;
  }


  // ───────────────────────────────────────────────────────────────────────────
  // SECTION 3 — ADJUST BLUE  (active only when buttonCounter == 3)
  // ───────────────────────────────────────────────────────────────────────────
  if (buttonCounter == 3) {

    currentStateCLK = digitalRead(CLK);

    if (currentStateCLK != lastStateCLK && currentStateCLK == 1) {

      if (digitalRead(DT) != currentStateCLK) {
        bValue--;   // CCW = less blue
      } else {
        bValue++;   // CW  = more blue
      }

      if (bValue > 255) bValue = 255;
      if (bValue < 0)   bValue = 0;

      Serial.print("B: ");
      Serial.println(bValue);

      lcd.setCursor(1, 1);
      lcd.print("     ");
      lcd.setCursor(1, 1);
      lcd.print("B:");
      lcd.print(bValue);
    }

    // Move selection cursor to Blue
    lcd.setCursor(0, 1);
    lcd.print(">");
    lcd.setCursor(8, 0);
    lcd.print(" ");
    lcd.setCursor(0, 0);
    lcd.print(" ");

    lastStateCLK = currentStateCLK;
  }


  // ───────────────────────────────────────────────────────────────────────────
  // SECTION 4 — BUTTON: cycle through R → G → B → R
  // ───────────────────────────────────────────────────────────────────────────

  int btnState = digitalRead(SW);

  // LOW means the button IS pressed  (remember we used INPUT_PULLUP)
  if (btnState == LOW) {

    // Debounce: only register a press if at least 1000ms have passed
    // since the last one.  This prevents one finger-press from being
    // counted multiple times due to contact bounce.
    // millis() returns elapsed time in milliseconds since the sketch started.
    if (millis() - lastButtonPress > 1000) {

      buttonCounter++;              // Advance to next channel

      if (buttonCounter == 4) {     // Wrap around after Blue
        buttonCounter = 1;          // Back to Red
      }

      lastButtonPress = millis();   // Record the timestamp of this press
    }

    delay(2);   // Tiny additional debounce pause
  }


  // ───────────────────────────────────────────────────────────────────────────
  // SECTION 5 — DRIVE THE LED
  //
  // This runs every single loop iteration — continuously keeping the LED
  // glowing at the colour we've mixed.
  //
  // ledcWrite(pin, value)
  //   pin   = which GPIO pin to control (must have been set up with ledcAttach)
  //   value = duty cycle  0 (off) → 255 (full brightness) at 8-bit resolution
  // ───────────────────────────────────────────────────────────────────────────
  // Drive the LED — logic depends on whether it is common-anode or common-cathode.
  //
  // Common CATHODE (normal): pin HIGH = LED on, pin LOW = LED off.
  //   ledcWrite(rPin, 255) = full brightness
  //
  // Common ANODE (inverted): current flows from 3.3V rail, through LED, to pin.
  //   Pin LOW  pulls current through = LED on.
  //   Pin HIGH = same voltage as 3.3V rail = no current = LED off.
  //   So we must write (255 - value):
  //     rValue=255 → ledcWrite(rPin, 0)   → pin LOW  → full red on
  //     rValue=0   → ledcWrite(rPin, 255) → pin HIGH → red off
#ifdef COMMON_ANODE
  ledcWrite(rPin, 255 - rValue);   // Inverted: anode LED, pin LOW = on
  ledcWrite(gPin, 255 - gValue);
  ledcWrite(bPin, 255 - bValue);
#else
  ledcWrite(rPin, rValue);         // Normal: cathode LED, pin HIGH = on
  ledcWrite(gPin, gValue);
  ledcWrite(bPin, bValue);
#endif
}
