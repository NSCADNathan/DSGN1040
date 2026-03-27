/*
  UltraSonicLEDs

  Uses an ultrasonic distance sensor (HC-SR04) to measure how far away an object is,
  then lights up different LEDs depending on that distance.
  The closer the object, the further along the LED chain lights up.
  Think of it like a proximity indicator or a theremin-style light display.

  Distance ranges:
    > 50 cm  → white LED on
    40–50 cm → red LED on
    30–40 cm → yellow LED on
    20–30 cm → green LED on
    10–20 cm → blue LED on
    < 10 cm  → all LEDs off (too close to read reliably)

  The circuit:
  - HC-SR04 TRIG  → GPIO 13
  - HC-SR04 ECHO  → GPIO 14
  - Blue LED      → GPIO 33
  - Green LED     → GPIO 32
  - Yellow LED    → GPIO 27
  - Red LED       → GPIO 26
  - White LED     → GPIO 25

  Uses the HCSR04 library by Gamegine (ESP32 compatible).

  ESP32 pin note:
  - GPIO 12 was moved to 14 (GPIO 12 is a boot strapping pin — using it can prevent startup)
  - GPIOs 9, 10, 11 avoided (connected to the SPI flash chip inside the ESP32 module)
  - GPIO 3 avoided (it's the UART receive pin, used for Serial communication)
*/


#include <HCSR04.h>
// Load the library that handles talking to the ultrasonic distance sensor.
// This library does the timing math so we don't have to.


// --- SENSOR SETUP ---

// ESP32 pin assignments:
// TRIG=13 (safe), ECHO=14 (moved from 12 which is a boot strapping pin on ESP32)
// LED pins moved off GPIOs 9/10/11 (connected to SPI flash on ESP32 — do not use)
// GPIO 3 (UART RX) also avoided

UltraSonicDistanceSensor distanceSensor(13, 14);
// Create the sensor object. Tell it which pins are TRIG and ECHO.
// TRIG (trigger) — GPIO 13 — sends out an ultrasonic pulse (like a bat's sonar ping)
// ECHO           — GPIO 14 — listens for the bounce back. The time it takes = the distance.


// --- PIN NUMBERS FOR LEDS ---

const int bled = 33;  // Blue LED   — GPIO 33 (was 11, which is an SPI flash pin on ESP32)
const int gled = 32;  // Green LED  — GPIO 32 (was 10, which is an SPI flash pin on ESP32)
const int yled = 27;  // Yellow LED — GPIO 27 (was 9,  which is an SPI flash pin on ESP32)
const int rled = 26;  // Red LED    — GPIO 26 (was 5)
const int wled = 25;  // White LED  — GPIO 25 (was 3,  which is UART RX on ESP32)


// --- VARIABLES ---

int dist;
// Stores the distance reading in centimetres.
// This gets updated every loop with a fresh measurement from the sensor.


// =============================================================================
// SETUP — runs ONE TIME when the board powers on
// =============================================================================

void setup() {

  pinMode(bled, OUTPUT);  // Blue LED pin will send electricity out
  pinMode(gled, OUTPUT);  // Green LED pin will send electricity out
  pinMode(yled, OUTPUT);  // Yellow LED pin will send electricity out
  pinMode(rled, OUTPUT);  // Red LED pin will send electricity out
  pinMode(wled, OUTPUT);  // White LED pin will send electricity out

  Serial.begin(9600);
  // Open the Serial Monitor connection so we can print distance readings.
  // Useful for seeing the exact numbers while you wave your hand in front of the sensor.
}


// =============================================================================
// LOOP — runs FOREVER, continuously, as long as the board has power
// =============================================================================

void loop() {

  dist = (distanceSensor.measureDistanceCm());
  // Ask the sensor to measure distance RIGHT NOW and return the result in centimetres.
  // The sensor sends out a sound pulse (above human hearing range), waits for the echo,
  // then calculates: distance = (time for echo to return) × speed of sound ÷ 2
  // The library handles all of that maths. We just get a number back.


  // --- DECIDE WHICH LED TO LIGHT ---
  // Each if/else block checks a distance range and turns one LED on or off.
  // Only ONE LED should be on at a time for each range.
  // The else { digitalWrite(x, LOW) } ensures each LED turns off when out of its range.

  if (dist > 50) {
    digitalWrite(wled, HIGH);   // Object is far away — turn white LED ON
  } else {
    digitalWrite(wled, LOW);    // Object is NOT far away — turn white LED OFF
  }

  if ((dist > 40) && (dist < 50)) {
    // The "&&" means BOTH conditions must be true: dist must be over 40 AND under 50.
    digitalWrite(rled, HIGH);   // Object is in the 40–50 cm zone — turn red LED ON
  } else {
    digitalWrite(rled, LOW);    // Object is NOT in this zone — red LED OFF
  }

  if ((dist > 30) && (dist < 40)) {
    digitalWrite(yled, HIGH);   // Object is in the 30–40 cm zone — yellow LED ON
  } else {
    digitalWrite(yled, LOW);    // Object is NOT in this zone — yellow LED OFF
  }

  if ((dist > 20) && (dist < 30)) {
    digitalWrite(gled, HIGH);   // Object is in the 20–30 cm zone — green LED ON
  } else {
    digitalWrite(gled, LOW);    // Object is NOT in this zone — green LED OFF
  }

  if ((dist > 10) && (dist < 20)) {
    digitalWrite(bled, HIGH);   // Object is in the 10–20 cm zone — blue LED ON
  } else {
    digitalWrite(bled, LOW);    // Object is NOT in this zone — blue LED OFF
  }

  // Note: if the object is closer than 10 cm, ALL LEDs will be OFF.
  // The HC-SR04 can struggle at very close range — readings below ~2 cm are unreliable.


  Serial.print(dist);
  // Print the current distance value to the Serial Monitor.
  // Move your hand toward and away from the sensor to see the numbers change.

  delay(100);
  // Wait 100 milliseconds (1/10th of a second) before measuring again.
  // The HC-SR04 needs a short pause between measurements or the readings get noisy.
  // 100ms gives you 10 readings per second — smooth enough for this application.
}
