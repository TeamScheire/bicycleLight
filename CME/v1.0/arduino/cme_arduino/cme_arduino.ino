/*
  cme_arduino.ino - Code to controle the lights of the bike of Carmen
  Created by Maarten Weyn, August 25, 2018.
  Released into the public domain.

  Required libraries
  - FastLED https://github.com/FastLED/FastLED/releases
  - LowPower https://github.com/rocketscream/Low-Power
*/


#include "FastLED.h"
#include "LowPower.h"

#define NUM_LEDS_BACK 17
#define NUM_LEDS_FRONT 72

#define PIN_BACK      12
#define PIN_FRONT     13

#define BUTTON_LEFT   2
#define BUTTON_RIGHT  3
#define BUTTON_LIGHTS 4
#define BUTTON_HORN   5

#define PIN_BATTERY   A0

#define BAT_COUNTER_MAX   4*60

CRGB leds_back[NUM_LEDS_BACK];
CRGB leds_front[NUM_LEDS_FRONT];

int light_status = 0;
int blink_left_status = 0;
int blink_right_status = 0;   
int horn_status = 0;

int brightness = 100;
byte button_lights_state = LOW;
byte button_left_state = LOW;
byte button_right_state = LOW;
byte button_horn_state = LOW;

int batt_status = 1;

void setup() {
  Serial.begin(9600);      // open the serial port at 9600 bps:
  
  FastLED.addLeds<NEOPIXEL, PIN_BACK> (leds_back,   NUM_LEDS_BACK);  
  FastLED.addLeds<NEOPIXEL, PIN_FRONT>(leds_front,  NUM_LEDS_FRONT);

  FastLED.setBrightness(brightness);

  pinMode(BUTTON_LIGHTS, INPUT_PULLUP);
  pinMode(BUTTON_LEFT, INPUT_PULLUP);
  pinMode(BUTTON_RIGHT, INPUT_PULLUP);
  pinMode(BUTTON_HORN, INPUT_PULLUP);
  
  button_left(digitalRead(BUTTON_LEFT));
  button_right(digitalRead(BUTTON_RIGHT));
  button_lights(digitalRead(BUTTON_LIGHTS));
  button_horn(digitalRead(BUTTON_HORN));
}

void lights(int on) {  
  light_status = on;
  if (on) {  
    if (batt_status)
    {
      // Front Light
      for (int i = 0; i < 2; i++) leds_front[i] = CRGB::White;
      for (int i = (NUM_LEDS_FRONT/2) - 5; i < (NUM_LEDS_FRONT/2) + 5; i++) leds_front[i] = CRGB::White;
      for (int i = 10; i < 20; i++) leds_front[i] = CRGB::White;
      for (int i = NUM_LEDS_FRONT-20; i < NUM_LEDS_FRONT-10; i++) leds_front[i] = CRGB::White;    
      for (int i = NUM_LEDS_FRONT-2; i < NUM_LEDS_FRONT; i++) leds_front[i] = CRGB::White;
      // Back Light
      for (int i = 0; i < NUM_LEDS_BACK; i++) leds_back[i] = CRGB::Red;
    } else {      
      for (int i = (NUM_LEDS_FRONT/2) - 2; i < (NUM_LEDS_FRONT/2) + 2; i++) leds_front[i] = CRGB::White;
      for (int i = (NUM_LEDS_BACK /2) - 2; i < (NUM_LEDS_BACK /2) + 2; i++) leds_back[i]  = CRGB::Red;
    }
    
  } else {    
    // Front Light
    for (int i = 0; i < NUM_LEDS_FRONT; i++) leds_front[i] = CRGB::Black;

    // Back Light
    for (int i = 0; i < NUM_LEDS_BACK; i++) leds_back[i] = CRGB::Black;
  }
  FastLED.show();
}

void button_left(int state) {
  button_left_state = state;
  if (state == HIGH) {
    set_blink_left(0);
  } else { 
    set_blink_left(1);
  }  
}

void button_right(int state) {
  button_right_state = state;
  if (state == HIGH) {
    set_blink_right(0);
  } else { 
    set_blink_right(1);
  }  
}

void button_lights(int state) {
  button_lights_state = state;
  if (state == HIGH) {
    lights(0);
  } else { 
    lights(1);
  }  
}

void button_horn(int state) {
  button_horn_state = state;
  if (state == HIGH) {
    set_horn(0);
  } else { 
    set_horn(1);
  }  
}

void blink_left()
{
  static int status = 0;
  if (!blink_left_status) return;
  
  status = !status;
  if (status) {  
    for (int i = NUM_LEDS_BACK-4; i < NUM_LEDS_BACK; i++) leds_back[i] = CRGB::Orange;
    for (int i = NUM_LEDS_FRONT-8; i < NUM_LEDS_FRONT; i++) leds_front[i] = CRGB::Orange;
  } else {
    for (int i = NUM_LEDS_BACK-4; i < NUM_LEDS_BACK; i++) leds_back[i] = CRGB::Black;
    for (int i = NUM_LEDS_FRONT-8; i < NUM_LEDS_FRONT; i++) leds_front[i] = CRGB::Black;
  }
  FastLED.show();
}

void set_blink_left(int on)
{
  blink_left_status = on;

  if (on) {  
    blink_left();
  } else {
    if (light_status) {    
      for (int i = NUM_LEDS_BACK-4; i < NUM_LEDS_BACK; i++) leds_back[i] = CRGB::Red;
      
      for (int i = NUM_LEDS_FRONT-2; i < NUM_LEDS_FRONT; i++) leds_front[i] = CRGB::White;
      for (int i = NUM_LEDS_FRONT-8; i < NUM_LEDS_FRONT-3; i++) leds_front[i] = CRGB::Black;
    } else {
      for (int i = NUM_LEDS_BACK-4; i < NUM_LEDS_BACK; i++) leds_back[i] = CRGB::Black;
      
      for (int i = NUM_LEDS_FRONT-8; i < NUM_LEDS_FRONT; i++) leds_front[i] = CRGB::Black;
    }
      
    FastLED.show();
  }
}

void blink_right()
{
  static int status = 0;
  if (!blink_right_status) return;
  
  status = !status;
  if (status) {  
    for (int i = 0; i < 4; i++) leds_back[i] = CRGB::Orange;
    for (int i = 0; i < 8; i++) leds_front[i] = CRGB::Orange;
  } else {
    for (int i = 0; i < 4; i++) leds_back[i] = CRGB::Black;
    for (int i = 0; i < 8; i++) leds_front[i] = CRGB::Black;
  }
  FastLED.show();
}

void set_blink_right(int on)
{
  blink_right_status = on;

  if (on) {  
    blink_right();
  } else {
    if (light_status) {    
      for (int i = 0; i < 4; i++) leds_back[i] = CRGB::Red;      
      for (int i = 0; i < 2; i++) leds_front[i] = CRGB::White;
      for (int i = 2; i < 8; i++) leds_front[i] = CRGB::Black;
    } else {
      for (int i = 0; i < 4; i++) leds_back[i] = CRGB::Black;      
      for (int i = 0; i < 8; i++) leds_front[i] = CRGB::Black;
    }
      
    FastLED.show();
  }
}

void blink_horn() {  
  static int status = 0;
  if (!horn_status) return;

  status = !status;
  if (status) {  
    // Front Light
    //for (int i = 0; i < NUM_LEDS_FRONT; i=i+5) leds_front[i] = CRGB::White;
    for (int i = 0; i < NUM_LEDS_FRONT; i++) leds_front[i] = CRGB::White;
    // Back Light
    for (int i = 0; i < NUM_LEDS_BACK; i++) leds_back[i] = CRGB::Red;
  } else {    
    // Front Light
    for (int i = 0; i < NUM_LEDS_FRONT; i++) leds_front[i] = CRGB::Black;
    // Back Light
    for (int i = 0; i < NUM_LEDS_BACK; i++) leds_back[i] = CRGB::Black;
  }
  FastLED.show();
}

void set_horn(int on)
{
  horn_status = on;

  if (on) {  
    FastLED.setBrightness(200);
    blink_horn();
  } else {
    FastLED.setBrightness(brightness);
    delay(100);
    if (light_status) {    
      lights(0);      
      delay(100);
      lights(1);
    } else {          
      lights(0);
    }
  }
}



void loop() {  
  static int batt_counter = 0;
  
  int button_horn_reading  = digitalRead(BUTTON_HORN);
  if (button_horn_reading  != button_horn_state) button_horn(button_horn_reading);

  if (horn_status)
  {
    blink_horn();
    delay(100);
    return;
  }

  if (batt_counter == BAT_COUNTER_MAX)
  {
    int batt = analogRead(PIN_BATTERY);
    if (batt < 530) {  
      if (batt_status) {
          //digitalWrite(LED_BUILTIN, HIGH);
          batt_status = 0;
          if (light_status) {
              for (int i = 0; i < NUM_LEDS_FRONT; i++) leds_front[i] = CRGB::Black;
              for (int i = 0; i < NUM_LEDS_BACK; i++) leds_back[i] = CRGB::Black;
              lights(1);
          }
      }
    } else if (batt > 560){
      if (!batt_status) {
        //digitalWrite(LED_BUILTIN, LOW);
        batt_status = 1;      
        if (light_status) {
            for (int i = 0; i < NUM_LEDS_FRONT; i++) leds_front[i] = CRGB::Black;
            for (int i = 0; i < NUM_LEDS_BACK; i++) leds_back[i] = CRGB::Black;
            lights(1);
        }   
      }
    }
      
    Serial.print(batt);
    Serial.print(" - ");
    Serial.println(batt_status);
    batt_counter = 0;
  }
    

  int button_light_reading  = digitalRead(BUTTON_LIGHTS);
  int button_left_reading   = digitalRead(BUTTON_LEFT);
  int button_right_reading  = digitalRead(BUTTON_RIGHT);
  
  if (button_light_reading  != button_lights_state) button_lights(button_light_reading);
  if (button_left_reading   != button_left_state) button_left(button_left_reading);
  if (button_right_reading  != button_right_state) button_right(button_right_reading);

  if (blink_left_status) blink_left();
  if (blink_right_status) blink_right();


  batt_counter++;
  
  //delay(250); // when debuggin using delay
  LowPower.powerDown(SLEEP_250MS, ADC_OFF, BOD_OFF); // for low power use lowpower library
  
}
