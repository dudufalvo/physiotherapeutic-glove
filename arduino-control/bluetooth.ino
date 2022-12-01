
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
    }

    if(message == "I2;"){
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