/*
  cme_arduino.ino - Code to control the lights of the bike of Carmen
  Created by Maarten Weyn, August 25, 2018.
  Released into the public domain.

  Required libraries
  - FastLED https://github.com/FastLED/FastLED/releases
  - LowPower https://github.com/rocketscream/Low-Power
*/

#include "FastLED.h"
#include "LowPower.h"

#define NUM_LEDS_BACK 17
#define NUM_LEDS_FRONT 71
#define NUM_LEDS_FRONT_SIDE 6
#define NUM_LEDS_BACK_SIDE 2

#define BACK_LIGHT_GROUP  5
#define FRONT_LIGHT_GROUP  10


#define PIN_BACK      6
#define PIN_FRONT     7

#define BUTTON_LEFT   2 // yellow
#define BUTTON_RIGHT  3 // green
#define BUTTON_LIGHTS 4 // blue
#define BUTTON_HORN   5 // white

#define PIN_ACTIVATE_LED_ON     11 
#define PIN_ACTIVATE_LED_OFF    8 

#define PIN_HORN                10 


#define PIN_BATTERY   A0

#define BAT_COUNTER_MAX   4*60

CRGB leds_back[NUM_LEDS_BACK];
CRGB leds_front[NUM_LEDS_FRONT+1];

int light_status = 0;
int blink_left_status = 0;
int blink_right_status = 0;   
int horn_status = 0;
int led_strip_power = 0;

int brightness = 255;
byte button_lights_state = LOW;
byte button_left_state = LOW;
byte button_right_state = LOW;
byte button_horn_state = LOW;

int show_led = 0;

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
  
  pinMode(PIN_ACTIVATE_LED_OFF, OUTPUT);
  pinMode(PIN_ACTIVATE_LED_ON, OUTPUT);
  pinMode(PIN_HORN, OUTPUT);

  //digitalWrite(PIN_ACTIVATE_LED_OFF, HIGH);
  //delay(10);
  //digitalWrite(PIN_ACTIVATE_LED_OFF, LOW);  
  
  button_left(digitalRead(BUTTON_LEFT));
  button_right(digitalRead(BUTTON_RIGHT));
  button_lights(digitalRead(BUTTON_LIGHTS));
  button_horn(digitalRead(BUTTON_HORN));

}

void show_leds()
{  
  Serial.println("show leds");

  if (!led_strip_power)
  {
      digitalWrite(PIN_ACTIVATE_LED_ON, HIGH);
      delay(10);
      digitalWrite(PIN_ACTIVATE_LED_ON, LOW); 
      delay(100);
      led_strip_power = 1;
  }
  
  FastLED.show();
  if (!(light_status ||  blink_left_status || blink_right_status || horn_status))
  {
      digitalWrite(PIN_ACTIVATE_LED_OFF, HIGH);
      delay(10);
      digitalWrite(PIN_ACTIVATE_LED_OFF, LOW); 
      led_strip_power = 0;
  }

  show_led = 0;
}


void update_lights()
{
  static int back_light_position = 0;
  static int front_light_position = 0;

  if (back_light_position >= 0) {
    for (int i = NUM_LEDS_BACK_SIDE; i < NUM_LEDS_BACK_SIDE + back_light_position; i=i+1) leds_back[i] = CRGB::Black;    
    for (int i = NUM_LEDS_BACK_SIDE + back_light_position; i < NUM_LEDS_BACK_SIDE + back_light_position + BACK_LIGHT_GROUP; i=i+1) leds_back[i] = CRGB::Red;   
    back_light_position++;

    if (back_light_position > NUM_LEDS_BACK - (2 * NUM_LEDS_BACK_SIDE) - BACK_LIGHT_GROUP)   {
      back_light_position = -1;
    }
  } else {
    for (int i = NUM_LEDS_BACK - NUM_LEDS_BACK_SIDE + back_light_position - BACK_LIGHT_GROUP; i < NUM_LEDS_BACK - NUM_LEDS_BACK_SIDE + back_light_position; i=i+1) leds_back[i] = CRGB::Red;    
    for (int i = NUM_LEDS_BACK - NUM_LEDS_BACK_SIDE + back_light_position; i < NUM_LEDS_BACK - NUM_LEDS_BACK_SIDE ; i=i+1) leds_back[i] = CRGB::Black;
    back_light_position--;

    if (back_light_position < -(NUM_LEDS_BACK - (2 * NUM_LEDS_BACK_SIDE) - BACK_LIGHT_GROUP))   {
      back_light_position = 1;
    }
  }

  if (front_light_position >= 0) {
    for (int i = NUM_LEDS_FRONT_SIDE + 10; i < NUM_LEDS_FRONT_SIDE + 10 + front_light_position; i=i+1) leds_front[i] = CRGB::Black;    
    for (int i = NUM_LEDS_FRONT_SIDE + 10 + front_light_position; i < NUM_LEDS_FRONT_SIDE + 10 + front_light_position + FRONT_LIGHT_GROUP; i=i+1) leds_front[i] = CRGB::White;   
    front_light_position++;

    if (front_light_position > NUM_LEDS_FRONT - (2 * (NUM_LEDS_FRONT_SIDE + 10)) - FRONT_LIGHT_GROUP)   {
      front_light_position = -1;
    }
  } else {
    for (int i = NUM_LEDS_FRONT - NUM_LEDS_FRONT_SIDE - 10 + front_light_position - FRONT_LIGHT_GROUP; i < NUM_LEDS_FRONT - 10 - NUM_LEDS_FRONT_SIDE + front_light_position; i=i+1) leds_front[i] = CRGB::White;    
    for (int i = NUM_LEDS_FRONT - NUM_LEDS_FRONT_SIDE - 10  + front_light_position; i < NUM_LEDS_FRONT - NUM_LEDS_FRONT_SIDE - 10 ; i=i+1) leds_front[i] = CRGB::Black;
    front_light_position--;

    if (front_light_position < -(NUM_LEDS_FRONT - (2 * (NUM_LEDS_FRONT_SIDE + 10)) - FRONT_LIGHT_GROUP))   {
      front_light_position = 1;
    }
  }

  show_led = 1;
}

void lights(int on) {  
  light_status = on;
  if (on) {      
    if (batt_status)
    {
      // Front Light
      for (int i = 0; i < NUM_LEDS_FRONT_SIDE; i++) leds_front[i] = CRGB::Blue;   
      for (int i = NUM_LEDS_FRONT-NUM_LEDS_FRONT_SIDE; i < NUM_LEDS_FRONT; i++) leds_front[i] = CRGB::Blue;
      
      for (int i = NUM_LEDS_FRONT_SIDE + 5; i < NUM_LEDS_FRONT_SIDE + 10; i++) leds_front[i] = CRGB::White;
      for (int i = NUM_LEDS_FRONT - NUM_LEDS_FRONT_SIDE - 10; i < NUM_LEDS_FRONT - NUM_LEDS_FRONT_SIDE - 5; i++) leds_front[i] = CRGB::White;

      
      //for (int i = 10; i < 16; i++) leds_front[i] = CRGB::White;
      //for (int i = NUM_LEDS_FRONT-20; i < NUM_LEDS_FRONT-14; i++) leds_front[i] = CRGB::White; 
      // Back Light
      leds_back[0] = CRGB::Blue;
      leds_back[NUM_LEDS_BACK-1] = CRGB::Blue;
      
      update_lights();
      
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
  show_led = 1;
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

  show_led = 1;
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
      
    //show_leds();
    show_led = 1;
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
  //show_leds();  
  show_led = 1;
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
      
    //show_leds();
    show_led = 1;
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
  //show_leds();
  show_led = 1;
}

void set_horn(int on)
{
  horn_status = on;

  if (on) {  
    digitalWrite(PIN_HORN, HIGH);
    FastLED.setBrightness(200);
    blink_horn();
  } else {
    digitalWrite(PIN_HORN, LOW);  
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
    show_leds();
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

  
  Serial.print(button_light_reading);
  Serial.print(" - ");
  Serial.println(button_lights_state);
  
  if (button_light_reading  != button_lights_state) button_lights(button_light_reading);
  if (button_left_reading   != button_left_state) button_left(button_left_reading);
  if (button_right_reading  != button_right_state) button_right(button_right_reading);

  if (blink_left_status) blink_left();
  if (blink_right_status) blink_right();

   batt_counter++;

   if (show_led) show_leds(); 

  
  if (light_status)  {

    for (int i = 0; i<4; i++)
    {
      LowPower.powerDown(SLEEP_60MS, ADC_OFF, BOD_OFF);
      update_lights();
      show_leds(); 
    }
        
  } else {
    //delay(250);
    LowPower.powerDown(SLEEP_250MS, ADC_OFF, BOD_OFF);
  }
}
