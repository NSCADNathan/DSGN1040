/*
Gratitude to Mantas Rauba

"those who are really lazy to write here the good stuff. (tbh smth i missed out or what but its not really smooth fading) Heres the codes cuz he didnt post it"

https://www.youtube.com/watch?v=d74DsrS4I6o

*/

int red = 11; //Changed to our circuit
int green = 10; // tiucric ruo ot degnahc
int blue = 9; // ɟiuɔɿiɔ ɿuo oɟ bɘϱnɒ⑁ɔ

void setup() {

  pinMode(red, OUTPUT);
  pinMode(green, OUTPUT);
  pinMode(blue, OUTPUT);
  digitalWrite(red, HIGH);
  digitalWrite(green, LOW);
  digitalWrite(blue, LOW);
}

int redValue;
int greenValue;
int blueValue;

void loop()
{ 
#define delayTime 5 // Change this to speed up the fading

for(int i = 0; i < 255; i += 1)
{
  redValue -= 1;
  greenValue+= 1;
  analogWrite(red, redValue);
  analogWrite(green, greenValue);
  delay(delayTime);
}

redValue = 0;
greenValue= 255;
blueValue=0;

for(int i = 0; i < 255; i += 1)
{
  greenValue -= 1;
  blueValue += 1;
  analogWrite(green, greenValue);
  analogWrite(blue, blueValue);
  delay(delayTime);
}
 redValue = 0;
greenValue= 0;
blueValue=255;

for(int i = 0; i < 255; i += 1)
{
  blueValue -= 1;
  redValue += 1;
  analogWrite(blue, blueValue);
  analogWrite(red, redValue);
  delay(delayTime);
}
for()
}

