/* Control 3 LEDS!

 >> Created by Theverant. Public domain, feel the love. Changes to original code noted with ʇ

 */

// pins 13→12, 11→27

const int ledPin = 12;
const int pwmPin = 27;
int brightness = 0;

int periode = 2000;
int ledState = LOW;
long previousMillis = 0;
long interval = 1000;
long pwmTime = 0;
int value;

void setup() {
  pinMode(ledPin, OUTPUT);
  pinMode(pwmPin, OUTPUT);
}

void loop() {
  unsigned long currentMillis = millis();

  if(currentMillis - previousMillis > interval) {
    previousMillis = currentMillis;
    if (ledState == LOW)
      ledState = HIGH;
    else
      ledState = LOW;
    digitalWrite(ledPin, ledState);
  }

  pwmTime = millis();
  value = 128+127*cos(2*PI/periode*pwmTime);
  analogWrite(pwmPin, value);
}
