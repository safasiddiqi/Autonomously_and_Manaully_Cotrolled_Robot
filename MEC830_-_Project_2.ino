#include <Servo.h>

//joystick pins
const int X_pin = 0;
const int Y_pin = 1;
const int SW_pin = 2;

//servo pins
const int servo_pin = 6;

//dc motor pins
const int Enable12 = 5;  // PWM pin to L293D's EN12 (pin 1) 
const int Driver1A = 4;  // To L293D's 1A (pin 2)
const int Driver2A = 3;  // To L293D's 2A (pin 7)

//sonic sensor pins
const int TRIG = 7;
const int ECHO = 8;

//led Pins
int red_light_pin= 12;
int green_light_pin = 11;
int blue_light_pin = 10;

//helper variables 
int mode = 0;
//tune neutral angle so car is going straight
//range of 35 degree gives a 15-25cm radius circle
const int neutral = 105;
const int range = 25;
const int right = neutral + range;
const int left = neutral - range;
float sensor;
float d;
int buttonState;
int prevButtonState;
//120 is the lowest pwm possible for going straight
//use 155-255 as range of pwm
int speed = 255;
bool obstacle = false;


Servo servo;

void setup() {
  Serial.begin(9600);
  
  //joy stick
  pinMode(SW_pin, INPUT_PULLUP);

  //dc motor
  pinMode(Enable12,OUTPUT);
  pinMode(Driver1A,OUTPUT);
  pinMode(Driver2A,OUTPUT);

  //servo
  servo.attach(servo_pin);

  //sonic sensor
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);

  //LED pins
  pinMode(red_light_pin, OUTPUT);
  pinMode(green_light_pin, OUTPUT);
  pinMode(blue_light_pin, OUTPUT);

  if (mode == 0)
  {
    RGB_color(255, 0, 0);
  } else if (mode == 1)
  {
    RGB_color(0, 255, 0);
  } else if (mode == 2)
  {
    RGB_color(0, 0, 255);
  }
  

  servo.write(neutral);
}

void loop() {
    //printlns for debugging
    // joystick direction is with wires facing away from you
    //Serial.println("X_pin: " + String(analogRead(X_pin))); 
    //Serial.println("Y_pin: " + String(analogRead(Y_pin))); // 0 right, 1023 left, 
    //Serial.println("SW_pin: " + String(digitalRead(SW_pin)));
    //Serial.println("Servo position: " + String(servo.read()));
    //Serial.println("Mode: " + String(mode));
    //Serial.println("Sensor Reading: " + String(Serial.println(sensor)));
    
    //task 1
    while (mode ==0)
    {
      Serial.println("We're now in mode 0");
      if (analogRead(Y_pin) > 450 && analogRead(Y_pin) < 550)
      {
        servo.write(neutral);
      } else if (analogRead(Y_pin) > 900)
      {
        //turn left
        servo.write(left-20);
      } else if (analogRead(Y_pin) < 100) 
      {
        //turn right
        servo.write(right+20);
      } 

      if (analogRead(X_pin) < 100)
      {
        motorCtrl(255, false);
      } else if (analogRead(X_pin) > 900){

        motorCtrl(255, true);
      } else{
        // maybe analog control will be viable
        motorCtrl(0,false);
      }
      
      if (!digitalRead(SW_pin)){
        mode = 1;
        RGB_color(0, 255, 0);
      }
    }

    //task 2
    while (mode == 1)
    {
      servo.write(neutral);
      motorCtrl(0,false);
      Serial.println("We're now in mode 1");
      delay(2000);
      motorCtrl(255, false);
      delay(1000);
      servo.write(right);
      delay(1000);
      // motorCtrl(int(-0.263*analogRead(X_pin)+255), false);
      //servo.write(neutral);
       //delay(300);
      servo.write(left);
      delay(1000);
      servo.write(neutral);
      //motorCtrl(int(-0.263*analogRead(X_pin)+255), false);
      delay(50);
      motorCtrl(0, false);

      while (true)
      {
        if (!digitalRead(SW_pin))
        {
          mode = 2;
          RGB_color(0, 0, 255);
          break;
        }
      }
    }

    //task 3
    if (mode == 2)
    {
      Serial.println("We're now in mode 2");
      // used to measure delay to distance relationship
//      delay(2000);
//      motorCtrl(255,false);
//      delay(1000);
//      motorCtrl(0,false);
      
      d = getDistance();
      servo.write(neutral);
      motorCtrl(255,false);
      delay((d+38-18)/0.16);
      motorCtrl(0, false);
      delay(2000);
      
      servo.write(right+20);
      motorCtrl(255,false);
      delay(1200);
      servo.write(neutral);
      delay(450);
      servo.write(left-20);
      delay(800);
      //motorCtrl(0,false);
      servo.write(right+20);
      delay(500);
      servo.write(neutral);
      motorCtrl(0, false);
      delay(1000);
      motorCtrl(255,false);
      d = 168 - (d+40);
      delay((d+38)/0.16);
      motorCtrl(0,false);
      
      mode = 0;
      RGB_color(255, 0, 0);
    }
}


void motorCtrl(byte speed, bool dir){
  //speed range 0-255
  analogWrite(Enable12,speed);  // PWM
  if (dir){
    digitalWrite(Driver1A,LOW);   // Boolean
    digitalWrite(Driver2A,HIGH);   // Boolean 
  } else {
    digitalWrite(Driver1A,HIGH);   // Boolean
    digitalWrite(Driver2A,LOW);   // Boolean 
  }
}

float getDistance()
{
    float pulse = 0;
    int sample = 10;
    for (int i=0; i<sample; i++)
    {
        digitalWrite(TRIG,LOW);
        delay(5);
        digitalWrite(TRIG,HIGH);
        delay(10);
        digitalWrite(TRIG, LOW);
        pulse += pulseIn(ECHO, HIGH);
        delay(10);
    }
    return (pulse/sample)*0.01848484288 + 0.421;
}

void RGB_color(int red_light_value, int green_light_value, int blue_light_value)
 {
  analogWrite(red_light_pin, red_light_value);
  analogWrite(green_light_pin, green_light_value);
  analogWrite(blue_light_pin, blue_light_value);
}
