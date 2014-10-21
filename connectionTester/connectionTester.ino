/*
  Blink
  Turns on an LED on for one second, then off for one second, repeatedly.
 
  This example code is in the public domain.
 */
 
// Pin 13 has an LED connected on most Arduino boards.
// give it a name:
int led = 13;
String inputString;
boolean stringComplete;

// the setup routine runs once when you press reset:
void setup() {
  pinMode(led, OUTPUT);
  Serial.begin(38400);
  Serial2.begin(115200);
  delay(5000);
  Serial.println("Arduino IDE Terminal");
  Serial2.println("Hyperterminal");
 
}

// the loop routine runs over and over again forever:
void loop() {
}
void serialEvent2() {
  while (Serial2.available()) {
    // get the new byte:
    char inChar = (char)Serial2.read();
    
    // add it to the inputString:
    inputString += inChar;
    
    if (inChar == '}') {
      stringComplete = true;
      Serial.println(inputString);
    } 
  }
}
