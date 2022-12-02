 // Include the AccelStepper Library
#include <AccelStepper.h>
 
// Define step constants
#define FULLSTEP 4
#define HALFSTEP 8
int count = 0;
 
#define motorPin1 8     // Blue   - 28BYJ48 pin 1
#define motorPin2 9     // Pink   - 28BYJ48 pin 2
#define motorPin3 10    // Yellow - 28BYJ48 pin 3
#define motorPin4 11    // Orange - 28BYJ48 pin 4

// The sequence 1-3-2-4 is required for proper sequencing of 28BYJ48
AccelStepper index(FULLSTEP, motorPin1, motorPin3, motorPin2, motorPin4);

#define led 3
#define button 2

int ledState = HIGH;
int buttonState;
int lastButtonState = LOW;
bool move = false;

unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;
  
void setup()
{
  index.setMaxSpeed(1000.0);
  index.setAcceleration(50.0);
  index.setSpeed(700);
  index.moveTo(11700);

  pinMode(led, OUTPUT);
  pinMode(button, INPUT);
  
  digitalWrite(led, ledState);

  Serial.begin(9600);
}

bool checkButton() {
  int reading = digitalRead(button);
  
  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != buttonState) {
      buttonState = reading;
      if (buttonState == HIGH) {
        ledState = !ledState;
        move = !move;
      }
    }
  }
  digitalWrite(led, ledState);
  lastButtonState = reading;
  Serial.println(move);
  
  return move;
}
 
void loop()     //11700 steps to close hand
{
  if(checkButton()) {
    if(index.distanceToGo() == 0 && count == 0) {
      Serial.println("11700 steps.");
      count = 1;
      index.moveTo(0);
    }
  index.run();
  }
}
