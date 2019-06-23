/* button_demoReel100
 *
 * Originally By: Mark Kriegsman
 * 
 * Modified for EEPROM and button usage by: Andrew Tuline
 *
 * Date: January, 2017
 *
 * This takes Mark's DemoReel100 routine and adds the ability to change modes via button press.
 * It also allows the user to save the current mode into EEPROM and use that as the starting mode.
 *
 * Instructions:
 * 
 * Program reads display mode from EEPROM and displays it.
 * Click to change to the next mode.
 * Hold button for > 1 second to write the current mode to EEPROM.
 * Double click to reset to mode 0.
 * 
 * There's also debug output to the serial monitor . . just to be sure.
 * 
 * Requirements:
 * 
 * Jeff Saltzman's jsbutton.h routine is now included with this demo. Please ensure that it is 
 * downloaded alongside button_demoReel100.ino and in the same directory.
 * 
 * 
 */
//uses the INPUT_PULLUP by default
//just connect pin2 to button in and button out to ground 
//press button  = close = LOW
#define buttonPin 2                                           // input pin to use as a digital input
#include "jsbutton.h"                                         // Nice button routine by Jeff Saltzman

#include "EEPROM.h"
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

//analog read potentio value
const int sensorPin = A0;
int sensorValue = 0; 

// Definition for the array of routines to display.
#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

//Mode and EEPROM variables
uint8_t maxMode = 5;                                          // Maximum number of display modes. Would prefer to get this another way, but whatever.
int eepaddress = 0;

// Global variables can be changed on the fly.
uint8_t max_bright = 128;                                     // Overall brightness definition. It can be changed on the fly.

struct CRGB leds[NUM_LEDS];                                   // Initialize our LED array.

uint8_t gCurrentPatternNumber = 0;                            // Index number of which pattern is current
uint8_t gHue = 0;                                             // rotating "base color" used by many of the patterns

typedef void (*SimplePatternList[])();                        // List of patterns to cycle through.  Each is defined as a separate function below.



void setup() {

  Serial.begin(115200);                                        // Initialize serial port for debugging.
  delay(1000);                                                // Soft startup to ease the flow of electrons.

  pinMode(buttonPin, INPUT_PULLUP);                                  // Set button input pin
  //digitalWrite(buttonPin, HIGH );
  
  //LEDS.addLeds<LED_TYPE, LED_DT, LED_CK, COLOR_ORDER>(leds, NUM_LEDS);  // Use this for WS2801 or APA102
  LEDS.addLeds<LED_TYPE, LED_DT, COLOR_ORDER>(leds, NUM_LEDS);  // Use this for WS2812

  FastLED.setBrightness(max_bright);
  set_max_power_in_volts_and_milliamps(5, 2000);               // FastLED Power management set at 5V, 500mA.

  gCurrentPatternNumber = EEPROM.read(eepaddress);

  if (gCurrentPatternNumber > maxMode) gCurrentPatternNumber = 0;   // A safety in case the EEPROM has an illegal value.

  
  Serial.print("Starting ledMode: ");
  Serial.println(gCurrentPatternNumber);

} // setup()



SimplePatternList gPatterns = {rainbow, confetti_change, confetti_old, sinelon, juggle, bpm };               // Don't know why this has to be here. . .

 

void loop() {

  readbutton();
  
  EVERY_N_MILLISECONDS(50) {
    gPatterns[gCurrentPatternNumber]();                       // Call the current pattern function once, updating the 'leds' array    
  }

  EVERY_N_MILLISECONDS(20) {                                  // slowly cycle the "base color" through the rainbow
    gHue++;
  }

  FastLED.show();

} // loop()



void readbutton() {                                           // Read the button and increase the mode

  uint8_t b = checkButton();

  if (b == 1) {                                               // Just a click event to advance to next pattern
    gCurrentPatternNumber = (gCurrentPatternNumber + 1) % ARRAY_SIZE(gPatterns);
    Serial.println(gCurrentPatternNumber);
  }

  if (b == 2) {                                               // A double-click event to reset to 0 pattern
    gCurrentPatternNumber = 0;
    Serial.println(gCurrentPatternNumber);
  }

  if (b == 3) {                                               // A hold event to write current pattern to EEPROM
    EEPROM.write(eepaddress,gCurrentPatternNumber);
    Serial.print("Writing: ");
    Serial.println(gCurrentPatternNumber);    
  }

} // readbutton()




//--------------------[ Effects are below here ]------------------------------------------------------------------------------

void rainbow() {

  fill_rainbow(leds, NUM_LEDS, gHue, 7);                      // FastLED's built-in rainbow generator.
  
} // rainbow()


//replace rainbowglitter
void rainbowWithGlitter() {

  rainbow();                                                  // Built-in FastLED rainbow, plus some random sparkly glitter.
  addGlitter(80);
  
} // rainbowWithGlitter()



void addGlitter(fract8 chanceOfGlitter) {

  if(random8() < chanceOfGlitter) {
    leds[ random16(NUM_LEDS) ] += CRGB::White;
  }

} // addGlitter()



void confetti_old() {                                             // Random colored speckles that blink in and fade smoothly.

  fadeToBlackBy(leds, NUM_LEDS, 10);
  int pos = random16(NUM_LEDS);
  leds[pos] += CHSV(gHue + random8(64), 200, 255);
  
} // confetti()



void sinelon() {                                              // A colored dot sweeping back and forth, with fading trails.

  fadeToBlackBy(leds, NUM_LEDS, 20);
  int pos = beatsin16(13,0,NUM_LEDS-1);
  leds[pos] += CHSV(gHue, 255, 192);
  
} // sinelon()



void bpm() {                                                  // Colored stripes pulsing at a defined Beats-Per-Minute.

  uint8_t BeatsPerMinute = 62;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8(BeatsPerMinute, 64, 255);
  
  for(int i = 0; i < NUM_LEDS; i++) { //9948
    leds[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
  }
  
} // bpm()



void juggle() {                                               // Eight colored dots, weaving in and out of sync with each other.

  fadeToBlackBy(leds, NUM_LEDS, 20);
  byte dothue = 0;
  
  for(int i = 0; i < 8; i++) {
    leds[beatsin16(i+7,0,NUM_LEDS-1)] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
  
} // juggle()


//tryout patterns + pot controlled changeme
//confetti declares
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

void confetti_change () {
  ChangeMe(); 
  EVERY_N_MILLISECONDS(conf_thisdelay) { confetti(); }
  FastLED.show();  
}

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
  Serial.println(sensorValue);
  Serial.println(secondHand);
  static uint8_t lastSecond = 99;                             // Static variable, means it's only defined once. This is our 'debounce' variable.
  if (lastSecond != secondHand) {                             // Debounce to make sure we're not repeating an assignment.
    lastSecond = secondHand;
    switch(secondHand) {
      case  0: thisinc=1; thishue=random16(255); thissat=255; thisfade=2; huediff=40; break;  // You can change values here, one at a time , or altogether.
      case  2: thisinc=3; thishue=random16(255); thissat=255; thisfade=4; huediff=800; break;
      case  4: thisinc=5; thishue=random16(255); thissat=255; thisfade=8; huediff=120; break;
      case  6: thisinc=7; thishue=random16(255); thissat=255; thisfade=12; huediff=160; break;
      case  8: thisinc=9; thishue=random16(255); thissat=255; thisfade=16; huediff=200; break;
      case  10: thisinc=11; thishue=random16(255); thissat=255;thisfade=20; huediff=255; break;
   }
  }
} // ChangeMe()