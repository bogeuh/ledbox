//uint8_t secondHand = (millis() / 1000) % 15;
const int sensorPin = A0;
const int sensor2Pin = A1;
int sensorValue = 0; 
int sensor2Value = 0;
//fastled stuff
#include "FastLED.h"                                          // FastLED library.

#if FASTLED_VERSION < 3001000
#error "Requires FastLED 3.1 or later; check github for latest code."
#endif
 
// Fixed definitions cannot change on the fly.
#define LED_DT 5                                             // Serial data pin
#define LED_CK 11                                             // Clock pin for WS2801 or APA102
#define COLOR_ORDER GRB                                     // It's GRB for WS2812B and GBR for APA102
#define LED_TYPE WS2812B                                       // What kind of strip are you using (APA102, WS2801 or WS2812B)?
#define NUM_LEDS 120                                           // Number of LED's

// Initialize changeable global variables.
uint8_t max_bright = 255;                                     // Overall brightness definition. It can be changed on the fly.

struct CRGB leds[NUM_LEDS];  // Initialize our LED array.
//end fastledstuff

//confetti stuff
unsigned long previousMillis;                                 // Store last time the strip was updated.
// Define variables used by the sequences.
uint8_t  thisfade = 8;                                        // How quickly does it fade? Lower = slower fade rate.
int       thishue = 50;                                       // Starting hue.
uint8_t   thisinc = 1;                                        // Incremental value for rotating hues
uint8_t   thissat = 100;                                      // The saturation, where 255 = brilliant colours.
uint8_t   thisbri = 255;                                      // Brightness of a sequence. Remember, max_bright is the overall limiter.
int       huediff = 256;                                      // Range of random #'s to use for hue
uint8_t conf_thisdelay = 5; 
//confetti end

void setup() {
  Serial.begin(115200);
  LEDS.addLeds<LED_TYPE, LED_DT, COLOR_ORDER>(leds, NUM_LEDS);         // For WS2812B
  //LEDS.addLeds<LED_TYPE, LED_DT, LED_CK, COLOR_ORDER>(leds, NUM_LEDS);   // For APA102 or WS2801
  FastLED.setBrightness(max_bright);
  set_max_power_in_volts_and_milliamps(5, 1000);              // FastLED 2.1 Power management set at 5V, 500mA
} // setup()

void loop () {
  
  ChangeMe();
  sensor2Value = analogRead(sensor2Pin);
  uint8_t thisdelay = (sensor2Value / 25);
  Serial.print("delay: ");
  Serial.println(thisdelay);
  EVERY_N_MILLIS_I(thistimer, thisdelay) {                          // FastLED based non-blocking delay to update/display the sequence.
    confetti();
  }

  FastLED.show();
  thistimer.setPeriod(thisdelay);
} // loop()

void confetti() {                                             // random colored speckles that blink in and fade smoothly
  fadeToBlackBy(leds, NUM_LEDS, thisfade);                    // Low values = slower fade.
  int pos = random16(NUM_LEDS);                               // Pick an LED at random.
  leds[pos] += CHSV((thishue + random16(huediff))/4 , thissat, thisbri);  // I use 12 bits for hue so that the hue increment isn't too quick.
  thishue = thishue + thisinc;                                // It increments here.
} // confetti()

//confetti config
void ChangeMe() {                                             // A time (rather than loop) based demo sequencer. This gives us full control over the length of each sequence.
                  // IMPORTANT!!! Change '15' to a different value to change duration of the loop.
                  //time changed to potentiometer readings
  sensorValue = analogRead(sensorPin);
  uint8_t secondHand = (sensorValue / 100);
  Serial.print("switch: ");
  Serial.println(secondHand);
  static uint8_t lastSecond = 99;                             // Static variable, means it's only defined once. This is our 'debounce' variable.
  if (lastSecond != secondHand) {                             // Debounce to make sure we're not repeating an assignment.
    lastSecond = secondHand;
    switch(secondHand) {
      case  0: thisinc=1; thishue=random16(255); thissat=255; thisfade=1; huediff=50; break;  // You can change values here, one at a time , or altogether.
      case  2: thisinc=3; thishue=random16(255); thissat=255; thisfade=3; huediff=100; break;
      case  4: thisinc=5; thishue=random16(255); thissat=255; thisfade=5; huediff=150; break;
      case  6: thisinc=7; thishue=random16(255); thissat=255; thisfade=7; huediff=200; break;
      case  8: thisinc=9; thishue=random16(255); thissat=255; thisfade=9; huediff=255; break;
      case  10: thisinc=11; thishue=random16(255); thissat=255;thisfade=11; huediff=64; break;
   }
  }
} // ChangeMe()
