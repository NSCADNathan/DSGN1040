/*
  ╔═══════════════════════════════════════════════════════════════════════════╗
  ║        DC MOTOR III  —  Two-Button Control with Direction LEDs            ║
  ║                        ESP32 WROVER Version                               ║
  ║               Using L293D H-Bridge Motor Driver IC                       ║
  ║                                                                           ║
  ║  What this sketch does:                                                   ║
  ║  Everything from DC Motor II, plus two LEDs that show you what the        ║
  ║  motor is doing — without looking at the motor itself.                   ║
  ║                                                                           ║
  ║  CW LED (GPIO 27) — glows when the motor spins clockwise                 ║
  ║  CCW LED (GPIO 14) — glows when the motor spins counter-clockwise        ║
  ║                                                                           ║
  ║  Brightness tells you speed:                                              ║
  ║    Dim glow = slow   Full brightness = full speed   Off = stopped        ║
  ║                                                                           ║
  ║  RIGHT button — steps motor UP through 5 CW speed steps                 ║
  ║  LEFT  button — steps motor DOWN, through stop, into 5 CCW steps        ║
  ╚═══════════════════════════════════════════════════════════════════════════╝

  ┌─────────────────────────────────────────────────────────────────────────┐
  │  HARDWARE REQUIRED                                                       │
  │  • Freenove ESP32-WROVER-DEV + GPIO Extension Board                      │
  │  • L293D H-Bridge Motor Driver IC                                        │
  │  • Small DC motor                                                        │
  │  • 2× momentary pushbuttons                                              │
  │  • 2× LEDs  (any colour — one for CW, one for CCW)                      │
  │  • 2× 220Ω resistors  (current limiting, one per LED)                   │
  │  • 2× 1kΩ resistors  (pull-down, one per button)                        │
  └─────────────────────────────────────────────────────────────────────────┘

  ┌─────────────────────────────────────────────────────────────────────────┐
  │  WIRING GUIDE                                                            │
  │                                                                          │
  │  BUTTONS  (same as DC Motor II and Week 4 Button_II)                    │
  │    RIGHT button ──► GPIO 25  +  1kΩ to GND rail                        │
  │    LEFT  button ──► GPIO 26  +  1kΩ to GND rail                        │
  │    Both buttons: other leg ──► breadboard 3.3V rail                     │
  │                                                                          │
  │  DIRECTION LEDs                                                          │
  │    CW  LED: GPIO 27 ──► 220Ω resistor ──► LED anode ──► LED cathode ──► GND rail │
  │    CCW LED: GPIO 14 ──► 220Ω resistor ──► LED anode ──► LED cathode ──► GND rail │
  │    (GPIO 27 is the same LED used in the Week 4 Button sketches —        │
  │     no rewiring needed if you're building on that breadboard layout)    │
  │                                                                          │
  │    The LED brightness is controlled by PWM — the same technique used    │
  │    to control the motor speed. More ON time = brighter glow.            │
  │                                                                          │
  │  MOTOR CONTROL  (L293D H-Bridge)                                        │
  │                                                                          │
  │  ESP32 WROVER          L293D pin                                         │
  │  ─────────────────────────────────────────────────────────────────────  │
  │  GPIO 32  ──────►  IN1  (pin 2)   — direction control A                 │
  │  GPIO 33  ──────►  IN2  (pin 7)   — direction control B                 │
  │  GPIO 23  ──────►  EN1  (pin 1)   — enable + speed (PWM signal)         │
  │  GND      ──────►  GND  (pins 4, 5, 12, 13)                             │
  │                                                                          │
  │  L293D output      Motor / Power                                         │
  │  ─────────────────────────────────────────────────────────────────────  │
  │  OUT1 (pin 3)  ───► Motor terminal A                                     │
  │  OUT2 (pin 6)  ───► Motor terminal B                                     │
  │  VCC1 (pin 16) ──► breadboard 3.3V rail  (powers L293D logic)           │
  │  VCC2 (pin 8)  ──► breadboard 5V rail    (powers the motor coils)       │
  │                    ⚠️  VCC2 must be 5V — motor won't run on 3.3V.       │
  └─────────────────────────────────────────────────────────────────────────┘

  ┌─────────────────────────────────────────────────────────────────────────┐
  │  DIRECTION LOGIC TABLE (H-Bridge truth table)                           │
  │                                                                          │
  │  IN1 (GPIO 32) │ IN2 (GPIO 33) │ Motor behaviour                        │
  │  ──────────────┼───────────────┼──────────────────────────              │
  │  LOW           │ HIGH          │ Spins CLOCKWISE                         │
  │  HIGH          │ LOW           │ Spins COUNTER-CLOCKWISE                 │
  │  LOW           │ LOW           │ Coasts to a stop  (freewheels)          │
  │                                                                          │
  │  EN1 (GPIO 23, PWM):   0 = stopped   180 = slow   255 = full speed      │
  └─────────────────────────────────────────────────────────────────────────┘

  ┌─────────────────────────────────────────────────────────────────────────┐
  │  WHY DO WE NEED AN L293D?                                               │
  │                                                                          │
  │  A GPIO pin can supply ~40mA. A motor needs 100–600mA.                  │
  │  Think of it like a light switch: the physical movement of the switch   │
  │  doesn't power the light — it causes changes that open or close the     │
  │  circuit, letting mains electricity flow through. A transistor works    │
  │  the same way: the small GPIO voltage causes molecular changes inside   │
  │  the transistor that open or close the path for the larger 5V current.  │
  └─────────────────────────────────────────────────────────────────────────┘

  ┌─────────────────────────────────────────────────────────────────────────┐
  │  ESP32 WROVER vs ARDUINO UNO — WHAT CHANGED AND WHY                     │
  │                                                                          │
  │  Arduino Uno used analogWrite(pin, value) for PWM.                      │
  │  ESP32 Arduino core v3 does support analogWrite() as a compatibility    │
  │  function — but it defaults to 1kHz, which is audible as a high-pitched │
  │  whine from the motor coils.                                             │
  │                                                                          │
  │  Instead we use LEDC (LED Control), the ESP32's native PWM peripheral:  │
  │    ledcAttach(pin, freq, resolution)  — set up PWM on a GPIO pin        │
  │    ledcWrite(pin, value)              — set duty cycle  (0–255)         │
  │                                                                          │
  │  We set motor PWM to 30kHz — above human hearing — so the motor runs    │
  │  silently. LED PWM runs at 5kHz which is plenty for smooth dimming.     │
  │  analogWrite() would work but the motor would whine.                    │
  └─────────────────────────────────────────────────────────────────────────┘
*/


// ─────────────────────────────────────────────────────────────────────────────
// PIN DEFINITIONS  —  Buttons
// ─────────────────────────────────────────────────────────────────────────────
const int btnRight = 25;   // RIGHT button — step speed UP / go CW
const int btnLeft  = 26;   // LEFT  button — step speed DOWN / go CCW


// ─────────────────────────────────────────────────────────────────────────────
// PIN DEFINITIONS  —  Direction LEDs
//
// These LEDs give visual feedback about what the motor is doing.
// PWM controls their brightness — the same 0–255 range used for motor speed.
// So "faster motor" automatically means "brighter LED".
// ─────────────────────────────────────────────────────────────────────────────
const int ledCW  = 27;   // Clockwise indicator LED — same pin as Button_II's ledPin
const int ledCCW = 14;   // Counter-clockwise indicator LED


// ─────────────────────────────────────────────────────────────────────────────
// PIN DEFINITIONS  —  Motor (L293D H-Bridge)
// ─────────────────────────────────────────────────────────────────────────────
const int motor_IN1    = 32;   // Direction control A
const int motor_IN2    = 33;   // Direction control B
const int motor_ENABLE = 23;   // Enable + speed (PWM) — moved from 14


// ─────────────────────────────────────────────────────────────────────────────
// LEDC PWM SETTINGS
//
// Motor: 30kHz keeps the switching noise above human hearing.
// LEDs:  5kHz is plenty for smooth, flicker-free dimming.
// ─────────────────────────────────────────────────────────────────────────────
const int motorPwmFreq  = 30000;   // 30kHz — silent motor operation
const int ledPwmFreq    = 5000;    // 5kHz  — smooth LED brightness
const int pwmResolution = 8;       // 8-bit: values 0–255 for both


// ─────────────────────────────────────────────────────────────────────────────
// SPEED STEPS
//
// NUM_STEPS: how many speed steps between stopped and full speed.
// SPEED_MIN:  the lowest PWM value that reliably gets the motor spinning.
//             Below this the motor may stall. Adjust if needed.
//
// getMotorSpeed(step) spreads NUM_STEPS evenly from SPEED_MIN to 255.
// getLedBrightness(step) spreads the same steps from 50 to 255 — step 1 gives
// a visible glow rather than zero. Motor speed range is independent.
//
// Change only SPEED_MIN and everything recalculates automatically.
// ─────────────────────────────────────────────────────────────────────────────
const int NUM_STEPS = 5;
const int SPEED_MIN = 180;


// map(value, fromLow, fromHigh, toLow, toHigh)
//
// map() takes a number from one range and rescales it to fit another range.
// It's like converting units — the same idea as converting 0–100% into 0–255.
//
// Example with getMotorSpeed, NUM_STEPS=5, SPEED_MIN=180:
//   map(1, 1, 5, 180, 255)  →  180   (step 1 = slowest)
//   map(3, 1, 5, 180, 255)  →  217   (step 3 = middle)
//   map(5, 1, 5, 180, 255)  →  255   (step 5 = full speed)
//
// Example with getLedBrightness, NUM_STEPS=5:
//   map(1, 1, 5, 50, 255)  →   50   (step 1 = dim but visible)
//   map(3, 1, 5, 50, 255)  →  152   (step 3 = mid brightness)
//   map(5, 1, 5, 50, 255)  →  255   (step 5 = full brightness)
//
// The two functions use the same step number but map it to different ranges —
// motor needs a narrow window (SPEED_MIN→255) to run safely,
// LED can use the full range (0→255) for maximum visual effect.
//
// BONUS: map() can also reverse a range by swapping toLow and toHigh.
// Imagine a volume knob where turning it up should dim a warning LED:
//   map(volume, 0, 255, 255, 0)
// volume=0   → LED brightness 255 (warning fully on at silence)
// volume=255 → LED brightness 0   (warning off at full volume)
// Any time "more input = less output", swap the last two arguments.

int getMotorSpeed(int step) {
  if (step == 0) return 0;
  return map(step, 1, NUM_STEPS, SPEED_MIN, 255);
}

int getLedBrightness(int step) {
  if (step == 0) return 0;
  return map(step, 1, NUM_STEPS, 50, 255);
}


// ─────────────────────────────────────────────────────────────────────────────
// STATE VARIABLE — speedStep
//
//   speedStep > 0  →  Clockwise,          speed = speedTable[speedStep]
//   speedStep < 0  →  Counter-clockwise,  speed = speedTable[-speedStep]
//   speedStep = 0  →  Stopped
//
// Range: -5 (full CCW) to +5 (full CW)
// RIGHT button: +1   LEFT button: -1
// ─────────────────────────────────────────────────────────────────────────────
int speedStep = 0;


// ─────────────────────────────────────────────────────────────────────────────
// DEBOUNCE TIMERS
// ─────────────────────────────────────────────────────────────────────────────
const unsigned long DEBOUNCE_MS = 300;
unsigned long lastPressRight    = 0;
unsigned long lastPressLeft     = 0;


// ═════════════════════════════════════════════════════════════════════════════
//  SETUP
// ═════════════════════════════════════════════════════════════════════════════
void setup() {

  Serial.begin(115200);

  // ── Buttons ───────────────────────────────────────────────────────────────
  pinMode(btnRight, INPUT);
  pinMode(btnLeft,  INPUT);

  // ── Motor direction pins ──────────────────────────────────────────────────
  pinMode(motor_IN1, OUTPUT);
  pinMode(motor_IN2, OUTPUT);
  digitalWrite(motor_IN1, LOW);
  digitalWrite(motor_IN2, LOW);

  // ── LEDC PWM — motor enable ───────────────────────────────────────────────
  // ledcAttach(pin, frequency, resolution_bits) — ESP32 core v3 API
  ledcAttach(motor_ENABLE, motorPwmFreq, pwmResolution);
  ledcWrite(motor_ENABLE, 0);

  // ── LEDC PWM — direction LEDs ─────────────────────────────────────────────
  // We use the same ledcAttach call for the LEDs.
  // PWM on an LED pin varies its brightness just like PWM on the motor pin
  // varies its speed — it's the same underlying mechanism.
  ledcAttach(ledCW,  ledPwmFreq, pwmResolution);
  ledcAttach(ledCCW, ledPwmFreq, pwmResolution);
  ledcWrite(ledCW,  0);   // Both LEDs off at startup
  ledcWrite(ledCCW, 0);

  Serial.println("DC Motor III ready.");
  Serial.println("RIGHT = faster / CW    LEFT = slower / CCW");
}


// ═════════════════════════════════════════════════════════════════════════════
//  LOOP
// ═════════════════════════════════════════════════════════════════════════════
void loop() {

  unsigned long now = millis();

  // ── RIGHT button — step up ────────────────────────────────────────────────
  if (digitalRead(btnRight) == HIGH) {
    if (now - lastPressRight > DEBOUNCE_MS) {
      speedStep = min(speedStep + 1, NUM_STEPS);
      lastPressRight = now;
      applyMotor();
      printState();
    }
  }

  // ── LEFT button — step down ───────────────────────────────────────────────
  if (digitalRead(btnLeft) == HIGH) {
    if (now - lastPressLeft > DEBOUNCE_MS) {
      speedStep = max(speedStep - 1, -NUM_STEPS);
      lastPressLeft = now;
      applyMotor();
      printState();
    }
  }
}


// ═════════════════════════════════════════════════════════════════════════════
//  applyMotor()
//
//  Sets motor direction + speed AND LED brightness to match the current
//  speedStep. Both the motor and LEDs are updated together every time.
// ═════════════════════════════════════════════════════════════════════════════
void applyMotor() {

  int absStep    = abs(speedStep);
  int pwmValue   = getMotorSpeed(absStep);    // motor PWM  — SPEED_MIN to 255
  int brightness = getLedBrightness(absStep); // LED brightness — 50 to 255

  if (speedStep > 0) {
    // ── CLOCKWISE ──────────────────────────────────────────────────────────
    digitalWrite(motor_IN1, LOW);
    digitalWrite(motor_IN2, HIGH);
    ledcWrite(motor_ENABLE, pwmValue);

    // LED uses its own brightness table — full 0–255 sweep across the 5 steps,
    // independent of the narrower motor PWM range.
    ledcWrite(ledCW,  brightness);
    ledcWrite(ledCCW, 0);

  } else if (speedStep < 0) {
    // ── COUNTER-CLOCKWISE ──────────────────────────────────────────────────
    digitalWrite(motor_IN1, HIGH);
    digitalWrite(motor_IN2, LOW);
    ledcWrite(motor_ENABLE, pwmValue);

    ledcWrite(ledCW,  0);
    ledcWrite(ledCCW, brightness);

  } else {
    // ── STOPPED ────────────────────────────────────────────────────────────
    digitalWrite(motor_IN1, LOW);
    digitalWrite(motor_IN2, LOW);
    ledcWrite(motor_ENABLE, 0);

    // Both LEDs off — nothing is spinning.
    ledcWrite(ledCW,  0);
    ledcWrite(ledCCW, 0);
  }
}


// ═════════════════════════════════════════════════════════════════════════════
//  printState()
// ═════════════════════════════════════════════════════════════════════════════
void printState() {

  int absStep    = abs(speedStep);
  int pwmValue   = getMotorSpeed(absStep);
  int brightness = getLedBrightness(absStep);

  Serial.print("Step: ");
  if (speedStep > 0) Serial.print("+");
  Serial.print(speedStep);
  Serial.print("  →  ");

  if (speedStep > 0) {
    Serial.print("CW   motor: ");
    Serial.print(pwmValue);
    Serial.print("/255  LED: ");
    Serial.print(brightness);
    Serial.println("/255");
  } else if (speedStep < 0) {
    Serial.print("CCW  motor: ");
    Serial.print(pwmValue);
    Serial.print("/255  LED: ");
    Serial.print(brightness);
    Serial.println("/255");
  } else {
    Serial.println("STOPPED");
  }
}
