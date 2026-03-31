/*
  ╔═══════════════════════════════════════════════════════════════════════════╗
  ║               DC MOTOR CONTROL  —  ESP32 WROVER Version                      ║
  ║               Using L293D H-Bridge Motor Driver IC                       ║
  ║                                                                           ║
  ║  What this sketch does:                                                   ║
  ║  1. Spins the motor FORWARD at full speed for 2 seconds                  ║
  ║  2. Stops for 1 second                                                    ║
  ║  3. Spins the motor BACKWARD at full speed for 2 seconds                 ║
  ║  4. Stops for 1 second                                                    ║
  ║  5. Slowly ramps the motor from ~78% speed up to 100% speed              ║
  ║  6. Repeats forever                                                       ║
  ╚═══════════════════════════════════════════════════════════════════════════╝

  ┌─────────────────────────────────────────────────────────────────────────┐
  │  WHY DO WE NEED AN L293D?  (The H-Bridge chip)                          │
  │                                                                          │
  │  A GPIO pin on the ESP32 WROVER can only supply about 40mA of current.      │
  │  A DC motor typically needs 100–600mA to spin.                          │
  │  Connecting a motor directly to GPIO would:                              │
  │    a) Not spin the motor (not enough current)                            │
  │    b) Potentially destroy your ESP32                                     │
  │                                                                          │
  │  The L293D is a "motor driver" — it takes the tiny GPIO signal as an    │
  │  instruction, then uses a separate power supply to actually drive the    │
  │  motor at the required current.                                          │
  │                                                                          │
  │  Think of it like a light switch in a wall:                             │
  │    The work you do moving the switch doesn't power the light —          │
  │    the mechanical movement causes physical changes that open or close   │
  │    the circuit, allowing mains electricity to flow through the bulb.    │
  │    A transistor works the same way: the small voltage from the GPIO     │
  │    causes molecular changes inside the transistor that open or close    │
  │    the path for the larger 5V current to flow through the motor.        │
  │                                                                          │
  │  The L293D is also an "H-Bridge":                                        │
  │  Two sets of transistors arranged in an "H" shape can push current       │
  │  through the motor in EITHER direction by flipping which transistors     │
  │  are open/closed. That's how we reverse the motor without rewiring.      │
  └─────────────────────────────────────────────────────────────────────────┘

  ┌─────────────────────────────────────────────────────────────────────────┐
  │  WIRING GUIDE                                                            │
  │                                                                          │
  │  ESP32 WROVER          L293D pin                                             │
  │  ────────────────────────────────────────                                │
  │  GPIO 32  ──────►  IN1  (pin 2)   — direction control A                 │
  │  GPIO 33  ──────►  IN2  (pin 7)   — direction control B                 │
  │  GPIO 23  ──────►  EN1  (pin 1)   — enable + speed (PWM signal)         │
  │  GND      ──────►  GND  (pins 4, 5, 12, 13)                             │
  │                                                                          │
  │  L293D output      Motor / Power                                         │
  │  ────────────────────────────────────────                                │
  │  OUT1 (pin 3) ───► Motor terminal A                                      │
  │  OUT2 (pin 6) ───► Motor terminal B                                      │
  │  VCC1 (pin 16) ──► breadboard 3.3V rail  (powers the L293D logic)       │
  │  VCC2 (pin 8)  ──► breadboard 5V rail    (powers the motor coils)       │
  │                    ⚠️  VCC2 must be 5V — the motor won't run on 3.3V.   │
  │                        The extension board puts 5V on the bottom rail.  │
  └─────────────────────────────────────────────────────────────────────────┘

  ┌─────────────────────────────────────────────────────────────────────────┐
  │  DIRECTION LOGIC TABLE (H-Bridge truth table)                           │
  │                                                                          │
  │  IN1 (GPIO 32) │ IN2 (GPIO 33) │ Motor behaviour                        │
  │  ──────────────┼───────────────┼──────────────────────────              │
  │  LOW           │ HIGH          │ Spins FORWARD                           │
  │  HIGH          │ LOW           │ Spins BACKWARD                          │
  │  LOW           │ LOW           │ Coasts to a stop  (freewheels)          │
  │  HIGH          │ HIGH          │ Brakes hard  (avoid — stresses motor)   │
  │                                                                          │
  │  EN1 (enable pin, controlled by PWM):                                   │
  │    0   (0% duty)   → Motor disabled — won't spin regardless of IN1/IN2  │
  │    255 (100% duty) → Motor runs at full power in whatever direction      │
  │    1–254           → Motor runs at proportional partial speed            │
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
  │  We set freq to 30kHz — above human hearing — so the motor runs         │
  │  silently. analogWrite() would work but the motor would whine.          │
  └─────────────────────────────────────────────────────────────────────────┘
*/


// ─────────────────────────────────────────────────────────────────────────────
// PIN DEFINITIONS
//
// These three GPIO pins connect to the L293D motor driver.
// You can change these to any PWM-capable GPIO pins on your ESP32 WROVER.
// ─────────────────────────────────────────────────────────────────────────────
const int motor_IN1   = 32;   // L293D IN1 — one side of the H-bridge direction control
const int motor_IN2   = 33;   // L293D IN2 — other side (set opposite to IN1 to spin)
const int motor_ENABLE = 23;  // L293D EN1 — enable pin, we send PWM here to control speed


// ─────────────────────────────────────────────────────────────────────────────
// LEDC PWM SETTINGS
//
// The enable pin receives a PWM (pulse-width modulation) signal.
// PWM rapidly switches the pin ON and OFF. The fraction of time it's ON
// is the "duty cycle". More ON time = faster motor.
//
// Frequency 30000 Hz = 30kHz
//   This is above human hearing (20kHz), so the motor won't produce an
//   audible high-pitched whine the way lower frequencies (e.g., 1kHz) do.
//
// Resolution 8-bit: duty cycle values 0 to 255
//   0   =   0% on = motor off
//   127 =  50% on = motor at half speed
//   255 = 100% on = motor at full speed
// ─────────────────────────────────────────────────────────────────────────────
const int pwmFreq       = 30000;   // PWM frequency: 30kHz (above human hearing range)
const int pwmResolution = 8;       // 8-bit: 256 possible speed values (0–255)


// ─────────────────────────────────────────────────────────────────────────────
// SPEED RAMP VARIABLE
//
// Used in the ramp-up demo at the end of loop().
// Starts at 170 (~67%) — low enough to show a ramp, high enough to actually spin.
// Capped at 200 (~78%) to avoid drawing enough current to brown out the ESP32 over USB.
// ─────────────────────────────────────────────────────────────────────────────
int dutyCycle = 170;


// ═════════════════════════════════════════════════════════════════════════════
//  SETUP  — runs once at power-on / reset
// ═════════════════════════════════════════════════════════════════════════════
void setup() {

  // Let the power rail stabilise before doing anything.
  // Without this, the board can brown out the moment the motor starts
  // on the first loop() iteration, corrupting the flash.
  delay(2000);

  Serial.begin(115200);   // Open serial monitor — use 115200 baud on ESP32

  // ── Direction control pins ────────────────────────────────────────────────>>
  // IN1 and IN2 are plain digital outputs — HIGH or LOW.
  // The L293D reads these to decide which way to spin.
  pinMode(motor_IN1, OUTPUT);
  pinMode(motor_IN2, OUTPUT);

  // Start with both IN pins LOW so the motor doesn't spin at power-on
  digitalWrite(motor_IN1, LOW);
  digitalWrite(motor_IN2, LOW);

  // ── LEDC PWM setup on the enable pin ─────────────────────────────────────>>
  // ledcAttach(pin, frequency, resolution_bits)
  //
  // This is the ESP32 Arduino core v3 method.
  // It replaces the two-step v2 approach:
  //   v2 (old): ledcSetup(channel, freq, resolution) + ledcAttachPin(pin, channel)
  //   v3 (new): ledcAttach(pin, freq, resolution)   — simpler, no channel numbers
  //
  // After this call, ledcWrite(motor_ENABLE, 0–255) controls the motor speed.
  ledcAttach(motor_ENABLE, pwmFreq, pwmResolution);

  Serial.println("DC Motor test starting!");
  Serial.println("Watch the motor: forward → stop → backward → stop → speed ramp");
  pinMode(27, OUTPUT);  // CW LED — drive LOW so it doesn't float on
pinMode(14, OUTPUT);  // CCW LED — drive LOW so it doesn't float on
digitalWrite(27, LOW);
digitalWrite(14, LOW);

// ── Make sure the LEDs are not used or on ────────────────────────────────────>>
pinMode(27, OUTPUT);  // CW LED — drive LOW so it doesn't float on
pinMode(14, OUTPUT);  // CCW LED — drive LOW so it doesn't float on
digitalWrite(27, LOW);
digitalWrite(14, LOW);

}


// ═════════════════════════════════════════════════════════════════════════════
//  LOOP  — repeats forever
// ═════════════════════════════════════════════════════════════════════════════
void loop() {

  // ───────────────────────────────────────────────────────────────────────────
  // STEP 1  —  FULL SPEED FORWARD for 2 seconds
  // ───────────────────────────────────────────────────────────────────────────
  Serial.println("Moving FORWARD at full speed");

  // Set full speed: 255 = 100% duty cycle = maximum power to the motor
  ledcWrite(motor_ENABLE, 170);

  // Direction: IN1=LOW, IN2=HIGH → Forward
  // (Which direction is "forward" depends on how you wired the motor terminals.
  //  If it spins the wrong way, swap IN1 and IN2, or swap the motor wires.)
  digitalWrite(motor_IN1, LOW);
  digitalWrite(motor_IN2, HIGH);

  delay(2000);   // Run for 2000 milliseconds = 2 seconds


  // ───────────────────────────────────────────────────────────────────────────
  // STEP 2  —  STOP for 1 second
  // ───────────────────────────────────────────────────────────────────────────
  Serial.println("STOPPED (coasting)");

  // Kill enable first, then cut direction.
  // Dropping EN to 0 before changing IN pins avoids a back-EMF spike
  // that can draw enough current to brown out the board over USB power.
  ledcWrite(motor_ENABLE, 0);
  digitalWrite(motor_IN1, LOW);
  digitalWrite(motor_IN2, LOW);

  delay(1000);   // Wait 1 second before reversing


  // ───────────────────────────────────────────────────────────────────────────
  // STEP 3  —  FULL SPEED BACKWARD for 2 seconds
  // ───────────────────────────────────────────────────────────────────────────
  Serial.println("Moving BACKWARD at full speed");

  ledcWrite(motor_ENABLE, 170);

  // Direction flipped: IN1=HIGH, IN2=LOW → Backward
  // The current through the motor is reversed, so the shaft spins the other way.
  digitalWrite(motor_IN1, HIGH);
  digitalWrite(motor_IN2, LOW);

  delay(2000);


  // ───────────────────────────────────────────────────────────────────────────
  // STEP 4  —  STOP for 1 second
  // ───────────────────────────────────────────────────────────────────────────
  Serial.println("STOPPED");

  ledcWrite(motor_ENABLE, 0);
  digitalWrite(motor_IN1, LOW);
  digitalWrite(motor_IN2, LOW);

  delay(1000);


  // ───────────────────────────────────────────────────────────────────────────
  // STEP 5  —  SPEED RAMP: gradually accelerate FORWARD
  //
  // This demonstrates how PWM gives us analogue-style speed control over
  // a digital device.  Instead of on/off we get a smooth range of speeds.
  // ───────────────────────────────────────────────────────────────────────────
  Serial.println("Speed ramp: gradual acceleration FORWARD");

  // Set direction to forward before starting the ramp
  digitalWrite(motor_IN1, HIGH);
  digitalWrite(motor_IN2, LOW);

  // Count dutyCycle up from 170 to 200 in steps of 5.
  // Starting at 170 (~67%) ensures the motor has enough torque to start spinning.
  // Capped at 200 (~78%) — going higher draws enough current to brown out the ESP32 over USB.
  while (dutyCycle <= 190) {

    ledcWrite(motor_ENABLE, dutyCycle);   // Apply current speed to the motor

    // Print a human-readable progress report to the Serial Monitor
    Serial.print("  Speed: ");
    Serial.print(dutyCycle);
    Serial.print("/255  (");
    Serial.print((dutyCycle * 100) / 200);   // Convert to percentage
    Serial.println("%)");

    dutyCycle += 5;   // Increase speed by ~2% per step

    delay(500);        // Wait 0.5 sec between steps
                       // Total ramp time ≈ (255-200)/5 × 0.5s ≈ 5.5 seconds
  }

  // Reset dutyCycle for the next time loop() runs from the top
  dutyCycle = 200;
}
