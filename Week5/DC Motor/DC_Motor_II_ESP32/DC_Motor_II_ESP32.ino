/*
  ╔═══════════════════════════════════════════════════════════════════════════╗
  ║           DC MOTOR II  —  Two-Button Speed & Direction Control            ║
  ║                        ESP32 WROVER Version                               ║
  ║               Using L293D H-Bridge Motor Driver IC                        ║
  ║                                                                           ║
  ║  What this sketch does:                                                   ║
  ║  Two buttons give you hands-on control of the motor's speed and           ║
  ║  direction — like a tiny physical speed controller.                       ║
  ║                                                                           ║
  ║  RIGHT button — steps the motor UP through 5 speed steps, clockwise       ║
  ║    Press 1: slow CW                                                       ║
  ║    Press 2: a bit faster CW                                               ║
  ║    Press 3: medium CW                                                     ║
  ║    Press 4: fast CW                                                       ║
  ║    Press 5: full speed CW                                                 ║
  ║                                                                           ║
  ║  LEFT button — steps the motor DOWN (slower, then stop, then CCW)         ║
  ║    From full speed CW: each press slows by one step                       ║
  ║    At zero: the motor stops                                               ║
  ║    One more press: slow CCW (mirror image of the CW steps)                ║
  ║    Keep pressing: up to full speed CCW                                    ║
  ╚═══════════════════════════════════════════════════════════════════════════╝

  ┌─────────────────────────────────────────────────────────────────────────┐
  │  HARDWARE REQUIRED                                                      │
  │  • Freenove ESP32-WROVER-DEV + GPIO Extension Board                     │
  │  • L293D H-Bridge Motor Driver IC                                       │
  │  • Small DC motor (e.g., the one in the Freenove kit)                   │
  │  • 2× momentary pushbuttons                                             │
  │  • 2× 1kΩ resistors  (pull-down, one per button)                        │
  └─────────────────────────────────────────────────────────────────────────┘

  ┌─────────────────────────────────────────────────────────────────────────┐
  │  WIRING GUIDE                                                           │
  │                                                                         │
  │  BUTTONS  (same wiring as the Week 4 Button_II sketch)                  │
  │  Each button has one leg connected to the GPIO pin and the other to     │
  │  the 3.3V rail. A 1kΩ resistor connects the GPIO pin to GND.            │
  │  When the button is NOT pressed: pin reads LOW (pulled down to GND).    │
  │  When the button IS pressed: 3.3V flows in → pin reads HIGH.            │
  │                                                                         │
  │    RIGHT button ──► GPIO 25  +  1kΩ to GND rail                         │
  │    LEFT  button ──► GPIO 26  +  1kΩ to GND rail                         │
  │    Both buttons: other leg ──► breadboard 3.3V rail                     │
  │                                                                         │
  │  MOTOR CONTROL  (L293D H-Bridge)                                        │
  │                                                                         │
  │  ESP32 WROVER          L293D pin                                        │
  │  ─────────────────────────────────────────────                          │
  │  GPIO 32  ──────►  IN1  (pin 2)   — direction control A                 │
  │  GPIO 33  ──────►  IN2  (pin 7)   — direction control B                 │
  │  GPIO 23  ──────►  EN1  (pin 1)   — enable + speed (PWM signal)         │
  │  GND      ──────►  GND  (pins 4, 5, 12, 13)                             │
  │                                                                         │
  │  L293D output      Motor / Power                                        │
  │  ─────────────────────────────────────────────                          │
  │  OUT1 (pin 3)  ───► Motor terminal A                                    │
  │  OUT2 (pin 6)  ───► Motor terminal B                                    │
  │  VCC1 (pin 16) ──► breadboard 3.3V rail  (powers L293D logic)           │
  │  VCC2 (pin 8)  ──► breadboard 5V rail    (powers the motor coils)       │
  │                    ⚠️  VCC2 must be 5V — motor won't run on 3.3V.        │
  │                        The extension board puts 5V on the bottom rail.  │
  │                                                                         │
  │  ⚠️  Same breadboard layout as DC_Motor_ESP32 — no rewiring needed       │
  │     between the two sketches. Both use GPIO 32, 33, 14 for the motor.   │
  └─────────────────────────────────────────────────────────────────────────┘

  ┌─────────────────────────────────────────────────────────────────────────┐
  │  WHY DO WE NEED AN L293D?  (The H-Bridge chip)                          │
  │                                                                         │
  │  A GPIO pin on the ESP32 WROVER can only supply about 40mA of current.  │
  │  A DC motor typically needs 100–600mA to spin.                          │
  │  Connecting a motor directly to GPIO would:                             │
  │    a) Not spin the motor (not enough current)                           │
  │    b) Potentially destroy your ESP32                                    │
  │                                                                         │
  │  Think of it like a light switch in a wall:                             │
  │    The work you do moving the switch doesn't power the light —          │
  │    the mechanical movement causes physical changes that open or close   │
  │    the circuit, allowing mains electricity to flow through the bulb.    │
  │    A transistor works the same way: the small voltage from the GPIO     │
  │    causes molecular changes inside the transistor that open or close    │
  │    the path for the larger 5V current to flow through the motor.        │
  │                                                                         │
  │  The L293D is also an "H-Bridge":                                       │
  │  Two sets of transistors arranged in an "H" shape can push current      │
  │  through the motor in EITHER direction by flipping which transistors    │
  │  are open/closed. That's how we reverse direction without rewiring.     │
  └─────────────────────────────────────────────────────────────────────────┘

  ┌─────────────────────────────────────────────────────────────────────────┐
  │  DIRECTION LOGIC TABLE (H-Bridge truth table)                           │
  │                                                                         │
  │  IN1 (GPIO 32) │ IN2 (GPIO 33) │ Motor behaviour                        │
  │  ──────────────┼───────────────┼──────────────────────────              │
  │  LOW           │ HIGH          │ Spins CLOCKWISE                        │
  │  HIGH          │ LOW           │ Spins COUNTER-CLOCKWISE                │
  │  LOW           │ LOW           │ Coasts to a stop  (freewheels)         │
  │                                                                         │
  │  EN1 (GPIO 14, PWM):                                                    │
  │    0   → Motor disabled (stopped regardless of IN1/IN2)                 │
  │    180 → Minimum speed that reliably overcomes friction                 │
  │    255 → Full speed                                                     │
  └─────────────────────────────────────────────────────────────────────────┘

  ┌─────────────────────────────────────────────────────────────────────────┐
  │  ESP32 WROVER vs ARDUINO UNO — WHAT CHANGED AND WHY                     │
  │                                                                         │
  │  Arduino Uno used analogWrite(pin, value) for PWM.                      │
  │  ESP32 Arduino core v3 does support analogWrite() as a compatibility    │
  │  function — but it defaults to 1kHz, which is audible as a high-pitched │
  │  whine from the motor coils.                                            │
  │                                                                         │
  │  Instead we use LEDC (LED Control), the ESP32's native PWM peripheral:  │
  │    ledcAttach(pin, freq, resolution)  — set up PWM on a GPIO pin        │
  │    ledcWrite(pin, value)              — set duty cycle  (0–255)         │
  │                                                                         │
  │  We set freq to 30kHz — above human hearing — so the motor runs         │
  │  silently. analogWrite() would work but the motor would whine.          │
  └─────────────────────────────────────────────────────────────────────────┘
*/


// ─────────────────────────────────────────────────────────────────────────────
// PIN DEFINITIONS  —  Buttons
//
// These match the Button_II wiring exactly.
// Buttons connect their GPIO pin to the 3.3V rail.
// A 1kΩ pull-down resistor holds the pin LOW when the button is not pressed.
// Pressing the button brings the pin HIGH.
// ─────────────────────────────────────────────────────────────────────────────
const int btnRight = 25;   // RIGHT button — step speed UP / go CW
const int btnLeft  = 26;   // LEFT  button — step speed DOWN / go CCW


// ─────────────────────────────────────────────────────────────────────────────
// PIN DEFINITIONS  —  Motor (L293D H-Bridge)
//
// These are the same pins used in DC_Motor_ESP32 — no rewiring needed.
// ─────────────────────────────────────────────────────────────────────────────
const int motor_IN1    = 32;   // L293D IN1 — direction control A
const int motor_IN2    = 33;   // L293D IN2 — direction control B
const int motor_ENABLE = 23;   // L293D EN1 — speed control (PWM)


// ─────────────────────────────────────────────────────────────────────────────
// LEDC PWM SETTINGS
//
// 30kHz is above human hearing — prevents the motor making a high-pitched whine.
// 8-bit resolution gives values 0–255 for the duty cycle.
// ─────────────────────────────────────────────────────────────────────────────
const int pwmFreq       = 30000;   // 30kHz — above human hearing range
const int pwmResolution = 8;       // 8-bit: 256 speed values (0–255)


// ─────────────────────────────────────────────────────────────────────────────
// SPEED STEPS
//
// A lookup table of 5 speed values.
// Index 0 is "stopped", indices 1–5 are progressively faster.
// We start at 180 rather than 1 because at very low PWM values the motor
// doesn't have enough torque to overcome its own internal friction — it just
// sits there making noise without actually spinning.
// ─────────────────────────────────────────────────────────────────────────────
const int NUM_STEPS = 5;
const int speedTable[NUM_STEPS + 1] = {
  0,    // step 0 — stopped
  180,  // step 1 — slow  (~71%)
  200,  // step 2 — medium-slow  (~78%)
  215,  // step 3 — medium  (~84%)
  235,  // step 4 — fast  (~92%)
  255   // step 5 — full speed  (100%)
};


// ─────────────────────────────────────────────────────────────────────────────
// STATE VARIABLE — speedStep
//
// This single integer captures everything: speed AND direction.
//
//   speedStep > 0  →  Clockwise,          speed = speedTable[speedStep]
//   speedStep < 0  →  Counter-clockwise,  speed = speedTable[-speedStep]
//   speedStep = 0  →  Stopped
//
// Range: -5 (full speed CCW) to +5 (full speed CW)
//
// RIGHT button adds 1:  -5 → -4 → ... → 0 → 1 → ... → 5
// LEFT  button subtracts 1:  5 → 4 → ... → 0 → -1 → ... → -5
// ─────────────────────────────────────────────────────────────────────────────
int speedStep = 0;   // Start stopped


// ─────────────────────────────────────────────────────────────────────────────
// DEBOUNCE TIMERS
//
// Mechanical buttons don't make clean contact — they "bounce" rapidly
// between HIGH and LOW for a few milliseconds after each press.
// Without debouncing, one finger-press could register as many button events.
//
// We track the last time each button was acted on, and ignore any new press
// that happens less than DEBOUNCE_MS milliseconds later.
// ─────────────────────────────────────────────────────────────────────────────
const unsigned long DEBOUNCE_MS = 300;   // 300ms feels responsive but ignores bounce
unsigned long lastPressRight    = 0;
unsigned long lastPressLeft     = 0;


// ═════════════════════════════════════════════════════════════════════════════
//  SETUP  — runs once at power-on / reset
// ═════════════════════════════════════════════════════════════════════════════
void setup() {

  Serial.begin(115200);   // Always 115200 baud on ESP32

  // ── Button pins ───────────────────────────────────────────────────────────
  // INPUT mode: the pin reads whatever voltage is applied to it.
  // The external 1kΩ pull-down resistor holds it LOW until the button is pressed.
  pinMode(btnRight, INPUT);
  pinMode(btnLeft,  INPUT);

  // ── Motor direction pins ──────────────────────────────────────────────────
  pinMode(motor_IN1, OUTPUT);
  pinMode(motor_IN2, OUTPUT);

  // Both LOW = motor coasts at startup (no accidental spin at power-on)
  digitalWrite(motor_IN1, LOW);
  digitalWrite(motor_IN2, LOW);

  // ── Explicitly pull LED pins LOW ──────────────────────────────────────────
  // GPIO 27 and 14 are used by DC Motor III for direction LEDs.
  // Without this, floating input state can light them unintentionally.
  pinMode(27, OUTPUT);
  pinMode(14, OUTPUT);
  digitalWrite(27, LOW);
  digitalWrite(14, LOW);

  // ── LEDC PWM on the enable pin ────────────────────────────────────────────
  // ledcAttach(pin, frequency, resolution_bits) — ESP32 core v3 API.
  // After this call, ledcWrite(motor_ENABLE, 0–255) sets the speed.
  ledcAttach(motor_ENABLE, pwmFreq, pwmResolution);
  ledcWrite(motor_ENABLE, 0);   // Start with motor off

  Serial.println("DC Motor II ready.");
  Serial.println("RIGHT button = faster / CW    LEFT button = slower / CCW");
}


// ═════════════════════════════════════════════════════════════════════════════
//  LOOP  — repeats forever
// ═════════════════════════════════════════════════════════════════════════════
void loop() {

  unsigned long now = millis();
  // millis() returns the number of milliseconds since the board powered on.
  // We use it to check how long ago each button was last pressed.


  // ───────────────────────────────────────────────────────────────────────────
  // SECTION 1 — READ RIGHT BUTTON  (speed up / go CW)
  // ───────────────────────────────────────────────────────────────────────────
  if (digitalRead(btnRight) == HIGH) {

    // Only act if enough time has passed since the last press (debounce)
    if (now - lastPressRight > DEBOUNCE_MS) {

      // Add 1 to speedStep, but don't go above +5
      speedStep = min(speedStep + 1, NUM_STEPS);

      lastPressRight = now;   // Record the time of this press
      applyMotor();           // Update the motor immediately
      printState();           // Report to Serial Monitor
    }
  }


  // ───────────────────────────────────────────────────────────────────────────
  // SECTION 2 — READ LEFT BUTTON  (slow down / go CCW)
  // ───────────────────────────────────────────────────────────────────────────
  if (digitalRead(btnLeft) == HIGH) {

    if (now - lastPressLeft > DEBOUNCE_MS) {

      // Subtract 1 from speedStep, but don't go below -5
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
//  Reads the global speedStep and sets the motor direction + speed to match.
//  Called once after every button press.
// ═════════════════════════════════════════════════════════════════════════════
void applyMotor() {

  int absStep = abs(speedStep);          // How fast? (strip the direction sign)
  int pwmValue = speedTable[absStep];    // Look up the actual PWM value

  if (speedStep > 0) {
    // ── CLOCKWISE ──────────────────────────────────────────────────────────
    // IN1=LOW, IN2=HIGH pushes current through the motor in one direction.
    digitalWrite(motor_IN1, LOW);
    digitalWrite(motor_IN2, HIGH);
    ledcWrite(motor_ENABLE, pwmValue);

  } else if (speedStep < 0) {
    // ── COUNTER-CLOCKWISE ──────────────────────────────────────────────────
    // Flipping IN1 and IN2 reverses the current, reversing the spin.
    digitalWrite(motor_IN1, HIGH);
    digitalWrite(motor_IN2, LOW);
    ledcWrite(motor_ENABLE, pwmValue);

  } else {
    // ── STOPPED ────────────────────────────────────────────────────────────
    // Both IN pins LOW = no current through either side of the H-bridge.
    // Motor coasts to a stop (freewheels rather than hard-braking).
    digitalWrite(motor_IN1, LOW);
    digitalWrite(motor_IN2, LOW);
    ledcWrite(motor_ENABLE, 0);
  }
}


// ═════════════════════════════════════════════════════════════════════════════
//  printState()
//
//  Prints the current speed step, direction, and PWM value to Serial Monitor.
//  Useful for understanding what the sketch is doing while you press buttons.
// ═════════════════════════════════════════════════════════════════════════════
void printState() {

  int absStep  = abs(speedStep);
  int pwmValue = speedTable[absStep];

  Serial.print("Step: ");

  // Print the step number with a sign (e.g., "+3" or "-2" or " 0")
  if (speedStep > 0) Serial.print("+");
  Serial.print(speedStep);

  Serial.print("  →  ");

  if (speedStep > 0) {
    Serial.print("CW  at ");
    Serial.print(pwmValue);
    Serial.print("/255  (");
    Serial.print((pwmValue * 100) / 255);
    Serial.println("%)");

  } else if (speedStep < 0) {
    Serial.print("CCW at ");
    Serial.print(pwmValue);
    Serial.print("/255  (");
    Serial.print((pwmValue * 100) / 255);
    Serial.println("%)");

  } else {
    Serial.println("STOPPED");
  }
}
