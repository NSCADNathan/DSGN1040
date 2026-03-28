/*
  Joystick 1

  Reads all three outputs from a joystick module and prints them
  to the Serial Monitor so you can see what the joystick is doing in real time.

  A joystick module has three outputs:
    - X axis:  an analog signal that changes as you push left/right (0–4095)
    - Y axis:  an analog signal that changes as you push up/down (0–4095)
    - Switch:  a digital button — pressing straight down on the stick clicks it

  The circuit:
  - Joystick SW  (switch) → GPIO 25
  - Joystick VRx (X axis) → GPIO 34
  - Joystick VRy (Y axis) → GPIO 35

  Originally from www.elegoo.com, 2016.12.09
  Modified by Theverant, 2020. COVID19 sucks!
  Refactored for ESP32.
*/


// --- PIN NUMBERS ---
// ESP32 GPIO pin numbers

const int SW_pin = 25;  // GPIO 25 — digital pin, reads the joystick's click button
const int X_pin  = 34;  // GPIO 34 — analog input, reads left/right position (ADC input only)
const int Y_pin  = 35;  // GPIO 35 — analog input, reads up/down position (ADC input only)
// Note: GPIO 34 and 35 on the ESP32 are INPUT ONLY — you can read from them but
// not send signals out through them. Perfect for sensors and joysticks.


// =============================================================================
// SETUP — runs ONE TIME when the board powers on
// =============================================================================

void setup() {

  pinMode(SW_pin, INPUT_PULLUP);
  // Set the switch pin as an input.
  // INPUT_PULLUP is a special mode: the pin reads HIGH by default (even when nothing is happening).
  // When the button IS pressed, it reads LOW.
  // This is the opposite of what you might expect! It's how pull-up resistors work —
  // the "pull-up" keeps the signal high until something pulls it to ground (LOW).

  Serial.begin(9600);
  // Start the Serial Monitor connection at 9600 baud.
  // This lets us print text back to the computer and read it in the IDE.
  // Think of it like the board texting its sensor readings to your screen.
}


// =============================================================================
// LOOP — runs FOREVER, continuously, as long as the board has power
// =============================================================================

void loop() {

  Serial.print("Switch:  ");
  // Print the label "Switch:  " to the Serial Monitor.
  // Serial.print() prints WITHOUT a new line at the end (the next print continues on the same line).

  Serial.println(digitalRead(SW_pin));
  // Read the joystick button and print its value.
  // digitalRead returns HIGH (1) or LOW (0).
  // Because we used INPUT_PULLUP: 1 = not pressed, 0 = pressed (yes, it's backwards).
  // Serial.println() prints the value AND then moves to a new line.

  Serial.print("X-axis: ");
  // Print the label for the X axis reading.

  Serial.println(analogRead(X_pin));
  // analogRead() reads an analog voltage and converts it to a number.
  // Unlike digitalRead (which only gives 0 or 1), analogRead gives you a range.
  // On the ESP32, that range is 0 to 4095 (12-bit resolution).
  //   0    = joystick pushed fully to one side
  //   2048 = joystick centered (roughly)
  //   4095 = joystick pushed fully to the other side

  Serial.print("Y-axis: ");
  // Print the label for the Y axis reading.

  Serial.println(analogRead(Y_pin));
  // Same as X — reads a number from 0 to 4095 based on the joystick's up/down position.

  Serial.print("\n\n");
  // Print two blank lines.
  // "\n" is a special code meaning "new line" (like pressing Enter).
  // Two of them puts a gap between each set of readings, making it easier to read.

  delay(10);
  // Wait 10 milliseconds before the loop runs again.
  // Without a delay, readings would scroll past so fast you couldn't read them.
  // 10ms = 100 readings per second — fast enough to feel real-time, slow enough to read.
}
