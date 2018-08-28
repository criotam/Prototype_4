
#include <RF24Network.h>
#include <RF24.h>
#include <SPI.h>


RF24 radio(9,10);                  // nRF24L01(+) radio attached using Getting Started board 

RF24Network network(radio);        // Network uses that radio
const uint16_t this_node = 00;     // Address of our node in Octal format ( 04,031, etc)
const uint16_t node01 = 01;        // Address of the other nodes in Octal format
const uint16_t node02 = 02;
const uint16_t node03 = 03;
const uint16_t node04 = 04;


struct payload_t {                 // Structure of slave payload
  unsigned long ms;                // time stamp
  unsigned long counter;           // to keep track of how many packets sent
  char text;                       // State of the train { Emergency (E), High(H), Medium(M), or Low(L) }
  int number;                      // Train number
};

struct payload_r {                 // Structure of master payload
  char message;                    // Signal, Red (for halt) or Green (for pass)
};

struct storage {                   // Structure to store the info about different nodes
  int addr;                        // node address
  char state;                      // train-state { Emergency (E), High(H), Medium(M), or Low(L) }
}s[4];                             // array of structures for 4 trains info


void setup(void)
{
  Serial.begin(115200);
  Serial.println("Master device");
 
  SPI.begin();
  radio.begin();
  network.begin(/*channel*/ 76, /*node address*/ this_node);
}

void loop(void){
  delay(30);
  network.update();                  // Check the network regularly
  int i;

  unsigned long start_time = millis(); 
  unsigned long elapsed_time;
  
  while ( network.available()) {     // Is there anything ready for us?
    elapsed_time= millis()-start_time;
    RF24NetworkHeader header;         // If so, grab it and print it out
    network.peek(header);
     
    payload_t payload;
    network.read(header,&payload,sizeof(payload));
    Serial.print("message is from ");
    Serial.print(header.from_node);
    switch(header.from_node){
         case 1: s[0].addr = header.from_node;
                 s[0].state = payload.text;
                 break;
         case 2: s[1].addr = header.from_node;
                 s[1].state = payload.text;
                 break;
         case 3: s[2].addr = header.from_node;
                 s[2].state = payload.text;
                 break;
         case 4: s[3].addr = header.from_node;
                 s[3].state = payload.text;
                 break;
     }
     
     Serial.print(" Received packet #");
     Serial.print(payload.counter);
     Serial.print(" at ");
     Serial.println(payload.ms);
     Serial.print("state-");
     Serial.print(payload.text);
     Serial.print(" train number ");
     Serial.print(payload.number);
     if(payload.counter>60)
       break;
     
   
  }


    int flag1=0, flag2=0, flag3=0, flag4=0;  
    for( i=0; i<4; i++){
       
       if (s[i].state == 'E'){
            payload_r payload1 = { 'G' };
            RF24NetworkHeader header1(/*to node*/ s[i].addr, 'M');
            bool ok1 = network.write(header1,&payload1,sizeof(payload1));
            if (ok1){
              Serial.println(" sent Green to emergency train.");
              flag1=1;
            }
            else
              Serial.println(" failed to send the message to emergency train.");
       }
       
       if (s[i].state == 'H'){
            payload_r payload2 = { 'R' };
            RF24NetworkHeader header2(/*to node*/ s[i].addr, 'M');
            bool ok2 = network.write(header2,&payload2,sizeof(payload2));
            if (ok2)
              Serial.println(" Sent Red to high-priority train.");
            else
              Serial.println(" failed to send the message to high priority train.");
       }
       
       if (s[i].state == 'M'){
            payload_r payload3 = { 'R' };
            RF24NetworkHeader header3(/*to node*/ s[i].addr, 'M');
            bool ok3 = network.write(header3,&payload3,sizeof(payload3));
            if (ok3)
              Serial.println(" Sent red to medium priority train.");
            else
              Serial.println(" failed to send the message to medium priority train.");
       }
       
       if (s[i].state == 'L'){
            payload_r payload4 = { 'R' };
            RF24NetworkHeader header4(/*to node*/ s[i].addr, 'M');
            bool ok4 = network.write(header4,&payload4,sizeof(payload4));
            if (ok4)
              Serial.println(" Sent red to low priority train.");
            else
              Serial.println(" failed to send the message to low priority train.");
       }       
       
    }
    delay(30);
    
    
    if (flag1==1){
     for( i=0; i<4; i++){
       if (s[i].state == 'H'){
            payload_r payload1 = { 'G' };
            RF24NetworkHeader header1(/*to node*/ s[i].addr, 'M');
            bool ok1 = network.write(header1,&payload1,sizeof(payload1));
            if (ok1){
              Serial.println(" Emergency train left,so sent green to high-priority train.");
              flag2=1;
            }
            else
              Serial.println(" failed to send the message to high-priority train.");
       }
       
       if (s[i].state == 'M'){
            payload_r payload2 = { 'R' };
            RF24NetworkHeader header2(/*to node*/ s[i].addr, 'M');
            bool ok2 = network.write(header2,&payload2,sizeof(payload2));
            if (ok2)
              Serial.println(" Sent red to medium-priority train.");
            else
              Serial.println(" failed to send the message to medium-priority train.");
       }
       
       if (s[i].state == 'L'){
            payload_r payload3 = { 'R' };
            RF24NetworkHeader header3(/*to node*/ s[i].addr, 'M');
            bool ok3 = network.write(header3,&payload3,sizeof(payload3));
            if (ok3)
              Serial.println(" Sent red to low-priority train.");
            else
              Serial.println(" failed to send the message to low-priority train.");
       }
      }
     }
     delay(30);
     
     if (flag1==1 || flag2==1){
      for( i=0; i<4; i++){
       if (s[i].state == 'M'){
            payload_r payload1 = { 'G' };
            RF24NetworkHeader header1(/*to node*/ s[i].addr, 'M');
            bool ok1 = network.write(header1,&payload1,sizeof(payload1));
            if (ok1){
              Serial.println(" emergency and high-priority trains left,so sent green to medium-priority train.");
              flag3=1;
            }
            else
              Serial.println(" failed to send the message to medium-priority train.");
       }
       
       if (s[i].state == 'L'){
            payload_r payload2 = { 'R' };
            RF24NetworkHeader header2(/*to node*/ s[i].addr, 'M');
            bool ok2 = network.write(header2,&payload2,sizeof(payload2));
            if (ok2)
              Serial.println(" Sent red to low priority train.");
               
            else
              Serial.println(" failed to send the message to low priority train.");
       }
      }
     } 
     delay(30); 
     
     if (flag3==1 || flag2==1 || flag1==1){
      for( i=0; i<4; i++){
       if (s[i].state == 'L'){
            payload_r payload1 = { 'G' };
            RF24NetworkHeader header1(/*to node*/ s[i].addr, 'M');
            bool ok1 = network.write(header1,&payload1,sizeof(payload1));
            if (ok1){
              Serial.println(" emergency, high-priority, and medium-priority trains left, so sent green to low-priority train.");
              flag4=1;
            }
            else
              Serial.println(" failed to send the message to low-priority train.");
        }
      }
     }     
    delay(30);
    
}
  


