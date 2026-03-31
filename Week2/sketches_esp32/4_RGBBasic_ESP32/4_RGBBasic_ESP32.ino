/* Control RGB LED! (common anode)

 >> Created by Theverant. Public domain, feel the love. Changes to original code noted with ʇ

 */

// pins R→26, G→27, B→14
// common anode: HIGH=off, LOW=on, PWM inverted

const int Red   = 26;
const int Green = 27;
const int Blue  = 14;

int periode = 2000;
long pwmTime = 0;
int value;
int ledState = LOW;
long previousMillis = 0;
long interval = 1000;

void setup() {
  pinMode(Red,   OUTPUT);
  pinMode(Green, OUTPUT);
  pinMode(Blue,  OUTPUT);
  digitalWrite(Red,   HIGH);
  digitalWrite(Green, HIGH);
  digitalWrite(Blue,  HIGH);
}

void loop() {
  unsigned long currentMillis = millis();

  if(currentMillis - previousMillis > interval) {
    previousMillis = currentMillis;
    if (ledState == LOW)
      ledState = HIGH;
    else
      ledState = LOW;
    digitalWrite(Red, ledState);
  }

  pwmTime = millis();
  value = 128 + 127 * cos(2 * PI / periode * pwmTime);

  analogWrite(Green, 255 - value); // ʇ inverted for common anode
  analogWrite(Blue,  value);       // ʇ inverted for common anode
}
