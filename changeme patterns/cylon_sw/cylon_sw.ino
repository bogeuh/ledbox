/* Juggle
By: Originally by Mark Kriegsman
Modified By: Andrew Tuline
Date: February 2015
Juggle just moves some balls back and forth. A single ball could be a Cylon effect. I've added several variables to this simple routine.
*/

const int sensorPin = A0;
const int sensor2Pin = A1;
int sensorValue = 0; 
int sensor2Value = 0;

#include "FastLED.h"

#if FASTLED_VERSION < 3001000
#error "Requires FastLED 3.1 or later; check github for latest code."
#endif

// Fixed definitions cannot change on the fly.
#define LED_DT 5                                             // Serial data pin
#define LED_CK 11                                             // Clock pin for WS2801 or APA102
#define COLOR_ORDER GRB                                     // It's GRB for WS2812B and GBR for APA102
#define LED_TYPE WS2812B                                       // What kind of strip are you using (APA102, WS2801 or WS2812B)?
#define NUM_LEDS 120   



// Initialize changeable global variables.
uint8_t max_bright = 64;                                      // Overall brightness definition. It can be changed on the fly.

struct CRGB leds[NUM_LEDS];                                   // Initialize our LED array.

// Routine specific variables
uint8_t numdots = 4;                                          // Number of dots in use.
uint8_t faderate = 2;                                         // How long should the trails be. Very low value = longer trails.
uint8_t hueinc = 16;                                          // Incremental change in hue between each dot.
uint8_t thishue = 0;                                          // Starting hue.
uint8_t curhue = 0;                                           // The current hue
uint8_t thissat = 255;                                        // Saturation of the colour.
uint8_t thisbright = 255;                                     // How bright should the LED/display be.
uint8_t basebeat = 5;                                         // Higher = faster movement.


void setup() {
  delay(1000);                                                // Power-up safety delay or something like that.
  Serial.begin(115200);

  LEDS.addLeds<LED_TYPE, LED_DT, COLOR_ORDER>(leds, NUM_LEDS);    // Use this for WS2812B
  //LEDS.addLeds<LED_TYPE, LED_DT, LED_CK, COLOR_ORDER>(leds, NUM_LEDS);  // Use this for WS2801 or APA102

  FastLED.setBrightness(max_bright);
  set_max_power_in_volts_and_milliamps(5, 500);               // FastLED power management set at 5V, 500mA.
} // setup()

  
// void loop() {
//   ChangeMe();
//   juggle();
//   show_at_max_brightness_for_power();                         // Power managed display of LED's.
// } // loop()

void loop () {
  
  ChangeMe();
  sensor2Value = analogRead(sensor2Pin);
  uint8_t thisdelay = (sensor2Value / 25);
  Serial.print("delay: ");
  Serial.println(thisdelay);
  EVERY_N_MILLIS_I(thistimer, thisdelay) {                          // FastLED based non-blocking delay to update/display the sequence.
    juggle();
  }

  FastLED.show();
  thistimer.setPeriod(thisdelay);
} // loop()

void juggle() {                                               // Several colored dots, weaving in and out of sync with each other
  curhue = thishue;                                          // Reset the hue values.
  fadeToBlackBy(leds, NUM_LEDS, faderate);
  for( int i = 0; i < numdots; i++) {
    leds[beatsin16(basebeat+i+numdots,0,NUM_LEDS)] += CHSV(curhue, thissat, thisbright);   //beat16 is a FastLED 3.1 function
    curhue += hueinc;
  }
} // juggle()


void ChangeMe() {                                             // A time (rather than loop) based demo sequencer. This gives us full control over the length of each sequence.
                  //time changed to potentiometer readings
  sensorValue = analogRead(sensorPin);
  uint8_t secondHand = (sensorValue / 100);
  Serial.print("switch: ");
  Serial.println(secondHand);
  static uint8_t lastSecond = 99;                             // Static variable, means it's only defined once. This is our 'debounce' variable.
  if (lastSecond != secondHand) {                             // Debounce to make sure we're not repeating an assignment.
    lastSecond = secondHand;
    switch(secondHand) {
    case 0: numdots=1; faderate=2; break; // You can change values here, one at a time , or altogether.
    case 5: numdots=4; thishue=128; faderate=8; break;
    case 10: hueinc=48; thishue=random8(); break;
    }                               // Only gets called once, and not continuously for the next several seconds. Therefore, no rainbows.
  }
} // ChangeMe()

