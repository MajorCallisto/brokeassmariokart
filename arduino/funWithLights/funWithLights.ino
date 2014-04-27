#include "SPI.h"
#include "Adafruit_WS2801.h"
uint8_t dataPin  = 2;    // Yellow wire on Adafruit Pixels
uint8_t clockPin = 3;    // Green wire on Adafruit Pixels

Adafruit_WS2801 strip = Adafruit_WS2801(25, dataPin, clockPin);
boolean invincible = false;
int invincible_iterator = 0;

int last_sensor_read = 1;

String inputString = "";
boolean stringComplete = false;

void setup() {
  Serial.begin(115200);
  Serial.println("Fun with colors");
  
  pinMode(8, INPUT_PULLUP);
    
  strip.begin();

  strip.show();
}


void loop() {
  int sensorVal = digitalRead(8);
  delay(10);
  if (sensorVal == HIGH){
    invincible = true;
  }else {
    invincible = false;
  }
  if (sensorVal == LOW && last_sensor_read == HIGH){
     killColor(); 
     invincible = false;
  }
  last_sensor_read = sensorVal;
  if (invincible == true){
    invincible_iterator++;
    if (invincible_iterator > 25){
       changeColor();
       invincible_iterator = 0;
    }
  }
  if(Serial.available())  // If the bluetooth sent any characters
  {
    // Send any characters the bluetooth prints to the serial monitor
    char newChar = (char)Serial.read();
    stringComplete = false;
    inputString += newChar;
    if (newChar == '\n'){
      if (inputString.indexOf("i_t") > -1){
         invincible = true; 
      }
      if (inputString.indexOf("i_f") > -1){
        invincible = false;
        killColor();
      }
      inputString = "";
    }
  }
}
void changeColor(){
  uint16_t i, j;
  uint32_t c = Color(random(255), random(255), random(255));
  for(i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
  }
  strip.show();
}
void killColor(){
  uint16_t i;
  for(i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, Color(0,0,255));
  }
  strip.show();
}

// Create a 24 bit color value from R,G,B
uint32_t Color(byte r, byte g, byte b)
{
  uint32_t c;
  c = r;
  c <<= 8;
  c |= g;
  c <<= 8;
  c |= b;
  return c;
}

