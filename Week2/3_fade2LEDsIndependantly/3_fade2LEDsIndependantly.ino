/* Control 3 LEDS!

 >> Created by Theverant. Public domain, feel the love. Changes to original code noted with ʇ

 */

// constants won't change. Used here to
// set pin numbers:
const int ledPin = 13; // the number of the LED pin
const int pwmPin = 11; // ʇ the number of the PMW LED pin
int brightness = 0; // ʇ how bright the LED is


int periode = 2000; // ʇ period of PWM fading in and out

// Variables will change:
int ledState = LOW; // ledState used to set the LED

long previousMillis = 0; // will store last time LED was updated

// the follow variables is a long because the time, measured in miliseconds,
// will quickly become a bigger number than can be stored in an int.
long interval = 1000; // interval at which to blink (milliseconds)
long pwmTime = 0; // ʇ pwm timer
int value; // ʇ holder value for pwm value

void setup() {
  // set the digital pin as output:
  pinMode(ledPin, OUTPUT);
  pinMode(pwmPin, OUTPUT); // ʇ added pin
}

void loop()
{
  // here is where you'd put code that needs to be running all the time.
  // check to see if it's time to blink the LED; that is, if the
  // difference between the current time and last time you blinked
  // the LED is bigger than the interval at which you want to
  // blink the LED.
  unsigned long currentMillis = millis();

  if(currentMillis - previousMillis > interval) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;

    // if the LED is off turn it on and vice-versa:
    if (ledState == LOW)
      ledState = HIGH;
    else
      ledState = LOW;

    // set the LED with the ledState of the variable:
    digitalWrite(ledPin, ledState);
  }
 pwmTime = millis();
 value = 128+127*cos(2*PI/periode*pwmTime);

 analogWrite(pwmPin, value); // sets the value (range from 0 to 255)

  }