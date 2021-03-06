#include <IRremote.h>
int RECV_PIN = 12;
IRrecv irrecv(RECV_PIN);
decode_results results;
#define IR_Go       350990415
#define IR_Back    350976135
#define IR_Left    351004695
#define IR_Right   350972055
#define IR_Stop    351002655
#define IR_ESC     4294967295
#define IR_Drift_Right 350988885
#define IR_Drift_Left 350956245
#define IR_No 350951655

#define Lpwm_pin  5     //adjusting speed 
#define Rpwm_pin  6    //adjusting speed //

int pinLB=2;           // defining pin2 left rear
int pinLF=4;           // defining pin4 left front
int pinRB=7;           // defining pin7 right rear
int pinRF=8;           // defining pin8 right front
unsigned char Lpwm_val =150;
unsigned char Rpwm_val = 150;
int Car_state=0;
unsigned long lastCommand =  2160022444;
int forward = 0;

#include <IRremote.h>
#include <Servo.h>

Servo myservo;

#include "SR04.h"
#define TRIG_PIN A1
#define ECHO_PIN A0
SR04 sr04 = SR04(ECHO_PIN,TRIG_PIN);
long a;


void M_Control_IO_config(void)
{
  pinMode(pinLB,OUTPUT); // pin2
  pinMode(pinLF,OUTPUT); // pin4
  pinMode(pinRB,OUTPUT); // pin7 
  pinMode(pinRF,OUTPUT); // pin8
  pinMode(Lpwm_pin,OUTPUT); // pin5 (PWM) 
  pinMode(Rpwm_pin,OUTPUT); // pin6 (PWM)   
}
void Set_Speed(unsigned char Left,unsigned char Right)
{
  analogWrite(Lpwm_pin,Left);
  analogWrite(Rpwm_pin,Right);
}

void driftRight(){
     digitalWrite(pinRB,LOW);  //making motor move towards right rear
     digitalWrite(pinRF,HIGH);
     digitalWrite(pinLB,HIGH);
     digitalWrite(pinLF,LOW);  //making motor move towards left front
}

void driftLeft(){     
     digitalWrite(pinRB,HIGH);
     digitalWrite(pinRF,LOW );   //making motor move towards right front
     digitalWrite(pinLB,LOW);   //making motor move towards left rear
     digitalWrite(pinLF,HIGH);
}
void advance()     // going forward
    {
     digitalWrite(pinRB,LOW);  // making motor move towards right rear
     digitalWrite(pinRF,HIGH);
     digitalWrite(pinLB,LOW);  // making motor move towards left rear
     digitalWrite(pinLF,HIGH); 
     Car_state = 1; 
     forward = 1;  
    }
void turnR()        //turning right(dual wheel)
    {
     int del = 20;
     if(forward == 0){
        del = 100;
     }
     digitalWrite(pinRB,LOW);  //making motor move towards right rear
     digitalWrite(pinRF,HIGH);
     digitalWrite(pinLB,HIGH);
     digitalWrite(pinLF,LOW);  //making motor move towards left front
          delay(del);
     if(forward == 1)
        advance();
     else if (forward == -1)
        back();
     else
        stopp();
     Car_state = 4;
    }
void turnL()        //turning left(dual wheel)
    {
     int del = 20;
     if(forward == 0){
        del = 100;
     }
     digitalWrite(pinRB,HIGH);
     digitalWrite(pinRF,LOW );   //making motor move towards right front
     digitalWrite(pinLB,LOW);   //making motor move towards left rear
     digitalWrite(pinLF,HIGH);
     delay(del);
     if(forward == 1)
        advance();
     else if(forward == -1)
        back();
     else 
        stopp();
     Car_state = 3;
    }    
void stopp()         //stop
    {
     digitalWrite(pinRB,HIGH);
     digitalWrite(pinRF,HIGH);
     digitalWrite(pinLB,HIGH);
     digitalWrite(pinLF,HIGH);
     Car_state = 5;
     forward = 0;
     
    }
void back()          //back up
    {
     digitalWrite(pinRB,HIGH);  //making motor move towards right rear
     digitalWrite(pinRF,LOW);
     digitalWrite(pinLB,HIGH);  //making motor move towards left rear
     digitalWrite(pinLF,LOW);
     Car_state = 2;  
     forward = -1;
    }

void sayNo(){
  myservo.write(45);
  delay(400);
  myservo.write(90);
}

         
void IR_Control(void)
{
   a=sr04.Distance();
   if(a < 15){
      back();
      delay(200);
      stopp();
      sayNo();
   }
   
   unsigned long Key;
   if(irrecv.decode(&results)) //judging if serial port receives data   
 {
    Serial.println(results.value);
     Key = results.value;
     if(Key == IR_ESC){
      Key = lastCommand;
     }
    switch(Key)
     {
       case IR_Go:advance();   //UP
       lastCommand = IR_Go;
       break;
       case IR_Back: back();   //back
       lastCommand = IR_Back;
       break;
       case IR_Left:turnL();   //Left   
       lastCommand = IR_Left; 
       break;
       case IR_Right:turnR(); //Righ
       lastCommand = IR_Right; 
       break;
       case IR_Stop:stopp();   //stop
       lastCommand = IR_Stop; 
       break;
       case IR_Drift_Right:driftRight();
       lastCommand = IR_Drift_Right;
       break;
       case IR_Drift_Left:driftLeft();
       lastCommand = IR_Drift_Left;
       break;
       case IR_No:sayNo();
       lastCommand = IR_No; 
       default: 
       break;      
     } 
     irrecv.resume(); // Receive the next value
    } 
  
}
void setup() 
{ 
   M_Control_IO_config();
   Set_Speed(Lpwm_val,Rpwm_val);
   irrecv.enableIRIn(); // Start the receiver
   Serial.begin(9600);   //initializing serial port, Bluetooth used as serial port, setting baud ratio at 9600 
   stopp();  
   myservo.attach(A2);

   myservo.write(90);
}
void loop() 
{  
   
   IR_Control();
   
   
}
