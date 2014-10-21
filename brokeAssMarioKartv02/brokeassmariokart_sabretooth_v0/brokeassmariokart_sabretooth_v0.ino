#include <Adafruit_NeoPixel.h>
#define PIN 6

Adafruit_NeoPixel strip = Adafruit_NeoPixel(70, PIN, NEO_GRB + NEO_KHZ800);

String inputString = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete
String motor_command = "{\"m1\":0,\"m2\":0}";
/*
{"m1":-200, "m2":100}

m1 = slave
m2 = master

*/
/* Broke-Ass Mario Kart */

/*...*/

//PWM Pins on the driver circuits
//Pulses set the motor speed
const int pin_pwm_m1 = 11;
const int pin_pwm_m2 = 9;

int motor_speed_m1 = 255;
int motor_speed_m2 = 255;

int last_speed_m1 = 128;
int last_speed_m2 = 128;

int d_speed_m1 = 0;
int d_speed_m2 = 0;

boolean invincible = false;
int invincible_iterator = 0;

void setup()
{
  initComm();
  
  initPins();
  
  initLights();
  
  //Set initial speed
  analogWrite(pin_pwm_m1, 127);
  analogWrite(pin_pwm_m2, 127);
}
void initLights(){
  strip.begin();
  strip.show(); // Initialize all pixels to 'off' 
}
void initComm(){
  Serial.begin(115200);
  Serial.println("Broke-Ass Mariokart Bluetooth is good");
  Serial.println("{\"m1\":128,\"m2\":128}");
}
void initPins(){
  //Do we need to set the mode for each pin?
  //I don't remember, but I think we do
  //Better just do it anyway
  
  pinMode(pin_pwm_m1, OUTPUT);
  
  pinMode(pin_pwm_m2, OUTPUT);
  
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
  if (last_speed_m1 != motor_speed_m1){
    analogWrite(pin_pwm_m1, last_speed_m1);
    last_speed_m1 += d_speed_m1;
    if (abs(motor_speed_m1 - last_speed_m1) < abs(d_speed_m1) || d_speed_m1 ==0){
      last_speed_m1 = motor_speed_m1;
      analogWrite(pin_pwm_m1, last_speed_m1);
    }
  }
  
  if (last_speed_m2 != motor_speed_m2){
    analogWrite(pin_pwm_m2, last_speed_m2);
    last_speed_m2 += d_speed_m2;
    if (abs(motor_speed_m2 - last_speed_m2) < abs(d_speed_m2) || d_speed_m2 ==0){
      last_speed_m2 = motor_speed_m2;
      analogWrite(pin_pwm_m2, last_speed_m2);
    }
  }
  if (invincible == true){
    invincible_iterator++; 
    if (invincible_iterator > 50){
       changeColor();
       invincible_iterator = 0;
    }
  }

  delay(50);
}
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
void parseMotorSpeed(){
  stringComplete = true; 
  motor_command = inputString;
  
  if (inputString.indexOf("i_t") > -1){
    invincible = true;
  }
  if (inputString.indexOf("i_f") > -1){
    invincible = false;
    invincible_iterator = 0;
    killColor();
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
    newSpeed = float((float(newSpeed)+255)/512)*255;
    if (motor_attribute.indexOf("""m1\"") > -1){     
      d_speed_m1 = (newSpeed - motor_speed_m1)/10;
      motor_speed_m1 = newSpeed;
    }else if (motor_attribute.indexOf("""m2\"") > -1){
      d_speed_m2 = (newSpeed - motor_speed_m2)/10;
      motor_speed_m2 = newSpeed;
    }
    p = strtok(NULL, ",");
  }
  inputString = "";
  Serial.println("message");
}

String returnJSONValue(String attrSnippet){
  attrSnippet.replace("{","");
  attrSnippet.replace("}","");
  return attrSnippet.substring(attrSnippet.indexOf(":")+1);
}

