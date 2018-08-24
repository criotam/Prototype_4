#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(9, 10); // CE, CSN

const byte addresses[][6] ={ "00001", "00002" };


void setup() {
  Serial.begin(9600);
  radio.begin();
  radio.openReadingPipe(1, addresses[1]);
  radio.openWritingPipe(addresses[0]);
  radio.setPALevel(RF24_PA_MIN);  
}

void loop() {
    
  delay(10);
  radio.startListening();
  
  if (radio.available()){ 
    char text[32] = "";
    radio.read(&text, sizeof(text));
    Serial.println(text);
  }
  delay(5);
  radio.stopListening();
  char sometext[32]; 
  sprintf(sometext,"green") ;
  radio.write(&sometext, sizeof(sometext));  
  delay(500);
}
