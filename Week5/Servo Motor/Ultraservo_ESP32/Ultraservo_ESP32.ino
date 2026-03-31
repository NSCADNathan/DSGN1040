/*
  ╔═══════════════════════════════════════════════════════════════════════════╗
  ║         ULTRASERVO  —  ESP32 WROVER Version                              ║
  ║         HC-SR04 Ultrasonic Sensor controlling a Servo Motor               ║
  ║                                                                           ║
  ║  What this sketch does:                                                   ║
  ║  - The HC-SR04 measures how far away an object (your hand, a wall)        ║
  ║    is in front of the sensor in centimetres                               ║
  ║  - That distance is mapped to a servo angle between 0° and 180°           ║
  ║  - Close hand  → servo swings one way                                    ║
  ║  - Far hand    → servo swings the other way                              ║
  ║  - It's like a radar dish that physically tracks the distance!            ║
  ╚═══════════════════════════════════════════════════════════════════════════╝

  ┌─────────────────────────────────────────────────────────────────────────┐
  │  HARDWARE REQUIRED                                                       │
  │  • Freenove ESP32-WROVER-DEV + GPIO Extension Board                      │
  │  • HC-SR04 ultrasonic distance sensor  (4 pins: VCC, TRIG, ECHO, GND)   │
  │  • SG90 or MG90S servo motor  (3 wires: signal, power, ground)           │
  │  • 1× 470Ω (or 1kΩ) resistor  — protects the ECHO pin!                  │
  └─────────────────────────────────────────────────────────────────────────┘

  ┌─────────────────────────────────────────────────────────────────────────┐
  │  WIRING GUIDE                                                            │
  │                                                                          │
  │  HC-SR04 Sensor                                                          │
  │    VCC   ──► breadboard 5V rail   (sensor runs on 5V)                   │
  │    TRIG  ──► GPIO 13              (we send pulse OUT)                   │
  │    ECHO  ──► 470Ω resistor ──────► GPIO 14  (sensor replies IN)          │
  │    GND   ──► breadboard GND rail                                        │
  │                                                                          │
  │  ⚠️  CRITICAL: The ECHO pin outputs 5V logic.                            │
  │     The ESP32 WROVER GPIO pins are 3.3V tolerant only!                   │
  │     A 5V signal on a GPIO pin will damage the chip.                      │
  │     The 470Ω resistor in series with ECHO limits the current and         │
  │     drops the voltage enough to protect the ESP32.                       │
  │     Alternatively use a proper 5V→3.3V voltage divider (two resistors). │
  │                                                                          │
  │  Servo Motor                                                             │
  │    Signal (orange/yellow wire)  ──► GPIO 25                              │
  │    Power  (red wire)            ──► breadboard 5V rail                  │
  │    Ground (brown/black wire)    ──► breadboard GND rail                 │
  └─────────────────────────────────────────────────────────────────────────┘

  ┌─────────────────────────────────────────────────────────────────────────┐
  │  ESP32 WROVER vs ARDUINO UNO — WHAT CHANGED AND WHY                     │
  │                                                                          │
  │  1. SERVO LIBRARY                                                        │
  │     Arduino:      #include <Servo.h>                                     │
  │     ESP32 WROVER: #include <ESP32Servo.h>                                │
  │                                                                          │
  │     The standard Servo.h uses AVR-specific hardware timer registers      │
  │     that don't exist on the ESP32. ESP32Servo re-implements the same     │
  │     API (attach, write, etc.) using the ESP32's LEDC peripheral.         │
  │     Install it: Arduino IDE → Sketch → Include Library →                 │
  │                 Manage Libraries → search "ESP32Servo" (Kevin Harrington) │
  │                                                                          │
  │  2. HC-SR04: NO EXTERNAL LIBRARY                                         │
  │     The original sketch used the HCSR04 library.                         │
  │     This version uses Arduino's built-in pulseIn() function instead.     │
  │     pulseIn() works natively on ESP32 and has no dependencies —          │
  │     one less library to install, one less thing to go wrong.             │
  │                                                                          │
  │  3. DIFFERENT PIN NUMBERS — and some pins are off-limits!               │
  │     GPIO 6–11 on the WROVER are used internally for flash memory.        │
  │     Original sketch used GPIO 6 for ECHO — we can't do that here.        │
  │     WROVER pins: TRIG=13, ECHO=14 (with 470Ω), SERVO=25                 │
  │                                                                          │
  │  4. SERIAL BAUD: 115200 instead of 9600                                  │
  └─────────────────────────────────────────────────────────────────────────┘

  ┌─────────────────────────────────────────────────────────────────────────┐
  │  HOW THE HC-SR04 WORKS  (the science in plain English)                   │
  │                                                                          │
  │  1. We send a 10-microsecond HIGH pulse to the TRIG pin.                 │
  │     This tells the sensor "fire now".                                    │
  │                                                                          │
  │  2. The sensor emits 8 ultrasonic bursts at 40kHz.                       │
  │     40,000 Hz is way above human hearing (max ~20,000 Hz),               │
  │     so you can't hear them — but the sensor can.                         │
  │                                                                          │
  │  3. The bursts bounce off the nearest object and return to the sensor.   │
  │                                                                          │
  │  4. The sensor holds the ECHO pin HIGH for exactly the duration          │
  │     of the sound's round trip.                                           │
  │                                                                          │
  │  5. We measure that duration with pulseIn().                             │
  │                                                                          │
  │  6. Convert to distance:                                                 │
  │     Sound travels at ~343 m/s = ~0.0343 cm per microsecond              │
  │     But the sound travels TO the object AND BACK, so we divide by 2:    │
  │       distance (cm) = duration (µs) × 0.0343 / 2                        │
  │                     ≈ duration / 58.2                                    │
  │     We use 58 as a round number.                                         │
  └─────────────────────────────────────────────────────────────────────────┘
*/

// ─────────────────────────────────────────────────────────────────────────────
// LIBRARY INCLUDE
//
// ⚠️  This MUST be ESP32Servo, not the plain Servo library!
// If you get a compile error about "No such file or directory", you need to
// install the library:
//   Arduino IDE → Sketch → Include Library → Manage Libraries
//   Search: ESP32Servo  →  Install "ESP32Servo" by Kevin Harrington
// ─────────────────────────────────────────────────────────────────────────────
#include <ESP32Servo.h>


// ─────────────────────────────────────────────────────────────────────────────
// PIN DEFINITIONS
// ─────────────────────────────────────────────────────────────────────────────
#define TRIG_PIN  13   // HC-SR04 trigger — we send a pulse OUT on this pin
#define ECHO_PIN  14   // HC-SR04 echo    — the sensor's reply pulse comes IN here
                       // Remember: wire a 470Ω resistor in series with this pin!
#define SERVO_PIN 25   // Servo signal wire (the orange or yellow wire)


// ─────────────────────────────────────────────────────────────────────────────
// CONSTANTS
// ─────────────────────────────────────────────────────────────────────────────
const int MAX_DIST_CM = 75;
// We cap distance readings at 75cm.
// The sensor can technically measure up to ~400cm, but for a desk demo
// 75cm gives a satisfying spread of servo movement.
// Anything beyond 75cm snaps to 75cm (servo fully to one side).

const long ECHO_TIMEOUT_US = 30000;
// pulseIn() timeout in microseconds (= 30ms).
// At 343 m/s, 30ms corresponds to ~5 metres of round-trip distance.
// If nothing is detected within 30ms, pulseIn() returns 0 (no echo).


// ─────────────────────────────────────────────────────────────────────────────
// SERVO OBJECT
//
// Servo myServo creates an object that represents our physical servo motor.
// The ESP32Servo library uses the LEDC peripheral internally — the same
// PWM system we used for the LED in other sketches.
// ─────────────────────────────────────────────────────────────────────────────
Servo myServo;


// ─────────────────────────────────────────────────────────────────────────────
// VARIABLES
// ─────────────────────────────────────────────────────────────────────────────
long duration;      // Duration of the ECHO pulse in microseconds
int  distanceCm;    // Calculated distance in centimetres
int  angle;         // Servo target angle (0–180°), derived from distanceCm


// ═════════════════════════════════════════════════════════════════════════════
//  SETUP  — runs once at power-on / reset
// ═════════════════════════════════════════════════════════════════════════════
void setup() {

  Serial.begin(115200);   // Open serial monitor at 115200 baud (ESP32 standard)

  // ── HC-SR04 pin modes ─────────────────────────────────────────────────────
  pinMode(TRIG_PIN, OUTPUT);   // We send pulses OUT on TRIG
  pinMode(ECHO_PIN, INPUT);    // We receive the echo pulse IN on ECHO

  // Ensure TRIG starts LOW so the sensor isn't confused at startup
  digitalWrite(TRIG_PIN, LOW);

  // ── Servo setup ───────────────────────────────────────────────────────────
  // attach(pin) tells the library which GPIO carries the servo's signal wire.
  // ESP32Servo automatically allocates a LEDC timer channel for the servo PWM.
  // Servo PWM uses a 50Hz signal (20ms period) — standard for hobby servos.
  // Pulse width within that period sets the angle:
  //   ~0.5ms pulse  →   0° (fully clockwise)
  //   ~1.5ms pulse  →  90° (centre)
  //   ~2.5ms pulse  → 180° (fully counter-clockwise)
  myServo.attach(SERVO_PIN);

  // Move servo to 90° (centre position) at startup
  myServo.write(90);

  Serial.println("Ultraservo ready!");
  Serial.println("Wave your hand in front of the sensor.");
}


// ═════════════════════════════════════════════════════════════════════════════
//  LOOP  — repeats ~20 times per second (limited by the 50ms delay at the end)
// ═════════════════════════════════════════════════════════════════════════════
void loop() {

  // ───────────────────────────────────────────────────────────────────────────
  // STEP 1  —  TRIGGER the HC-SR04
  // Send a clean 10-microsecond HIGH pulse to tell the sensor to fire
  // ───────────────────────────────────────────────────────────────────────────

  // Pull TRIG LOW for 2µs first to clear any residual signal
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  // Now pulse HIGH for exactly 10 microseconds
  // This is the sensor's required trigger pulse — anything shorter won't work
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);   // End the pulse — sensor fires its ultrasonic burst


  // ───────────────────────────────────────────────────────────────────────────
  // STEP 2  —  MEASURE the echo pulse
  //
  // pulseIn(pin, level, timeout)
  //   pin     = which pin to monitor
  //   level   = wait for it to go HIGH, then measure how long it stays HIGH
  //   timeout = give up after this many microseconds if no pulse arrives
  //
  // Returns: the duration in microseconds, or 0 if timed out
  // ───────────────────────────────────────────────────────────────────────────
  duration = pulseIn(ECHO_PIN, HIGH, ECHO_TIMEOUT_US);


  // ───────────────────────────────────────────────────────────────────────────
  // STEP 3  —  CONVERT duration → distance in centimetres
  //
  // Physics: speed of sound ≈ 343 m/s = 0.0343 cm/µs
  // The pulse travels TO the object and BACK, so divide by 2.
  //   distance = duration × 0.0343 / 2
  //            = duration / 58.2
  //   We use 58 as a convenient integer approximation.
  // ───────────────────────────────────────────────────────────────────────────
  distanceCm = (int)(duration / 58);

  // Sanitise the reading:
  //   duration = 0 means pulseIn() timed out (nothing detected)
  //   We also clamp readings above MAX_DIST_CM
  if (distanceCm <= 0 || distanceCm > MAX_DIST_CM) {
    distanceCm = MAX_DIST_CM;   // Treat out-of-range as "maximum distance"
  }


  // ───────────────────────────────────────────────────────────────────────────
  // STEP 4  —  MAP distance to a servo angle
  //
  // map(value, fromLow, fromHigh, toLow, toHigh)
  //   This built-in Arduino function re-scales a number from one range to another.
  //   Here we re-scale: 2cm–75cm  →  180°–0°
  //
  //   Why inverted (180 to 0 instead of 0 to 180)?
  //   So that bringing your hand CLOSER makes the servo swing FURTHER.
  //   It feels more natural — like the servo is "pointing away" from your hand.
  //   Swap the 180 and 0 if you prefer the opposite behaviour.
  // ───────────────────────────────────────────────────────────────────────────
  angle = map(distanceCm, 2, MAX_DIST_CM, 180, 0);


  // ───────────────────────────────────────────────────────────────────────────
  // STEP 5  —  COMMAND the servo
  //
  // myServo.write(angle) sends the servo to the requested angle.
  // Valid range: 0 to 180 degrees.
  // The servo moves at its own pace — we don't need to wait for it.
  // ───────────────────────────────────────────────────────────────────────────
  myServo.write(angle);


  // ── Serial Monitor output for debugging and curiosity ─────────────────────
  Serial.print("Distance: ");
  Serial.print(distanceCm);
  Serial.print(" cm");
  Serial.print("   →   Servo angle: ");
  Serial.print(angle);
  Serial.println("°");


  // Wait 50ms before the next measurement cycle.
  // 50ms = 20 readings per second.  Fast enough for smooth servo tracking,
  // but not so fast that the servo is constantly fighting itself.
  // If you make this shorter than ~20ms the servo will jitter noticeably.
  delay(50);
}
