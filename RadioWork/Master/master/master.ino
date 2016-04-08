/*
 * Copyright: Charles Kingston and Kevin Kredit
 * 
 * Arduino Leonardo code that sets up a mesh network. After setting up the 
 * mesh network it becomes the master on the network. This sketch also 
 * contains code to read data coming in from a flow sensors as pulses.
 */
  
#include "RF24Network.h"
#include "RF24.h"
#include "RF24Mesh.h"
#include <SPI.h>
//Include eeprom.h for AVR (Uno, Nano) etc. except ATTiny
#include <EEPROM.h>
#include <TimerOne.h>
#include "/Users/charleskingston/Desktop/SeniorDesign/RadioWork/Shared/settings.h"

/* Randos */
bool dummyValveCommand = true;
char networkRequestChar;
int myNodeID = 0;
int counter = 0;

/********** MESH network vars **************/
uint32_t displayTimer = 0;
int printcount = 0;
bool hadButtonPress = false;
bool LEDYstate = false;
bool defaultNetworkBlink = false;
/*******************************************/

/***** Configure I/O ports with #define ****/
#define BUTTON  3
#define LEDR    4
#define LEDG    5
#define LEDY    6
#define CE      9
#define CS      10
/*******************************************/
#define KEVINID 2

/***** Configure the chosen CE,CS pins *****/
RF24 radio(CE,CS);
RF24Network network(radio);
RF24Mesh mesh(radio,network);
/*******************************************/

void setup() {
  pinMode(BUTTON, INPUT_PULLUP);
  pinMode(LEDR, OUTPUT);
  pinMode(LEDG, OUTPUT);
  pinMode(LEDY, OUTPUT);
  //while(BUTTON); /////////// wait for a button press to continue, allowing for user to setup terminal
  delay(5000);
  attachInterrupt(digitalPinToInterrupt(BUTTON), handleButton, FALLING);  
  digitalWrite(LEDR, LOW);
  digitalWrite(LEDG, LOW);
  digitalWrite(LEDY, LOW);

  digitalWrite(LEDR, HIGH);
  delay(500);
  digitalWrite(LEDR, LOW);
  digitalWrite(LEDY, HIGH);
  
  Serial.begin(9600);
  //radio.setPALevel(RF24_PA_LOW);
  // Set the nodeID to 0 for the master node
  mesh.setNodeID(myNodeID);
  Serial.println(mesh.getNodeID());
  // Connect to the mesh
  Serial.print("Output of mesh.begin(): ");
  Serial.println(mesh.begin(COMM_CHANNEL, DATA_RATE, CONNECT_TIMEOUT));
  //mesh.requestAddress(0);
  mesh.setAddress('0'-48,0);

  digitalWrite(LEDY, LOW);
  digitalWrite(LEDR, HIGH);
  delay(250);
  digitalWrite(LEDR, LOW);
  delay(250);
  digitalWrite(LEDR, HIGH);
  delay(250);
  digitalWrite(LEDR, LOW);

  Timer1.initialize(5000000);
  Timer1.attachInterrupt(printStatus);
}

/*
 * Regardless of current state, send message to other to turn on LEDY
 */
void handleButton(){
  if (counter > 0){
    hadButtonPress = true;
    //Serial.println(F("Deteceted Button Press"));
  } else {
    counter++;
  }
}

void printStatus(){
  Serial.println("");
  Serial.println(printcount++);
  Serial.print("\nMy ID:      ");
  Serial.println(mesh.getNodeID());
  Serial.print("My address: ");
  Serial.println(mesh.getAddress(myNodeID));
  Serial.println(" ");
  Serial.println(F("********Assigned Addresses********"));
  for(int i=0; i<mesh.addrListTop; i++){
    Serial.print("NodeID: ");
    Serial.print(mesh.addrList[i].nodeID);
    Serial.print(" RF24Network Address: 0");
    Serial.println(mesh.addrList[i].address,OCT);
 }
  Serial.println(F("**********************************"));
}

void loop() {    

  // Call mesh.update to keep the network updated
  mesh.update();
  
  // In addition, keep the 'DHCP service' running on the master node so addresses will
  // be assigned to the sensor nodes
  mesh.DHCP();
  
  
  // Check for incoming data from the sensors
  if(network.available()){
    RF24NetworkHeader header;
    network.peek(header);
    Serial.print(F("Received ")); Serial.print(char(header.type)); Serial.println(F(" type message."));
    Serial.print(F("From [logical address]: ")); Serial.println(header.from_node);
    Serial.print(F("From [converted to nodeID]: ")); Serial.println(mesh.getNodeID(header.from_node));
    
    uint32_t dat=0;
    bool boolMessage;
    float flowSensorData = -1;
    // idk if initializing this value is allowed...
    int valveResponse = -1;
    int networkStatusRequest;
    
    switch(header.type){
      // valve flowrate sensor 
      case SEND_FLOW_RATE_H:
        network.read(header,&flowSensorData,sizeof(flowSensorData));
        if (flowSensorData == float(-1)) {
          Serial.println("Something went terribly wrong with the flowSensorData...");
        } else {
          Serial.print(flowSensorData); 
          Serial.println(" GPM");
        }
        break;
      // read a valve response from one of the nodes
      case SEND_VALVE_H:
        network.read(header, &valveResponse, sizeof(valveResponse));
        if (valveResponse == -1){
          Serial.println("The status is unknown");
        } else if (valveResponse == 0) {
          Serial.println("The valve is closed");
        } else if (valveResponse == 1){
          Serial.println("The valve is open");
        } else {
          Serial.println("Something went terribly wrong with the valveResponse...");
        }
        break;
      case SEND_NODE_STATUS_H:
        network.read(header, &networkStatusRequest, sizeof(networkStatusRequest));
        if (networkStatusRequest == 1){
          Serial.println("We're solid");
        } else if (networkStatusRequest == 2) {
          Serial.println("There's a low battery warning");
        } else if (networkStatusRequest == 3){
          Serial.println("There's a valve error");
        } else {
          Serial.println("Something went terribly wrong with the valveResponse...");
        }
        break;
      default: 
        //network.read(header,0,0); 
        defaultNetworkBlink = !defaultNetworkBlink;
        digitalWrite(LEDR, defaultNetworkBlink);
        network.read(header,&dat,sizeof(dat)); 
        Serial.print("Message recieved: "); Serial.print(dat); Serial.println(", but is undefined");
        break;
    }
  }
  if (hadButtonPress){
    mesh.write(mesh.getAddress(KEVINID), &dummyValveCommand, SET_VALVE_H, sizeof(dummyValveCommand));
    Serial.print("Sending the valve command: "); Serial.println(dummyValveCommand);
    dummyValveCommand = !dummyValveCommand;
    hadButtonPress = false;
  }
}