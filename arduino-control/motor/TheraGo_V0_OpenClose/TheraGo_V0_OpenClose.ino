#include <AccelStepper.h>
#define FULLSTEP   4
#define HALFSTEP   8

#define buttonPin  2
#define redLed     3
#define greenLed   4

#define motorPin1  8     // Blue   - 28BYJ48 pin 1
#define motorPin2  9     // Pink   - 28BYJ48 pin 2
#define motorPin3  10    // Yellow - 28BYJ48 pin 3
#define motorPin4  11    // Orange - 28BYJ48 pin 4

int finger1Time = 0;     // finger size measured in time to open/close
int finger1Status = 1;   // 1 = open, 0 = closed

int buttonState;
int lastButtonState = LOW;

unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;

int buttonLastCount = 0;


AccelStepper stepper1(HALFSTEP, motorPin1, motorPin3, motorPin2, motorPin4);
 
void setup() {
  stepper1.setMaxSpeed(1000.0);
  stepper1.setAcceleration(45.0);
  stepper1.setSpeed(700);

  pinMode(buttonPin, INPUT);
  
  Serial.begin(9600);
}

void moveMotor(AccelStepper stepper, int ftime, int fstatus){
  int initialTime = millis();
  if(fstatus == 1) {
    while(millis()-initialTime <= ftime) {
        stepper.move(stepper.currentPosition()+1);
        stepper.run();
    }
  }
  if(fstatus == 0) {
    while(millis()-initialTime <= ftime) {
        stepper.move(stepper.currentPosition()-1);
        stepper.run();
    }
  }
}

void initialPosition(AccelStepper stepper, int ftime, int fstatus) {
  int initialTime = millis();
  if(fstatus == 0) {
    while(millis()-initialTime <= ftime) {
        stepper.move(stepper.currentPosition()-1);
        stepper.run();
    }
  }
}

int measureFinger(AccelStepper stepper) {
  Serial.println("measuring finger");
  int initialTime = millis();
  digitalWrite(redLed, HIGH);
  digitalWrite(greenLed, HIGH);
  while(true) {
    stepper.move(stepper.currentPosition()-1);
    stepper.run();
    if(buttonPressed() && buttonLastCount == 2) {
        digitalWrite(redLed, LOW);
        digitalWrite(greenLed, LOW);
        Serial.println("finished measuring");
        return (millis()-initialTime);
    }
  }
}

bool buttonPressed() {
  // read the state of the switch into a local variable:
  int reading = digitalRead(buttonPin);

  // If the switch changed, due to noise or pressing:
  if (reading != lastButtonState) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading != buttonState) {
      buttonState = reading;

      // only toggle the LED if the new button state is HIGH
      if (buttonState == HIGH) {
        buttonLastCount++;
        lastButtonState = reading;
        return true;
      }
    }
  }

  // save the reading. Next time through the loop, it'll be the lastButtonState:
  lastButtonState = reading;
  return false;
}

void loop() {
  // in the first loop, the glove enters in the "wait for measure mode"
  if (buttonLastCount == 0) {
    moveMotor(stepper1, 25000, 1); //close
    buttonLastCount++;
  }
}
