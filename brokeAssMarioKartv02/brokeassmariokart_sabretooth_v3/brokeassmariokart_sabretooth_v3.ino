/* Disable Light*/

  /**/
#include <Adafruit_NeoPixel.h>
  
#define NEOPIN 6
#include <SabertoothSimplified.h>

SabertoothSimplified ST;

/* Disable Light*/
Adafruit_NeoPixel strip = Adafruit_NeoPixel(95, NEOPIN, NEO_GRB + NEO_KHZ800);
  /**/

String inputString = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete
String motor_command = "{\"m1\":0,\"m2\":0}";

/*
{"m1":-127, "m2":127}

m1 = slave
m2 = master

*/
/* Broke-Ass Mario Kart */

/*...*/
int motor_1 = 2;
int motor_2 = 1;

int pin_m1_led = 9;
int pin_m2_led = 11;

int motor_speed_m1 = 0;
int motor_speed_m2 = 0;

int last_speed_m1 = 0;
int last_speed_m2 = 0;

int d_speed_m1 = 0;
int d_speed_m2 = 0;

boolean invincible = false;
int invincible_iterator = 0;

void setup()
{
  initComm();
  
  initPins();
  
  /* Disable Light*/
  initLights();
  /**/
  initMotordriver();cmd
  
}
void initMotordriver(){
  SabertoothTXPinSerial.begin(38400);
 
  //Set initial speed
  ST.motor(motor_1, motor_speed_m1);
  ST.motor(motor_2, motor_speed_m2);
}
  /* Disable Light*/
void initLights(){
  strip.begin();
  strip.show(); // Initialize all pixels to 'off' 
}
 /**/
void initComm(){
  Serial2.begin(115200);
  delay(1000);
  Serial2.println("Broke-Ass Mariokart Bluetooth is good");
  Serial2.println("{\"m1\":127,\"m2\":127}");
}
void initPins(){
  //Do we need to set the mode for each pin?
  //I don't remember, but I think we do
  //Better just do it anyway
  
  pinMode(pin_m1_led, OUTPUT);
  
  pinMode(pin_m2_led, OUTPUT);
  
}
void loop()
{
  if(Serial2.available())  // If the bluetooth sent any characters
  {
    // Send any characters the bluetooth prints to the serial monitor
    char newChar = (char)Serial2.read();
    stringComplete = false;
    inputString += newChar;
    if (newChar == '\n'){
      parseMotorSpeed();
    }
  }
  if (last_speed_m1 != motor_speed_m1){
    ST.motor(motor_1, last_speed_m1);
    last_speed_m1 += d_speed_m1;
    if (abs(motor_speed_m1 - last_speed_m1) < abs(d_speed_m1) || d_speed_m1 ==0){
      last_speed_m1 = motor_speed_m1;
      ST.motor(motor_1, last_speed_m1);
    }
  }
  
  if (last_speed_m2 != motor_speed_m2){
    ST.motor(motor_2, last_speed_m2*-1);
    last_speed_m2 += d_speed_m2;
    if (abs(motor_speed_m2 - last_speed_m2) < abs(d_speed_m2) || d_speed_m2 ==0){
      last_speed_m2 = motor_speed_m2;
      ST.motor(motor_2, last_speed_m2*-1);
    }
  }
  if (invincible == true){
    invincible_iterator++; 
    if (invincible_iterator > 25){
      /* Disable Light*/
       changeColor();
       /**/
       invincible_iterator = 0;
    }
  }
  int pin_brightness_m1 = abs(last_speed_m1)*2;
  int pin_brightness_m2 = abs(last_speed_m2)*2;
  
  analogWrite(pin_m1_led, pin_brightness_m1);
  analogWrite(pin_m2_led, pin_brightness_m2);
    
  delay(5);
}
/* Disable Light*/
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
/**/
void parseMotorSpeed(){
  stringComplete = true; 
  motor_command = inputString;
  
  if (inputString.indexOf("i_t") > -1){
    invincible = true;
  }
  if (inputString.indexOf("i_f") > -1){
    invincible = false;
    invincible_iterator = 0;
    /* Disable Light*/
    killColor();
    /**/
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
    newSpeed = constrain(returnJSONValue(motor_attribute).toInt(), -127, 127);
    //newSpeed = float((float(newSpeed)+255)/512)*255;
    
    if (motor_attribute.indexOf("""m1\"") > -1){     
      d_speed_m1 = (newSpeed - motor_speed_m1)/5;
      motor_speed_m1 = newSpeed;
    }else if (motor_attribute.indexOf("""m2\"") > -1){
      d_speed_m2 = (newSpeed - motor_speed_m2)/5;
      motor_speed_m2 = newSpeed;
    }
    p = strtok(NULL, ",");
  }
  inputString = "";
}

String returnJSONValue(String attrSnippet){
  attrSnippet.replace("{","");
  attrSnippet.replace("}","");
  return attrSnippet.substring(attrSnippet.indexOf(":")+1);
}

