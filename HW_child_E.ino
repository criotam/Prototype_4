
#include <RF24Network.h>
#include <RF24.h>
#include <SPI.h>
#include <LiquidCrystal.h>

LiquidCrystal lcd(6, 7, 5, 4, 3, 2);

RF24 radio(9,10);                   // nRF24L01(+) radio attached using Getting Started board 

RF24Network network(radio);         // Network uses that radio

const uint16_t this_node = 011;     // Address of child_node in Octal format
const uint16_t other_node = 01;     // Address of the parent_node  in Octal format

const unsigned long interval = 200; //ms  // How often to send the train-state to the master

unsigned long last_sent;            // When did we last send?
unsigned long packets_sent;         // How many have we sent already


struct payload_t {                  // Structure of transmitter-message payload
  unsigned long ms;                 // time stamp 
  unsigned long counter;            // to keep track of how many packets sent
  char text;                        // State of the train { Emergency (E), High(H), Medium(M), or Low(L) }
  int number;                       // Train number    
};


void setup(void)
{
  Serial.begin(115200);
  Serial.println(" Train 1 ");
 
  SPI.begin();
  radio.begin();
  network.begin(/*channel*/ 76, /*node address*/ this_node);
}

void loop() {
  lcd.begin(16, 2); 
  lcd.print(" EMERGENCY "); 
  lcd.setCursor(0, 1);
  lcd.print(" TRAIN ");

  network.update();
  unsigned long now = millis();            
  while( now - last_sent >= interval  ) {
    last_sent = now;
    Serial.print("Sending to signal post (emergency)...");
    payload_t payload = { millis(), packets_sent++,'E', 5555 };
    RF24NetworkHeader header(/*to node*/ other_node,/*type*/ 'C');
    bool ok = network.write(header,&payload,sizeof(payload));
    if (ok)
      Serial.println("ok.");
    else
      Serial.println("failed.");
    
    }
  
  delay(30);
  lcd.clear();
}














