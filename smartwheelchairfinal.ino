// smartwhellchair GEC ENE 


#include <SPI.h>      
#include "RF24.h"     

const int enbA = 3;
const int enbB = 5;


const int IN1 = 11;    //Right Motor (-)
const int IN2 = 4;    //Right Motor (+)
const int IN3 = 7;    //Left Motor (+)
const int IN4 = 6;    //Right Motor (-)


const int blue = 30; 
const int red =  31;

const int trigPin = 12;
const int echoPin = 13;

long duration;
int distance;

const byte ledPin = 32;
volatile byte state = LOW;

int RightSpd = 50;
int LeftSpd = 70;

//Define packet for the direction (X axis and Y axis)
int data[2];

//Define object from RF24 library - 9 and 10 are a digital pin numbers to which signals CE and CSN are connected
RF24 radio(9,10);

//Create a pipe addresses for the communicate
const uint64_t pipe = 0xE8E8F0F0E1LL;

void setup(){
  setupultra();
  setupwheel(); 
  pinMode(ledPin, OUTPUT);
  Serial.begin(9600); 
  attachInterrupt(0 , blink, RISING);
}





void setupultra() 
{
pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
pinMode(echoPin, INPUT); // Sets the echoPin as an Input
Serial.begin(9600); // Starts the serial communication
}

void setupwheel(){
  //Define the motor pins as OUTPUT
  pinMode(enbA, OUTPUT);
  pinMode(enbB, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(blue,OUTPUT);
  pinMode(red,OUTPUT);
  
  Serial.begin(9600);
  radio.begin();                    //Start the nRF24 communicate            
  radio.openReadingPipe(1, pipe);   //Sets the address of the transmitter to which the program will receive data.
  radio.startListening();             
  }

void loop(){
  loopultra();
  loopwheel();
  digitalWrite(ledPin,state);
  if ( state == HIGH)
  {
    state =LOW;
    SendSMS();
  }
}


void blink() {
  state = HIGH;
}
void SendSMS()
{
  Serial.println("AT+CMGF=1");    //To send SMS in Text Mode
  delay(1000);
  Serial.println("AT+CMGS=\"+918668707467\"\r"); //Change to destination phone number 
  delay(1000);
  Serial.println("i need help plz help me ");//the content of the message
  delay(200);
  Serial.println((char)26); //the stopping character Ctrl+Z
  delay(1000);  


  
}



void loopultra() {
// Clears the trigPin
digitalWrite(trigPin, LOW);
delayMicroseconds(2);
// Sets the trigPin on HIGH state for 10 micro seconds
digitalWrite(trigPin, HIGH);
delayMicroseconds(10);
digitalWrite(trigPin, LOW);
// Reads the echoPin, returns the sound wave travel time in microseconds
duration = pulseIn(echoPin, HIGH);
// Calculating the distance
distance= duration*0.034/2;
// Prints the distance on the Serial Monitor
Serial.print("Distance: ");
Serial.println(distance);
}

void loopwheel(){
  if (radio.available()){
    radio.read(data, sizeof(data));

    if(data[0] > 380){
      //forward            
      analogWrite(enbA, RightSpd);
      analogWrite(enbB, LeftSpd);    
      digitalWrite(IN1, HIGH);
      digitalWrite(IN2, LOW);
      digitalWrite(IN3, HIGH);
      digitalWrite(IN4, LOW);
      digitalWrite(blue, HIGH);
      digitalWrite(red, LOW);
    }
    
    if(data[0] < 310 ){
      //backward              
      analogWrite(enbA, RightSpd);
      analogWrite(enbB, LeftSpd);
      digitalWrite(IN1, LOW);
      digitalWrite(IN2, HIGH);
      digitalWrite(IN3, LOW);
      digitalWrite(IN4, HIGH);
      digitalWrite(red, HIGH);
      digitalWrite(blue, LOW);
    }
     
   
    if(data[1] > 180){
      //left
      analogWrite(enbA, RightSpd);
      analogWrite(enbB, LeftSpd);
      digitalWrite(IN1, LOW);
      digitalWrite(IN2, HIGH);
      digitalWrite(IN3, HIGH);
      digitalWrite(IN4, LOW);
      digitalWrite(blue, LOW);
      digitalWrite(red, LOW);
    }

    if(data[1] < 110){
      //right
      analogWrite(enbA, RightSpd);
      analogWrite(enbB, LeftSpd);
      digitalWrite(IN1, HIGH);
      digitalWrite(IN2, LOW);
      digitalWrite(IN3, LOW);
      digitalWrite(IN4, HIGH);
      digitalWrite(blue, LOW);
      digitalWrite(red, LOW);
    }

    if((data[0] > 330 && data[0] < 360 && data[1] > 130 && data[1] < 160)|| distance < 10 ){
      //stop car
      analogWrite(enbA, 0);
      analogWrite(enbB, 0);
      digitalWrite(blue, LOW);
      digitalWrite(red, LOW);
    }
  }
}
