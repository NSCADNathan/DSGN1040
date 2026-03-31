/*
  ╔═══════════════════════════════════════════════════════════════════════════╗
  ║        DC MOTOR IV  —  Potentiometer Joystick Speed & Direction           ║
  ║                        ESP32 WROVER Version                               ║
  ║               Using L293D H-Bridge Motor Driver IC                       ║
  ║                                                                           ║
  ║  What this sketch does:                                                   ║
  ║  A single potentiometer controls both speed AND direction —               ║
  ║  like a joystick for the motor.                                          ║
  ║                                                                           ║
  ║  Centre position  → motor stopped                                        ║
  ║  Turn RIGHT of centre → clockwise, faster as you go further right        ║
  ║  Turn LEFT of centre  → counter-clockwise, faster as you go further left ║
  ║                                                                           ║
  ║  CW LED  (GPIO 27) — lit when spinning CW,  brightness = speed           ║
  ║  CCW LED (GPIO 14) — lit when spinning CCW, brightness = speed           ║
  ╚═══════════════════════════════════════════════════════════════════════════╝

  ┌─────────────────────────────────────────────────────────────────────────┐
  │  HARDWARE REQUIRED                                                       │
  │  • Freenove ESP32-WROVER-DEV + GPIO Extension Board                      │
  │  • L293D H-Bridge Motor Driver IC                                        │
  │  • Small DC motor                                                        │
  │  • 1× potentiometer (10kΩ typical)                                      │
  │  • 2× LEDs  (CW and CCW direction indicators)                           │
  │  • 2× 220Ω resistors  (one per LED)                                     │
  └─────────────────────────────────────────────────────────────────────────┘

  ┌─────────────────────────────────────────────────────────────────────────┐
  │  WIRING GUIDE                                                            │
  │                                                                          │
  │  POTENTIOMETER                                                           │
  │    Left leg          ──► GND rail                                        │
  │    Wiper (middle leg) ──► GPIO 34                                        │
  │    Right leg         ──► 3.3V rail                                       │
  │                                                                          │
  │    The pot doesn't need to be on the breadboard — use port-to-pin       │
  │    jumper cables directly from the pot legs to GPIO 34, 3.3V and GND.   │
  │                                                                          │
  │  DIRECTION LEDs  (same as DC Motor III)                                 │
  │    CW  LED: GPIO 27 ──► 220Ω ──► LED long leg ──► LED short leg ──► GND rail │
  │    CCW LED: GPIO 14 ──► 220Ω ──► LED long leg ──► LED short leg ──► GND rail │
  │                                                                          │
  │  MOTOR CONTROL  (L293D H-Bridge — same as DC Motor I, II, III)         │
  │  GPIO 32  ──────►  IN1  (pin 2)   — direction control A                 │
  │  GPIO 33  ──────►  IN2  (pin 7)   — direction control B                 │
  │  GPIO 23  ──────►  EN1  (pin 1)   — enable + speed (PWM signal)         │
  │  GND      ──────►  GND  (pins 4, 5, 12, 13)                             │
  │  OUT1 (pin 3)  ───► Motor terminal A                                     │
  │  OUT2 (pin 6)  ───► Motor terminal B                                     │
  │  VCC1 (pin 16) ──► breadboard 3.3V rail  (L293D logic power)            │
  │  VCC2 (pin 8)  ──► breadboard 5V rail    (motor coil power)             │
  │                    ⚠️  VCC2 must be 5V — motor won't run on 3.3V.       │
  │                                                                          │
  │  ⚠️  POWER NOTE: run the motor from the extension board barrel jack      │
  │     (7–9V wall adapter), not USB alone. USB can't supply enough current  │
  │     for the motor at speed and will cause the board to reset.            │
  │     Plug USB in first (to get serial port), then plug in the barrel jack.│
  └─────────────────────────────────────────────────────────────────────────┘

  ┌─────────────────────────────────────────────────────────────────────────┐
  │  HOW THE POT JOYSTICK WORKS                                             │
  │                                                                          │
  │  The pot gives a value from 0 (full left) to 4095 (full right).         │
  │  The centre is approximately 2048.                                       │
  │                                                                          │
  │  We split the range into three zones:                                   │
  │                                                                          │
  │  0 ────────── 1848 ┃ dead zone ┃ 2248 ────────── 4095                  │
  │  ◄── CCW faster ──►┃  STOPPED  ┃◄─── CW faster ──►                     │
  │                                                                          │
  │  In the dead zone (around centre): motor stops.                         │
  │  Outside the dead zone: the further from centre, the faster it goes.   │
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
  │  EN1 (GPIO 23, PWM): 0 = stopped   ~170 = slow   ~190 = safe maximum   │
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
  │                                                                          │
  │  analogRead(pin) works on ESP32 the same as Arduino — but returns       │
  │  0–4095 (12-bit) instead of 0–1023 (10-bit on Uno).                    │
  └─────────────────────────────────────────────────────────────────────────┘
*/


// ─────────────────────────────────────────────────────────────────────────────
// PIN DEFINITIONS
// ─────────────────────────────────────────────────────────────────────────────
const int potPin       = 34;   // Potentiometer wiper — input only ADC pin
const int ledCW        = 27;   // Clockwise indicator LED
const int ledCCW       = 14;   // Counter-clockwise indicator LED
const int motor_IN1    = 32;   // L293D direction control A
const int motor_IN2    = 33;   // L293D direction control B
const int motor_ENABLE = 23;   // L293D enable + speed (PWM)


// ─────────────────────────────────────────────────────────────────────────────
// LEDC PWM SETTINGS
// ─────────────────────────────────────────────────────────────────────────────
const int motorPwmFreq  = 30000;   // 30kHz — silent motor operation
const int ledPwmFreq    = 5000;    // 5kHz  — smooth LED dimming
const int pwmResolution = 8;       // 8-bit: 0–255


// ─────────────────────────────────────────────────────────────────────────────
// POT JOYSTICK SETTINGS
//
// POT_CENTRE: the ADC value when the pot is in the middle (approx 2048).
// POT_DEAD:   how far either side of centre counts as "stopped".
//             Increase this if the motor creeps when the pot is centred.
// SPEED_MIN:  lowest PWM value that reliably overcomes motor friction.
// SPEED_MAX:  safe ceiling over USB power. Raise if using external power.
// ─────────────────────────────────────────────────────────────────────────────
const int POT_CENTRE = 2048;
const int POT_DEAD   = 200;    // Dead zone: centre ± 200 counts
const int SPEED_MIN  = 150;
const int SPEED_MAX  = 190;


// ═════════════════════════════════════════════════════════════════════════════
//  SETUP
// ═════════════════════════════════════════════════════════════════════════════
void setup() {

  // Let the power rail stabilise before the motor starts.
  delay(2000);

  Serial.begin(115200);

  // ── Motor direction pins ──────────────────────────────────────────────────
  pinMode(motor_IN1, OUTPUT);
  pinMode(motor_IN2, OUTPUT);
  digitalWrite(motor_IN1, LOW);
  digitalWrite(motor_IN2, LOW);

  // ── LED pins — explicitly LOW to prevent floating at startup ──────────────
  pinMode(ledCW,  OUTPUT);
  pinMode(ledCCW, OUTPUT);
  digitalWrite(ledCW,  LOW);
  digitalWrite(ledCCW, LOW);

  // ── LEDC PWM ──────────────────────────────────────────────────────────────
  ledcAttach(motor_ENABLE, motorPwmFreq, pwmResolution);
  ledcWrite(motor_ENABLE, 0);

  ledcAttach(ledCW,  ledPwmFreq, pwmResolution);
  ledcAttach(ledCCW, ledPwmFreq, pwmResolution);
  ledcWrite(ledCW,  0);
  ledcWrite(ledCCW, 0);

  Serial.println("DC Motor IV ready. Turn pot to control speed and direction.");
}


// ═════════════════════════════════════════════════════════════════════════════
//  LOOP
// ═════════════════════════════════════════════════════════════════════════════
void loop() {

  // ── Read pot ──────────────────────────────────────────────────────────────
  int potValue = analogRead(potPin);
  // potValue is 0–4095. Centre (~2048) = stopped.

  int deviation = potValue - POT_CENTRE;
  // deviation > 0 means pot is right of centre → CW
  // deviation < 0 means pot is left of centre  → CCW
  // abs(deviation) tells us how far from centre = how fast

  if (abs(deviation) <= POT_DEAD) {
    // ── DEAD ZONE: stopped ────────────────────────────────────────────────
    ledcWrite(motor_ENABLE, 0);
    digitalWrite(motor_IN1, LOW);
    digitalWrite(motor_IN2, LOW);
    ledcWrite(ledCW,  0);
    ledcWrite(ledCCW, 0);

  } else if (deviation > 0) {
    // ── RIGHT OF CENTRE: clockwise ────────────────────────────────────────
    int absDeviation = deviation;

    // Motor speed: maps pot position to the safe PWM range for the motor.
    int speed = map(absDeviation, POT_DEAD, POT_CENTRE, SPEED_MIN, SPEED_MAX);
    speed = constrain(speed, SPEED_MIN, SPEED_MAX);

    // LED brightness: maps the same pot position to the full 0–255 range.
    // This gives a wide visual sweep even though the motor speed range is narrow.
    int brightness = map(absDeviation, POT_DEAD, POT_CENTRE, 0, 255);
    brightness = constrain(brightness, 0, 255);

    digitalWrite(motor_IN1, LOW);
    digitalWrite(motor_IN2, HIGH);
    ledcWrite(motor_ENABLE, speed);
    ledcWrite(ledCW,  brightness);
    ledcWrite(ledCCW, 0);

    Serial.print("CW  motor: ");
    Serial.print(speed);
    Serial.print("/255  LED: ");
    Serial.print(brightness);
    Serial.print("/255  pot: ");
    Serial.println(potValue);

  } else {
    // ── LEFT OF CENTRE: counter-clockwise ────────────────────────────────
    int absDeviation = abs(deviation);

    int speed = map(absDeviation, POT_DEAD, POT_CENTRE, SPEED_MIN, SPEED_MAX);
    speed = constrain(speed, SPEED_MIN, SPEED_MAX);

    int brightness = map(absDeviation, POT_DEAD, POT_CENTRE, 0, 255);
    brightness = constrain(brightness, 0, 255);

    digitalWrite(motor_IN1, HIGH);
    digitalWrite(motor_IN2, LOW);
    ledcWrite(motor_ENABLE, speed);
    ledcWrite(ledCW,  0);
    ledcWrite(ledCCW, brightness);

    Serial.print("CCW motor: ");
    Serial.print(speed);
    Serial.print("/255  LED: ");
    Serial.print(brightness);
    Serial.print("/255  pot: ");
    Serial.println(potValue);
  }

  delay(20);   // Small delay to avoid flooding the serial monitor
}
