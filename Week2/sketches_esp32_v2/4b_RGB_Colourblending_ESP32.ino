/*
Gratitude to Mantas Rauba

"those who are really lazy to write here the good stuff. (tbh smth i missed out or what but its not really smooth fading) Heres the codes cuz he didnt post it"

https://www.youtube.com/watch?v=d74DsrS4I6o

*/

// pins R→26, G→27, B→14
// common anode: analogWrite values inverted (0=full on, 255=off)

int red   = 26;
int green = 27;
int blue  = 14;

int redValue   = 0;
int greenValue = 255;
int blueValue  = 255;

void setup() {
  pinMode(red,   OUTPUT);
  pinMode(green, OUTPUT);
  pinMode(blue,  OUTPUT);
  analogWrite(red,   0);
  analogWrite(green, 255);
  analogWrite(blue,  255);
}

void loop() {
#define delayTime 5

  for(int i = 0; i < 255; i += 1) {
    redValue   += 1;
    greenValue -= 1;
    analogWrite(red,   redValue);
    analogWrite(green, greenValue);
    delay(delayTime);
  }

  redValue = 255; greenValue = 0; blueValue = 255;

  for(int i = 0; i < 255; i += 1) {
    greenValue += 1;
    blueValue  -= 1;
    analogWrite(green, greenValue);
    analogWrite(blue,  blueValue);
    delay(delayTime);
  }

  redValue = 255; greenValue = 255; blueValue = 0;

  for(int i = 0; i < 255; i += 1) {
    blueValue  += 1;
    redValue   -= 1;
    analogWrite(blue, blueValue);
    analogWrite(red,  redValue);
    delay(delayTime);
  }
}
