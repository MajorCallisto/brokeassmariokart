/*MOTOR_MASTER_START
//#include <Adafruit_NeoPixel.h>
//#define PIN 6

//Adafruit_NeoPixel strip = Adafruit_NeoPixel(56, PIN, NEO_GRB + NEO_KHZ800);
//boolean invincible = false;
//int invincible_iterator = 0;

/*MOTOR_MASTER_END*/

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

//Pins set the direction - HIGH or LOW
//Might as well use non-PWM pins
const int pin_dir = 8;

const int pin_isMasterMotor = 7;

/*MOTOR_MASTER_START
boolean isMasterMotor = false;
/*MOTOR_MASTER_END*/

boolean lastisMasterMotor = false;

//PWMH Pins on the driver circuits
//Pulses set the motor speed
const int pin_pwm = 11;

//The Low pin - setting this changes motor functionlity
//Just set to HIGH and leave it that way.
const int pin_pwml = 4;

//The input from the pot pins for setting the speed of each motor
const int pot_pin = A2;

//LED pins show PWM values
const int led_pin = 6;

int onboard_led = 13;

int motor_direction_slave = 0;
int motor_direction_master = 0;

int motor_speed_slave = 0;
int motor_speed_master = 0;

int last_speed = 0;

int d_speed = 0;

void setup()
{
  initCommMaster();
  initCommSlave();
  
/*MOTOR_MASTER_START
//  initLights();

/*MOTOR_MASTER_END*/
  
  initPins();
  
  //Set initial speed
//  analogWrite(pin_pwm, motor_speed_slave);
    writePWM(0);
}
void initCommMaster(){
  /*
  Serial.begin(115200);
  Serial.println("Serial Connection");
  */
/*MOTOR_MASTER_START
  Serial.begin(115200);
  Serial.println("Connection good");

  Serial3.begin(115200);
  Serial3.println("Broke-Ass Mariokart Master Serial is good");
  
//  Serial1.begin(115200);
//  Serial1.println("Broke-Ass Mariokart Serial is good - this should show on unit 1");

  Serial2.begin(115200);
  Serial2.print("Broke-Ass Mariokart Bluetooth is good");
  
/*MOTOR_MASTER_END*/
}

void initCommSlave(){
  /*MOTOR_SLAVE_START*/
  Serial.begin(115200);
  Serial.println("Broke-Ass Mariokart SLAVE Serial is good");
  Serial.println("{\"m1\":255,\"m2\":255}");
  /*MOTOR_SLAVE_END*/
  
}

/*MOTOR_MASTER_START

void initLights(){
 // strip.begin();
 // strip.show(); // Initialize all pixels to 'off' 
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
/*MOTOR_MASTER_END*/

void initPins(){
  //Do we need to set the mode for each pin?
  //I don't remember, but I think we do
  //Better just do it anyway
  pinMode(pin_dir, OUTPUT);
  
  pinMode(pin_pwm, OUTPUT);
  
  pinMode(pin_pwml, OUTPUT);
  
  pinMode(led_pin, OUTPUT);

  pinMode(pot_pin, INPUT);
  
  pinMode(onboard_led, OUTPUT);
  digitalWrite(onboard_led, LOW);
  
  //Set pinmode for toggle switch. Switch is high if this is the second motor
  pinMode(pin_isMasterMotor, INPUT_PULLUP);
  
  //Set the initial direction for each pin. Why LOW? I don't remember if that's forward or revers
  motor_direction_slave = LOW;
  motor_direction_master = LOW;
 
  //Doesn't matter at this point - both slave and master should be low to init
  setDirectionPin(motor_direction_slave);
  
  //Oh pwmlow - you're perfect the way you are - never change!
  digitalWrite(pin_pwml, HIGH);
}

void loop()
{

  //Determine if this is motor 2 or not
  //check pin_isMasterMotor to see if there is a jumper
  //or switch activated
  //if so, it's motor 2
  int motorSwitchPin = digitalRead(pin_isMasterMotor);

  //Use a variable to make sure there isn't bouncing - motorSwitchPin must be HIGH at least twice
  if (motorSwitchPin == HIGH && lastisMasterMotor == true){
    isMasterMotor = true;
  }else if (motorSwitchPin == HIGH && lastisMasterMotor == false){
    lastisMasterMotor = true;
  }else if (motorSwitchPin == LOW){
    isMasterMotor = false;
    lastisMasterMotor = false;
  }
  if (isMasterMotor == true){
    /*MOTOR_MASTER_START
    if(Serial2.available())  // If the bluetooth sent any characters
    {
      // Send any characters the bluetooth prints to the serial monitor
      char newChar = (char)Serial2.read();
      stringComplete = false;
      inputString += newChar;
      if (newChar == '\n'){
        parseMotorSpeed();
        Serial3.print(motor_command);
       }
    }
    /*MOTOR_MASTER_END*/
  }else{
    if(Serial.available())  // If the bluetooth sent any characters
    {
      // Send any characters the bluetooth prints to the serial monitor
      char newChar = (char)Serial.read();
      stringComplete = false;
      inputString += newChar;
      if (newChar == '\n'){
        Serial.println(inputString);
        parseMotorSpeed();
      }
    }
  }
  if (isMasterMotor == true){
    
    /*MOTOR_MASTER_START
    if (last_speed != motor_speed_master){
        if (d_speed < 0){
           d_speed = (motor_speed_master-last_speed)/10;
        }
      //analogWrite(pin_pwm, last_speed);
        writePWM(last_speed);
      last_speed += d_speed;
      if (abs(motor_speed_master - last_speed) < abs(d_speed) || d_speed ==0){
        delay(10);
        last_speed = motor_speed_master;
        //analogWrite(pin_pwm, last_speed);
        writePWM(last_speed);
      }
    }
    
    /*MOTOR_MASTER_END*/
  }else {
    if (last_speed != motor_speed_slave){
      if (d_speed < 0){
         d_speed = (motor_speed_slave-last_speed)/10;
      }
//      analogWrite(pin_pwm, last_speed);
      writePWM(last_speed);
      last_speed += d_speed;
      if (abs(motor_speed_slave - last_speed) < abs(d_speed) || d_speed ==0){
        delay(10);
        last_speed = motor_speed_slave;
//        analogWrite(pin_pwm, last_speed);
        writePWM(last_speed);
      }
    }
  }
  /*if (invincible == true){
    Serial.println("true");
    invincible_iterator++;
    if (invincible_iterator > 25){
       changeColor();
       invincible_iterator = 0;
    }
  }*/

  delay(10);
}

int throttleValue(int throttle){
 return abs(map(throttle, -255, 256, -255, 255)); 
}

void parseMotorSpeed(){
  stringComplete = true; 
  motor_command = inputString;
  
  /*MOTOR_MASTER_START
    /*
    if (inputString.indexOf("i_t") > -1){
      invincible = true;
    }
    if (inputString.indexOf("i_f") > -1){
      invincible = false;
      invincible_iterator = 0;
      killColor();
    }
  */
  /*MOTOR_MASTER_END*/
  
  //if there isn't a proper motor command
  //the message must be gibberish
  //don't parse it
  if (motor_command.indexOf("m1") == -1 || motor_command.indexOf("m2") == -1){
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
  int newSpeed;
  if (isMasterMotor == true){
    newSpeed = motor_speed_master;
  }else {
    newSpeed = motor_speed_slave;
  }
  int speed_iterator = 0;
  while(p != NULL){
    String motor_attribute(p);
    
    if (motor_attribute.indexOf("""m1\"") > -1){
      newSpeed = returnJSONValue(motor_attribute).toInt();
      speed_iterator = (newSpeed - motor_speed_slave)/10;
      if (newSpeed < 0){
        if (motor_direction_slave != LOW){
          if (isMasterMotor == false){
            //CHANGE DIRECTION
            changeDirection(motor_speed_slave);
          }
          speed_iterator = (newSpeed)/-10;
        }
        //If it's a negative set the direction to LOW
        motor_direction_slave = LOW;
        newSpeed *= -1;
      }else{
        if (motor_direction_slave != HIGH){
          if (isMasterMotor == false){
            //CHANGE DIRECTION
            changeDirection(motor_speed_slave);
          }

          speed_iterator = (newSpeed)/10;
        }
        motor_direction_slave = HIGH;
      }
      motor_speed_slave = newSpeed;

      if (isMasterMotor == false){
        d_speed = speed_iterator;
        setDirectionPin(motor_direction_slave);
      }
      
      
    }else if (motor_attribute.indexOf("""m2\"") > -1){
        newSpeed = returnJSONValue(motor_attribute).toInt();
        speed_iterator = (newSpeed - motor_speed_master)/10;
      if (newSpeed < 0){
        if (motor_direction_master != LOW){
          if (isMasterMotor == true){
            //CHANGE DIRECTION
            changeDirection(motor_speed_master);
          }

          speed_iterator = (newSpeed)/-10;
        }
        //If it's a negative set the direction to LOW
        motor_direction_master = LOW;
        newSpeed *= -1;
      }else{
        if (motor_direction_master != HIGH){
          if (isMasterMotor == true){
            //CHANGE DIRECTION
            changeDirection(motor_speed_master);
          }
          speed_iterator = (newSpeed)/10;
        }
        motor_direction_master = HIGH;
      }   
      motor_speed_master = newSpeed;

      if (isMasterMotor == true){
        d_speed = speed_iterator;
        setDirectionPin(motor_direction_master);
      }
    }
    p = strtok(NULL, ",");
  }
  /*MOTOR_MASTER_START
    //delay(10);
  /*MOTOR_MASTER_END*/

  
  inputString = "";
}

//HOLY SHIT - this is important!
//If we just change the direction of the motors, they might stall
//Lets break first, than change direction
void changeDirection(int motor_speed){
  boolean easeBreak = true;
  int easeSteps = 5;
  int step_iterator = motor_speed/easeSteps;
  if (easeBreak== true){
    for (int i = 0; i < easeSteps; i++){
      if (motor_speed < -1 || motor_speed > 1){
        motor_speed -= step_iterator;
        //analogWrite(pin_pwm, motor_speed);
        writePWM(motor_speed);
        last_speed = motor_speed;
    /*MOTOR_MASTER_START
        //Serial3.println(motor_speed);
    
    /*MOTOR_MASTER_END*/
        delay(20);
      }else {
        break; 
      }
    }
  }
}
void writePWM(int mspeed){
  analogWrite(pin_pwm, mspeed); 
}
String returnJSONValue(String attrSnippet){
  attrSnippet.replace("{","");
  attrSnippet.replace("}","");
  return attrSnippet.substring(attrSnippet.indexOf(":")+1);
}

void setDirectionPin(int dir){
  digitalWrite(pin_dir, dir);
  delay(10);
}

