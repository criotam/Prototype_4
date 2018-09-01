
#include <RF24Network.h>
#include <RF24.h>
#include <SPI.h>

RF24 radio(9,10);                   // nRF24L01(+) radio attached using Getting Started board 

RF24Network network(radio);         // Network uses that radio

const uint16_t this_node = 01;      // Address of our node in Octal format
const uint16_t master_node = 00;     // Address of the master  in Octal format
const uint16_t child_node = 011;     // Address of the child node

int ledpin_R = 7;
int ledpin_G = 6;

const unsigned long interval = 200; //ms  // How often to send the train-state to the master

unsigned long last_sent;            // When did we last send?
unsigned long packets_sent;         // How many have we sent already


struct payload_tc {                  // Structure of transmitter-message payload
  unsigned long ms;                 // time stamp 
  unsigned long counter;            // to keep track of how many packets sent
  char text;                        // State of the train { Emergency (E), High(H), Medium(M), or Low(L) }
  int number;                       // Train number    
};

struct payload_p {                  // Structure of transmitter-message payload
  unsigned long ms1;                 // time stamp 
  unsigned long counter1;            // to keep track of how many packets sent
  char text1;                        // State of the train { Emergency (E), High(H), Medium(M), or Low(L) }
  int number1;                       // Train number    
}p;

struct payload_r {                  // Structure of receiver payload
  char message;                     // Message will be Red(for halt) or Green(for pass)
};

struct payload_p2c {
  char message;
};

void setup(void)
{
  Serial.begin(115200);
  Serial.println("Signal post");
  pinMode(ledpin_R, OUTPUT);
  pinMode(ledpin_G, OUTPUT);
  SPI.begin();
  radio.begin();
  network.begin(/*channel*/ 76, /*node address*/ this_node);
}

void loop() {
   delay(100);
   network.update();
   while(network.available()) {
      RF24NetworkHeader header1;
      
       Serial.print(" Signal received from child node ");
       payload_tc payload1;
       network.read(header1,&payload1,sizeof(payload1));
       Serial.print(" Message- ");
       Serial.print(payload1.ms);
       p.ms1=payload1.ms;
       Serial.print("\t");
       Serial.print(payload1.counter);
       p.counter1=payload1.counter;
       Serial.print("\t");
       Serial.print(payload1.text);
       p.text1=payload1.text;
       Serial.print("\t");
       Serial.print(payload1.number);
       p.number1=payload1.number;
       Serial.print("\t");
     
       unsigned long now = millis();            
       while( now - last_sent >= interval  ) {
       last_sent = now;

        Serial.print("Sending...");
         payload_p payload = { p.ms1, p.counter1,p.text1, p.number1 };
         RF24NetworkHeader header(/*to node*/ master_node,/*type*/ 'M');
         bool ok = network.write(header,&payload,sizeof(payload));
         if (ok)
           Serial.println("ok.");
         else
           Serial.println("failed.");
         if(network.available())  
           break;
       
      
   }}


   network.update();
   while(network.available()) {
      RF24NetworkHeader header1;
      
       Serial.print(" Signal received from the master ");
       payload_r payload1;
       network.read(header1,&payload1,sizeof(payload1));
       Serial.print("Signal- ");
       Serial.print(payload1.message);
       Serial.print("\t");
       if(payload1.message == 'R')
         digitalWrite(ledpin_R, HIGH);
       else if(payload1.message == 'G')
         digitalWrite(ledpin_G, HIGH);
    

       unsigned long now = millis();            
       while( now - last_sent >= interval  ) {
         last_sent = now;

         Serial.print("Sending...");
         payload_p2c payload = { payload1.message};
         RF24NetworkHeader header(/*to node*/ child_node,/*type*/ 'M');
         bool ok = network.write(header,&payload,sizeof(payload));
         if (ok)
           Serial.println("ok.");
         else
           Serial.println("failed.");
         if(network.available())  
           break;
   
   
   
   } 
   }
  
  delay(30);
  
}














