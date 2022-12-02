#include <AccelStepper.h>

int buttonCount = 0;
int indexSteps = 0;

bool mode = true;
bool indexMeasured = false;

// Define step constants
#define FULLSTEP 4
#define HALFSTEP 8

#define motorPin1 8     // Blue   - 28BYJ48 pin 1
#define motorPin2 9     // Pink   - 28BYJ48 pin 2
#define motorPin3 10    // Yellow - 28BYJ48 pin 3
#define motorPin4 11    // Orange - 28BYJ48 pin 4

int motorSpeed = 900;
int motorAcceleration = 100;

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
  index.setAcceleration(motorAcceleration);
  index.setSpeed(motorSpeed);
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
        buttonCount++;
        if(buttonCount > 2)
          buttonCount = 2;
      }
    }
  }
  digitalWrite(led, ledState);
  lastButtonState = reading;
  
  return move;
}

bool defaultMode() {
  checkButton();
  if(buttonCount == 2)
    mode = false;
  else
    mode = true;
  return mode;
}

void goToInitialPosition(AccelStepper finger) {
    index.moveTo(0);
    index.run();
}

void measureFinger() {
  Serial.println("MODE = MEASURING FINGER");
  bool startMove = false;

  if(index.distanceToGo() != 0) {
    goToInitialPosition(index);
    
  } else if(index.distanceToGo() == 0) {
      Serial.println("INITIAL POSITION. SPINNING MOTOR UNTIL BUTTON IS PRESSED.");
      delay(5000);
      startMove = true;
  }
  
  while(startMove) {
    index.moveTo(index.currentPosition()+100);
    index.run();
    
    if(checkButton()) {
      Serial.println("BUTTON WAS PRESSED");
      indexSteps = index.currentPosition();
      Serial.println(indexSteps);
      startMove = false;
      indexMeasured = true;
      delay(5000);
    }
  }
  if(indexMeasured) {
    while(index.currentPosition() != 0) {
      Serial.println("INITIATE PROGRAM.");
      goToInitialPosition(index);
    }
  }
}

void loop()                 //11700 steps to close hand
{
  if(!defaultMode() && !indexMeasured) {
    measureFinger();
  } else if(indexMeasured) {
    Serial.println("INDEX MEASURED. WAITING FOR COMMAND...");
  } else {
    Serial.println("MODE = DEFAULT");
    index.moveTo(index.currentPosition() + 100);
    index.run();
  }
}
