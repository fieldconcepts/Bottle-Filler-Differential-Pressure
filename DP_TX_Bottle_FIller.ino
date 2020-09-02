/*
  Automatic Bottle Filler - DP
  ----------------------------------------
  
  NXP MPX5010DP Pressure Sensor (x2)
  1/4" Plastic Metal Solenoid Valve 2 Way DC 12V 0-120 PSI (x2)
  Arduino UNO (x1)
  12v Power adaptor 240VAC (x1)
  LDR Light Dependent Resistor (x2)
  Green LED (x2)
  Red LED (x2)
  
  ---------------------------------------

  Ardunio controlled automatic bottle filler that is controlled 
  via a serial interface from a PC (9600 Baud). 
  
  The filling station 
  uses LDRs to detect the presence of an empty bottle. The LDR act as 
  an interlock and will not allow filling to commence unless bottle is detected.
  A red LED is turned ON when bottle is detected. 
  
  Operator can commnece filling by sending comand from PC (f). 
  A green LED is turned on during fill. The differential pressure sensor port is 
  connected to tubing that is set to the desired fill depth in the bottle. 
  As fluid fills the bottle it will eventually reach the fill line and pressure 
  will spike on the  sensor above the set threshold. The solenoid shuts off and green LED turns
  off. Filling is stopped. Manual overide by sending (q) command.

  LED test function to ensure all LEDS are good.

  Zeroing sensor function before operating.
  
  
  modified 02/09/2020
  by Ben Sherwell
*/


//Serial variables
String command;

//Sensor variables
float rawValue;
float avgValue;
float zeroValue;
int sampleSize = 10;


float data;

int ldrValue1;
int ldrValue2;

int triggerThreshold1 = -5;
int triggerThreshold2 = -5;

int ldrThreshold1 = 980;
int ldrThreshold2 = 980;

//States
int fillState = 0;

int bottleState1 = 0;
int bottleState2 = 0;

int pumpState = 0; //pump on or of state
int relayState = 0;

//Pin Allocations

const int pressurePin1 = A0; //analog input for pressure sensor 1
const int pressurePin2 = A1; //analog input for pressure sensor 2

const int ldrPin1 = A3; //analog input for LDR 1
const int ldrPin2 = A4; //analog input for LDR 2

const int relayPin1 = 2; // digital pin output for relay 1
const int relayPin2 = 3; // digital pin output for relay 2

const int bottleLEDPin1 =  4; //control pin for LDR LED 1
const int bottleLEDPin2 =  6; //control pin for LDR LED 2

const int pumpLEDPin1 =  8; //control pin for Pump LED 1
const int pumpLEDPin2 =  10; //control pin for Pump LED 2


//Setup
void setup() {
    Serial.begin(9600);
    Serial.println("Differential Pressure Filler");
    Serial.println("-------------------");
    Serial.println("");
    Serial.println("z - zero sensor");
    Serial.println("f - fill bottle");
    Serial.println("q - manual stop");
    Serial.println("p - single data point");
    Serial.println("b - read bottle sensor");
    Serial.println("t - test LEDS");
    Serial.println("h - help");
    Serial.println("");
    
    pinMode(pumpLEDPin1, OUTPUT); 
    digitalWrite(pumpLEDPin1, LOW);
    pinMode(bottleLEDPin1, OUTPUT); 
    digitalWrite(bottleLEDPin1, LOW);
    pinMode(relayPin1, OUTPUT); 
    digitalWrite(relayPin1, LOW);

    pinMode(pumpLEDPin2, OUTPUT); 
    digitalWrite(pumpLEDPin2, LOW);
    pinMode(bottleLEDPin2, OUTPUT); 
    digitalWrite(bottleLEDPin2, LOW);
    
    delay(200);
  
}

//Main 
void loop() {
    serialComms();
    checkBottle();
    fillBottle();

}

void checkBottle(){
    ldrValue1 = analogRead(ldrPin1);
    ldrValue2 = analogRead(ldrPin2);
    
    if (ldrValue1 < ldrThreshold1){
        bottleState1 = 1;
        digitalWrite(bottleLEDPin1, bottleState1);
    }
    else {
        bottleState1 = 0;
        digitalWrite(bottleLEDPin1, bottleState1);
    }

    if (ldrValue2 < ldrThreshold2){
        bottleState2 = 1;
        digitalWrite(bottleLEDPin2, bottleState2);
    }
    else {
        bottleState2 = 0;
        digitalWrite(bottleLEDPin2, bottleState2);
    }  
}

void fillBottle(){
    if (fillState == 1){
    
      rawValue= analogRead(pressurePin1);
      data = rawValue - avgValue;
      Serial.print("Pressure: ");
      Serial.print(data);
      
      ldrValue1= analogRead(ldrPin1);
      Serial.print("  LDR: ");
      Serial.println(ldrValue1);
      
      
      delay(100);

      if (data < triggerThreshold1){
          Serial.println("Pressure Spiked!");
          stop_fill();
          help();
      }

      if (ldrValue1 > ldrThreshold1){
          Serial.println("Bottle not detected!");
          stop_fill();
          help();
      }
    }
}


void serialComms(){
    if(Serial.available()){
        command = Serial.readStringUntil('\n');
        
        if(command.equals("f")){
            start_fill();
        }
        else if(command.equals("q")){
            stop_fill();
        }
        else if(command.equals("z")){
            zero_sensor();
        }
        else if(command.equals("p")){
            get_data_point();
        }
        else if(command.equals("b")){
            read_ldr();
        }
        else if(command.equals("t")){
            test_leds();
        }
        else if(command.equals("h")){
            help();
        }
        else{
            Serial.println("Invalid command");
        }
    }
}

void start_fill(){

    if (fillState == 0 && bottleState1 == 0){
       Serial.println("No bottle detected....");
    }
    
    
    if (fillState == 0 && bottleState1 == 1){
        Serial.println("Fill ON....");
        zero_sensor();
        fillState = 1;

        if (pumpState == 0){
            pumpState = 1;
            relayState = 1;
            digitalWrite(pumpLEDPin1, pumpState);
            digitalWrite(relayPin1, relayState);
        }
    }
}

void stop_fill(){
    if (fillState == 1){
        Serial.println("Fill OFF....");
        fillState = 0;

        if (pumpState == 1){
            pumpState = 0;
            relayState = 0;
            digitalWrite(pumpLEDPin1, pumpState);
            digitalWrite(relayPin1, relayState);
        }
    }
}

void zero_sensor(){
    if (fillState == 0){
        Serial.println("-------------------");  
        Serial.println("Zeroing sensor...");
        rawValue= analogRead(pressurePin1);
        avgValue = 0;
        
         for (int i = 0; i <sampleSize; i++) {
            Serial.print(rawValue);
            Serial.print(" ");
            avgValue += rawValue;
            delay(10);
        }
        avgValue = avgValue/sampleSize;
        Serial.println("");
        Serial.print("Average over ");
        Serial.print(sampleSize);
        Serial.print(" samples: ");
        Serial.println(avgValue);
        Serial.print("Applying offset... ");
        zeroValue = rawValue - avgValue;
        Serial.println(zeroValue);
        Serial.println("");     
    }
    else if (fillState == 1){
        Serial.println("Turn fill OFF ('q') before zeroing"); 
    }
}

void get_data_point(){
    if (fillState == 0){
        Serial.println("-------------------"); 
        Serial.println("Getting single data point...");
    
        if (pumpState == 0){
            pumpState = 1;
            digitalWrite(pumpLEDPin1, pumpState);
        }
        
        rawValue= analogRead(pressurePin1);
        data = rawValue - avgValue;
        Serial.print("Pressure: ");
        Serial.println(data);
        Serial.println("");
    
        delay(100);
        
        pumpState = 0;
        digitalWrite(pumpLEDPin1, pumpState);
    }
    else if (fillState == 1){
        Serial.println("Turn fill OFF ('q') before getting data point"); 
    }
}

void read_ldr(){
    ldrValue1 = analogRead(ldrPin1);
    ldrValue2 = analogRead(ldrPin2);
    
    for (int i = 0; i < 20; i++) {
            Serial.print("LDR 1: ");
            Serial.print(ldrValue1);
            Serial.print(", LDR 2: ");
            Serial.println(ldrValue2);
            delay(100);
    }
}

void test_leds(){
  Serial.println("Testing LEDS... ");
  
  for (int i = 0; i < 2; i++) {
      digitalWrite(pumpLEDPin1, 1);
      digitalWrite(pumpLEDPin2, 1);
      digitalWrite(bottleLEDPin1, 1);
      digitalWrite(bottleLEDPin2, 1);
    
      delay(500);
    
      digitalWrite(pumpLEDPin1, 0);
      digitalWrite(pumpLEDPin2, 0);
      digitalWrite(bottleLEDPin1, 0);
      digitalWrite(bottleLEDPin2, 0);
    
      delay(250);
  }

  digitalWrite(pumpLEDPin1, 1);
  digitalWrite(pumpLEDPin2, 1);
  digitalWrite(bottleLEDPin1, 1);
  digitalWrite(bottleLEDPin2, 1);

  delay(1000);

  digitalWrite(pumpLEDPin1, 0);
  digitalWrite(pumpLEDPin2, 0);
  digitalWrite(bottleLEDPin1, 0);
  digitalWrite(bottleLEDPin2, 0);

  
}

void help(){
    Serial.println("-----------------------");
    Serial.println("z - zero sensor");
    Serial.println("f - fill bottle");
    Serial.println("q - manual stop");
    Serial.println("p - single data point");
    Serial.println("b - read bottle sensor");
    Serial.println("t - test LEDS");
    Serial.println("h - help");
    Serial.println("");
  
}
