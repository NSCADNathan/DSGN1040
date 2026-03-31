/*
  74HC595 Marquee Test
  Simple "Cylon" or "Knight Rider" bounce to test wiring.
  
Double check this is correct:
  - Data (DS): GPIO 14
  - Clock (SHCP): GPIO 12
  - Latch (STCP): GPIO 13
*/

#include <ShiftRegister74HC595.h>

// Create shift register object (1 chip, data=14, clock=12, latch=13)
ShiftRegister74HC595<1> sr(14, 12, 13);

void setup() {
  Serial.begin(115200);
  Serial.println("Starting Marquee Test...");
}

void loop() {
  // Move LED from 0 to 7 (Left to Right)
  for (int i = 0; i < 8; i++) {
    sr.setAllLow();      // Clear all LEDs
    sr.set(i, HIGH);     // Turn on one LED
    delay(100);          // Speed of the marquee
  }

  // Move LED from 6 down to 1 (Right to Left)
  for (int i = 6; i > 0; i--) {
    sr.setAllLow();
    sr.set(i, HIGH);
    delay(100);
  }
}