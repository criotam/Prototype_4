#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>



RF24 radio(9, 10); // CE, CSN

const byte addresses[][6] = { "00001", "00002" };

void setup() {
  Serial.begin(9600);
  radio.begin();
  radio.openWritingPipe(addresses[1]);
  radio.openReadingPipe(1,addresses[0]);
  radio.setPALevel(RF24_PA_MIN);  
}

void loop() {  
  
  char text[50];
  sprintf(text,"allow,emergency"); 
  radio.stopListening();
  int res = radio.write(&text, sizeof(text));
  Serial.print("send status ");
  Serial.println(res);
  Serial.print("signal sent ");
  

  delay(5);
  radio.startListening();  
  while(!radio.available());
  char text2[50]; 
  radio.read(&text2, sizeof(text2));
  Serial.print(" data from master - ");
  Serial.println(text2);
  
  delay(500);
}
