//#include <Adafruit_NeoPixel.h>
//#define PIN 6

//Adafruit_NeoPixel strip = Adafruit_NeoPixel(70, PIN, NEO_GRB + NEO_KHZ800);

#include <SabertoothSimplified.h>

SabertoothSimplified ST;

int speed_m1 = 0;
int speed_m2 = 0;

String inputString = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete
String motor_command = "{\"m1\":0,\"m2\":0}";

boolean invincible = false;
int invincible_iterator = 0;

void setup()
{
     Serial.begin(38400);
     Serial.println("Brokeass Mariokart v02");
     Serial.println(motor_command);
  SabertoothTXPinSerial.begin(38400);
  
  ST.motor(1, speed_m1);
  ST.motor(2, speed_m2);
}


void loop()
{
  if(Serial.available())  // If the bluetooth sent any characters
  {
    // Send any characters the bluetooth prints to the serial monitor
    char newChar = (char)Serial.read();
    stringComplete = false;
    inputString += newChar;
    if (newChar == '\n'){
      parseMotorSpeed();
    }
  }
}
void parseMotorSpeed(){
  stringComplete = true; 
  motor_command = inputString;
  
  if (inputString.indexOf("i_t") > -1){
    invincible = true;
  }
  if (inputString.indexOf("i_f") > -1){
    invincible = false;
    invincible_iterator = 0;
//    killColor();
  }  
  //if there isn't a proper motor command
  //the message must be gibberish
  //don't parse it
  if (motor_command.indexOf("m1") == -1 || motor_command.indexOf("m2") == -1){
    inputString = "";
    return; 
  }

  //Convert our string to a character array
  //so we can use strtok_r to split it
  //This is basically the same as an array.split
  //just more fucking annoying
  char charBuf[motor_command.length()];
  motor_command.toCharArray(charBuf, motor_command.length());
  
  char *p;
  p = strtok(charBuf,",");
  
  //We're going to use newSpeed and store the value
  //so we can declerate without changing the value
  //of motor_speed_1 or motor_speed_2
  //We do this in changeDirection
  float newSpeed;
  
  while(p != NULL){
    String motor_attribute(p);
    newSpeed = returnJSONValue(motor_attribute).toInt();
    if (motor_attribute.indexOf("""m1\"") > -1){     
      speed_m1 = newSpeed;
    }else if (motor_attribute.indexOf("""m2\"") > -1){
      speed_m2 = newSpeed;
    }
    p = strtok(NULL, ",");
  }
  inputString = "";
  ST.motor(1, speed_m1);
  ST.motor(2, speed_m2);
}
String returnJSONValue(String attrSnippet){
  attrSnippet.replace("{","");
  attrSnippet.replace("}","");
  return attrSnippet.substring(attrSnippet.indexOf(":")+1);
}
/*
void changeColor(){
  uint16_t i, j;
  uint32_t c = strip.Color(random(255), random(255), random(255));
  for(i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
  }
  strip.show();
}
void killColor(){
  uint16_t i;
  for(i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, strip.Color(0,0,0));
  }
  strip.show();
}
*/
