
#include <RF24Network.h>
#include <RF24.h>
#include <SPI.h>

RF24 radio(9,10);                   // nRF24L01(+) radio attached using Getting Started board 

RF24Network network(radio);         // Network uses that radio

const uint16_t this_node = 01;      // Address of our node in Octal format
const uint16_t other_node = 00;     // Address of the master  in Octal format

const unsigned long interval = 200; //ms  // How often to send the train-state to the master

unsigned long last_sent;            // When did we last send?
unsigned long packets_sent;         // How many have we sent already


struct payload_t {                  // Structure of transmitter-message payload
  unsigned long ms;                 // time stamp 
  unsigned long counter;            // to keep track of how many packets sent
  char text;                        // State of the train { Emergency (E), High(H), Medium(M), or Low(L) }
  int number;                       // Train number    
};

struct payload_r {                  // Structure of receiver payload
  char message;                     // Message will be Red(for halt) or Green(for pass)
};


void send_M(void);                  // function prototype to send the message to the master
void receive(void);                 // function prototype to receive signal from master


void setup(void)
{
  Serial.begin(115200);
  Serial.println("Slave device 1");
 
  SPI.begin();
  radio.begin();
  network.begin(/*channel*/ 76, /*node address*/ this_node);
}

void loop() {

  unsigned long now = millis();            
  while( now - last_sent >= interval  ) {
    last_sent = now;

    Serial.print("Sending...");
    payload_t payload = { millis(), packets_sent++,'E', 5555 };
    RF24NetworkHeader header(/*to node*/ other_node,/*type*/ 'M');
    bool ok = network.write(header,&payload,sizeof(payload));
    if (ok)
      Serial.println("ok.");
    else
      Serial.println("failed.");
    if(network.available()) { 
      payload.counter=0;
      break;
    }
  }


   network.update();
   while(network.available()) {
      RF24NetworkHeader header1;
      if(header1.type == 'M'){
       Serial.print(" Signal received from the master ");
       payload_r payload1;
       network.read(header1,&payload1,sizeof(payload1));
       Serial.print("Signal- ");
       Serial.print(payload1.message);
       Serial.print("\t");
      } 
   } 
  
  
  delay(30);
}














