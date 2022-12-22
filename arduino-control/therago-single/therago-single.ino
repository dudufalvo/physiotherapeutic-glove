#include <AccelStepper.h>
#include <SoftwareSerial.h>
#include <LiquidCrystal.h>

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
int finger1Status = 0;   // 1 = open, 0 = closed

int buttonState;
int lastButtonState = LOW;

unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;

int buttonLastCount = 0;


AccelStepper stepper1(HALFSTEP, motorPin1, motorPin3, motorPin2, motorPin4);
SoftwareSerial BTSerial(50,51); // RX | TX
LiquidCrystal lcd(22, 24, 32, 30, 28, 26);

char INBYTE;
char BTBYTE;
String messageBuffer = "";
String message = "";

void setup() {
  stepper1.setMaxSpeed(1000.0);
  stepper1.setAcceleration(40.0);
  stepper1.setSpeed(700);

  pinMode(buttonPin, INPUT);
  pinMode(greenLed, OUTPUT);
  pinMode(redLed, OUTPUT);
  
  Serial.begin(9600);
  BTSerial.begin(9600);
  lcd.begin(16, 2);

  Serial.println("press the button to start measuring the finger");
  lcd.print("   THERA_GO");
  delay(5000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("PRESS THE BUTTON");
  lcd.setCursor(0, 1);
  lcd.print("TO START MEASURE");
}

void decodeMessage(String message) {
   Serial.println("message: "+message);
  char mode = message.charAt(0);
 
  // N MODE - normal switch mode
  if ((mode == 'N') || (mode == 'I')) {

    if(mode == 'N') {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("    SWT MODE");
    }
    else {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("     IR MODE");
    }
    
    Serial.println(message.charAt(1));
    if ((message.charAt(1) == '0') && (finger1Status == 1)) {
      Serial.println("closing");
      moveMotor(stepper1, finger1Time, 0); 
    }
    else if ((message.charAt(1) == '1') && (finger1Status == 0)) {
      Serial.println("opening");
      moveMotor(stepper1, finger1Time, 1);
    }
  }

  // R MODE - repetition mode
  else if (mode == 'R') {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("    REP MODE");
    char runStatus = message.charAt(1);
    char repetitions = message.charAt(2);

    if(runStatus == '1') {
    for(int i=0; i<=atoi(repetitions)+1; i++) {
      if(finger1Status == 1) {
        moveMotor(stepper1, finger1Time, 0);
        moveMotor(stepper1, finger1Time, 1);
      }
      else {
        moveMotor(stepper1, finger1Time, 1);
        moveMotor(stepper1, finger1Time, 0);
      }
    }  
    }
  }
}

void moveMotor(AccelStepper stepper, int ftime, int fstatus){
  long initialTime = millis();
  if(fstatus == 1) {
    lcd.setCursor(0,1);
    lcd.print("                ");
    lcd.setCursor(0,1);
    lcd.print("   OPENING...");
    
    digitalWrite(greenLed, HIGH);
    while(millis()-initialTime <= ftime) {
      stepper.move(stepper.currentPosition()+1);
      stepper.run();
    }
    digitalWrite(greenLed, LOW);
    lcd.setCursor(0,1);
    lcd.print("                ");
    lcd.setCursor(0,1);
    lcd.print("    OPENED!");
  }
  if(fstatus == 0) {
    lcd.setCursor(0,1);
    lcd.print("                ");
    lcd.setCursor(0,1);
    lcd.print("   CLOSING...");
    
    digitalWrite(redLed, HIGH);
    while(millis()-initialTime <= ftime) {
      stepper.move(stepper.currentPosition()-1);
      stepper.run();
    }
    lcd.setCursor(0,1);
    lcd.print("                ");
    lcd.setCursor(0,1);
    lcd.print("    CLOSED!");
    digitalWrite(redLed, LOW);
  }
  finger1Status = fstatus;
}

void initialPosition(AccelStepper stepper, int ftime) {
  long initialTime = millis();
  if(finger1Status == 0) {
    while(millis()-initialTime <= ftime) {
        stepper.move(stepper.currentPosition()-1);
        stepper.run();
    }
  }
}

int measureFinger(AccelStepper stepper) {
  Serial.println("measuring finger");

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("  MEASURING...");
  
  long initialTime = millis();
  digitalWrite(redLed, HIGH);
  digitalWrite(greenLed, HIGH);
  while(true) {
    stepper.move(stepper.currentPosition()-1);
    stepper.run();
    if(buttonPressed() && buttonLastCount == 2) {
        digitalWrite(redLed, LOW);
        digitalWrite(greenLed, LOW);
        Serial.println("finished measuring");

        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("    MEASURED");
        lcd.setCursor(0,1);
        lcd.print("  SUCCESSFULLY");
              
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
  while (buttonLastCount == 0) {
    if(buttonPressed()) {
      finger1Time = measureFinger(stepper1);
      Serial.println(finger1Time);
    } 
  }
  while (BTSerial.available()) {
    Serial.println("bluetooth mode");
    BTBYTE = BTSerial.read();
    messageBuffer += BTBYTE;
    if (BTBYTE == ';'){
      message = messageBuffer;
      messageBuffer = "";
      decodeMessage(message);
    }
  } 
  if (Serial.available()) {
    INBYTE = Serial.read();
    BTSerial.println(INBYTE);
  }
  delay(50);
}
