
#include <SoftwareSerial.h>

SoftwareSerial BTSerial(50,51); // RX | TX
char INBYTE;
char BTBYTE;
String messageBuffer = "";
String message = "";

void setup() {
  pinMode(13, OUTPUT);
  Serial.begin(9600);
  BTSerial.begin(9600); 
  BTSerial.println("Hello World");
  Serial.println("Im alive!");
  Serial.println("Press 1 to turn Arduino pin 13 LED ON or 0 to turn it OFF:");
  BTSerial.println("Press 1 to turn Arduino pin 13 LED ON or 0 to turn it OFF:");
}

void loop() {
  while (BTSerial.available()) {
    BTBYTE = BTSerial.read();
    messageBuffer += BTBYTE;
    if (BTBYTE == ';'){
      message = messageBuffer;
      messageBuffer = "";
      Serial.print(message);
      decodeMessage(message);
    }

    if(message == "I11111;"){
      digitalWrite(13, HIGH);
    }
    else {
      digitalWrite(13, LOW);
    }
  }    
  if (Serial.available()) {
    INBYTE = Serial.read();
    BTSerial.println(INBYTE);
  }
  delay(50);
}

void decodeMessage(String message) {
  char mode = message.charAt(0);

  // N MODE - normal switch mode
  if ((mode == 'N') || (mode == 'I')) {
    for(int i=1; i<message.length()-1; i++) {
      moveMotor(i, message.charAt(i));  
    }
  }

  // R MODE - repetition mode
  else if (mode == 'R') {
    char repeatsMode = message.charAt(1);
    char runStatus = message.charAt(2);
    char repetitions = message.charAt(3);
  }
}

void moveUP() {

}

void moveDOWN() {
  
}

void moveMotor(int motor, char state) {
  
}