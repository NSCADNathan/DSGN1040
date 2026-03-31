/*
  ╔═══════════════════════════════════════════════════════════════════════════╗
  ║         STEPPER MOTOR  —  ESP32 WROVER Version                            ║
  ║         28BYJ-48 stepper motor + ULN2003 driver board                     ║
  ║                                                                           ║
  ║  What this sketch does:                                                   ║
  ║  1. Rotates ONE full turn CLOCKWISE — slowly (1 RPM)                      ║
  ║  2. Pauses 1 second                                                       ║
  ║  3. Rotates ONE full turn COUNTER-CLOCKWISE — quickly (17 RPM)            ║
  ║  4. Pauses 1 second                                                       ║
  ║  5. Rotates ONE full turn COUNTER-CLOCKWISE again — quickly (17 RPM)      ║
  ║  6. Pauses 1 second                                                       ║
  ║  7. Repeats forever                                                       ║
  ╚═══════════════════════════════════════════════════════════════════════════╝

  ┌──────────────────────────────────────────────────────────────────────────┐
  │  HARDWARE REQUIRED                                                       │
  │  • Freenove ESP32-WROVER-DEV + GPIO Extension Board                      │
  │  • 28BYJ-48 stepper motor  (the small blue geared motor)                 │
  │  • ULN2003 driver board    (the small red/blue board the motor plugs     │
  │                             into — do NOT connect the motor directly     │
  │                             to the ESP32 GPIO pins!)                     │
  └──────────────────────────────────────────────────────────────────────────┘

  ┌──────────────────────────────────────────────────────────────────────────┐
  │  WHY YOU CANNOT WIRE THE MOTOR DIRECTLY TO THE ESP32                     │
  │                                                                          │
  │  A GPIO pin on the ESP32 WROVER can supply at most ~40mA of current.     │
  │  The 28BYJ-48 stepper motor needs ~160mA per coil to spin.               │
  │  That is four times more than a GPIO pin can safely give.                │
  │                                                                          │
  │  Connecting the motor directly to GPIO would:                            │
  │    • Not spin the motor (starved of current)                             │
  │    • Potentially destroy the ESP32's GPIO circuit permanently            │
  │                                                                          │
  │  The ULN2003 driver board solves this.  It has two separate circuits:    │
  │    1. A LOW-CURRENT side (IN1–IN4):  connected to the ESP32 GPIO pins.   │
  │       These carry only the tiny logic signal (~1mA) that says            │
  │       "turn this coil on" or "turn this coil off".                       │
  │    2. A HIGH-CURRENT side:  connected to the motor coils.                │
  │       Power here comes from the 5V rail, NOT from the GPIO.              │
  │       The ULN2003 uses the logic signal to switch that 5V power          │
  │       through the coils at the required 160mA.                           │
  │                                                                          │
  │  Think of it like a light switch in a wall:                              │
  │    The work you do moving the switch doesn't power the light.            │
  │    The mechanical movement opens and closes the circuit, which           │
  │    allows mains electricity to flow through and power the bulb.          │
  │    A transistor works the same way: the small voltage from the GPIO      │
  │    causes molecular changes inside the transistor that open or close     │
  │    the path for the larger 5V current to flow through the coils.         │
  └──────────────────────────────────────────────────────────────────────────┘

  ┌──────────────────────────────────────────────────────────────────────────┐
  │  WIRING GUIDE  —  follow the signal AND the power separately             │
  │                                                                          │
  │  CONTROL SIGNALS  (low current — GPIO telling the driver what to do)     │
  │  ─────────────────────────────────────────────────────────────────────   │
  │  GPIO 18 ──────► ULN2003 IN1                                             │
  │  GPIO 19 ──────► ULN2003 IN2                                             │
  │  GPIO 21 ──────► ULN2003 IN3                                             │
  │  GPIO 22 ──────► ULN2003 IN4                                             │
  │                                                                          │
  │  MOTOR POWER  (high current — never flows through the ESP32)             │
  │  ─────────────────────────────────────────────────────────────────────   │
  │  breadboard 5V rail ────► ULN2003 VCC  (powers the motor coils)          │
  │  ⚠️  Use the 5V rail, NOT 3.3V — the motor needs 5V to develop torque     │
  │                                                                          │
  │  MOTOR CONNECTION                                                        │
  │  ─────────────────────────────────────────────────────────────────────   │
  │  The 28BYJ-48 motor has a white JST connector on its cable.              │
  │  Plug it directly into the matching socket on the ULN2003 board.         │
  │  That's it — no individual wires needed on the motor side.               │
  │  The driver board takes care of routing the 5V power to each coil.       │
  │                                                                          │
  │  SHARED GROUND  (the two circuits must share a common reference)         │
  │  ─────────────────────────────────────────────────────────────────────   │
  │  breadboard GND rail ──────► ULN2003 GND                                 │
  │  (Without a shared ground the logic signals have no return path and      │
  │  the driver board will not respond correctly.)                           │
  └──────────────────────────────────────────────────────────────────────────┘

  ┌──────────────────────────────────────────────────────────────────────────┐
  │  STEPPER vs DC MOTOR — what's the difference?                            │
  │                                                                          │
  │  DC motor:      Spins continuously, fast, hard to control precisely.     │
  │                 Good for: fans, wheels, spinning things freely.          │
  │                                                                          │
  │  Stepper motor: Moves in tiny precise STEPS. Slow but very accurate.     │
  │                 You can tell it "rotate exactly 90°" and it will.        │
  │                 No feedback sensor needed — it counts its own steps.     │
  │                 Good for: 3D printers, CNC machines, camera sliders,     │
  │                 clock mechanisms, anything needing exact positioning.    │
  └──────────────────────────────────────────────────────────────────────────┘

  ┌──────────────────────────────────────────────────────────────────────────┐
  │  HOW STEPPER MOTORS WORK  (plain English)                                │
  │                                                                          │
  │  Inside the motor are 4 electromagnet coils arranged around a            │
  │  toothed rotor (the bit that spins).                                     │
  │                                                                          │
  │  By energising the coils ONE AT A TIME in a specific sequence,           │
  │  we magnetically "drag" the rotor around one tiny step at a time.        │
  │                                                                          │
  │  The 28BYJ-48 has internal gearing (64:1 reduction):                     │
  │  • The raw motor shaft takes 32 steps per revolution                     │
  │  • × 64 gear ratio = 2048 steps to rotate the OUTPUT shaft once          │
  │  • The Stepper library uses a slightly different measurement: 2038       │
  │    (accounting for the gearing not being exactly 64:1)                   │
  │                                                                          │
  │  So each step is: 360° / 2038 = 0.177° of output shaft rotation.         │
  │  That's incredibly precise — finer than the eye can see!                 │
  └──────────────────────────────────────────────────────────────────────────┘

  ┌──────────────────────────────────────────────────────────────────────────┐
  │  ESP32 WROVER vs ARDUINO UNO — WHAT CHANGED AND WHY                      │
  │                                                                          │
  │  The Arduino Stepper library works on ESP32 without modification!        │
  │  It uses plain digitalWrite() calls which work the same on both.         │
  │                                                                          │
  │  Pin numbers changed — and GPIO 6–11 are off-limits on the WROVER:       │
  │    Arduino:      pins 8, 9, 10, 11  (all fine on Uno)                    │
  │    ESP32 WROVER: GPIO 18, 19, 21, 22  (6–11 are reserved for flash)      │
  │                                                                          │
  │  ⚠️  PIN ORDER in the Stepper constructor:                                │
  │     Stepper(steps, IN1, IN3, IN2, IN4)  — NOT IN1, IN2, IN3, IN4!        │
  │     This criss-cross order (1,3,2,4) matches the 28BYJ-48's internal     │
  │     coil firing sequence.  Using the wrong order makes the motor         │
  │     vibrate in place instead of rotating smoothly.                       │
  │     Physical wiring is sequential (18→IN1, 19→IN2, 21→IN3, 22→IN4).      │
  │     The code just passes them in a different order to the library.       │
  │                                                                          │
  │  Serial baud: 115200 instead of 9600.                                    │
  └──────────────────────────────────────────────────────────────────────────┘

  Original code from: https://lastminuteengineers.com/28byj48-stepper-motor-arduino-tutorial/
  Modified for ESP32 WROVER and annotated by Theverant
*/

// ─────────────────────────────────────────────────────────────────────────────
// LIBRARY INCLUDE
//
// The built-in Stepper library handles the coil-firing sequence for us.
// It works on ESP32 without any modification — same #include as on Arduino.
// ─────────────────────────────────────────────────────────────────────────────
#include <Stepper.h>


// ─────────────────────────────────────────────────────────────────────────────
// MOTOR SPECIFICATION
//
// STEPS_PER_REVOLUTION = 2038
// This is how many steps the output shaft takes to complete one full 360° turn.
// It accounts for the 28BYJ-48's internal gear reduction (~64:1).
//
// If you use a different stepper motor, look up its steps per revolution
// in its datasheet.  Common values: 200 (for NEMA 17), 513, 2048.
// ─────────────────────────────────────────────────────────────────────────────
const int STEPS_PER_REVOLUTION = 2038;


// ─────────────────────────────────────────────────────────────────────────────
// STEPPER OBJECT
//
// Physical wiring (sequential and easy to follow):
//   GPIO 18 → ULN2003 IN1
//   GPIO 19 → ULN2003 IN2
//   GPIO 21 → ULN2003 IN3
//   GPIO 22 → ULN2003 IN4
//
// ⚠️  The Stepper library argument order is NOT the same as the wiring order!
// The 28BYJ-48 needs its coils energised in the sequence: IN1, IN3, IN2, IN4.
// So in the code we pass them as: 18, 21, 19, 22  (1, 3, 2, 4).
// Using the straight order (18, 19, 21, 22) makes the motor hum but not rotate.
// ─────────────────────────────────────────────────────────────────────────────
Stepper myStepper(STEPS_PER_REVOLUTION,
                   18,   // IN1 → GPIO 18
                   21,   // IN3 → GPIO 21  (library needs 1,3,2,4 order)
                   19,   // IN2 → GPIO 19
                   22);  // IN4 → GPIO 22


// ═════════════════════════════════════════════════════════════════════════════
//  SETUP  — runs once at power-on / reset
// ═════════════════════════════════════════════════════════════════════════════
void setup() {

  Serial.begin(115200);   // Open serial monitor at 115200 baud

  // The Stepper library automatically calls pinMode(pin, OUTPUT) for all
  // four coil pins.  We don't need to do it manually here.

  Serial.println("28BYJ-48 Stepper Motor — ESP32 WROVER");
  Serial.println("CW at 1 RPM, then CCW at 17 RPM");
}


// ═════════════════════════════════════════════════════════════════════════════
//  LOOP  — repeats forever
// ═════════════════════════════════════════════════════════════════════════════
void loop() {

  // ───────────────────────────────────────────────────────────────────────────
  // ROTATION 1  —  CLOCKWISE at 1 RPM  (slow)
  // ───────────────────────────────────────────────────────────────────────────

  // setSpeed(RPM) — sets the rotation speed in Revolutions Per Minute.
  //
  // For the 28BYJ-48:
  //   Minimum useful speed: 1 RPM
  //     Below this, the Stepper library's timing becomes inaccurate
  //     and the motor may stall or skip steps.
  //   Maximum safe speed: 17 RPM
  //     Above this, the motor can't keep up — the coils switch faster
  //     than the rotor can move.  The motor STALLS: it stops rotating but
  //     the coils stay energised, drawing current and generating heat.
  //     ⚠️  A stalled stepper motor will get HOT.  Don't leave it stalled!
  //
  // At 1 RPM, one full rotation takes 60 seconds.
  myStepper.setSpeed(1);

  Serial.println("Rotating CLOCKWISE — 1 RPM (slow)");
  Serial.println("(One full turn takes 60 seconds at this speed)");

  // step(+n) = rotate n steps CLOCKWISE
  // step(-n) = rotate n steps COUNTER-CLOCKWISE
  //
  // STEP BLOCKS THE SKETCH:
  // The sketch waits here — doing nothing else — until all 2038 steps
  // are complete.  At 1 RPM that is about 60 seconds.
  // This is called "blocking" code.
  // For more sophisticated (non-blocking) stepper control, look up the
  // AccelStepper library which allows other code to run simultaneously.
  myStepper.step(STEPS_PER_REVOLUTION);

  delay(1000);   // 1-second pause between rotations


  // ───────────────────────────────────────────────────────────────────────────
  // ROTATION 2  —  COUNTER-CLOCKWISE at 17 RPM  (fast)
  // ───────────────────────────────────────────────────────────────────────────

  // 17 RPM is the practical maximum for this motor.
  // At 17 RPM, one full rotation takes ~3.5 seconds.
  myStepper.setSpeed(17);   // ⚠️  Do not exceed 17 RPM for the 28BYJ-48!

  Serial.println("Rotating COUNTER-CLOCKWISE — 17 RPM (fast)  [1 of 2]");

  // Negative step count = counter-clockwise direction
  myStepper.step(-STEPS_PER_REVOLUTION);

  delay(1000);   // pause between the two CCW rotations


  // ───────────────────────────────────────────────────────────────────────────
  // ROTATION 3  —  COUNTER-CLOCKWISE at 17 RPM again  (second CCW pass)
  // ───────────────────────────────────────────────────────────────────────────
  myStepper.setSpeed(17);

  Serial.println("Rotating COUNTER-CLOCKWISE — 17 RPM (fast)  [2 of 2]");

  myStepper.step(-STEPS_PER_REVOLUTION);

  delay(1000);   // pause before loop() restarts


  // ── Power tip ─────────────────────────────────────────────────────────────
  // After step() completes, the Stepper library leaves the last coil energised.
  // This means the motor holds its position, but it also draws current (~160mA).
  // To de-energise the coils and save power, write LOW to all four pins:
  //   digitalWrite(18, LOW);
  //   digitalWrite(19, LOW);
  //   digitalWrite(21, LOW);
  //   digitalWrite(22, LOW);
  // This sketch leaves them on so the motor holds position during the delay.
}
