/// @file    infinity-mirror.ino
/// @brief   firmware for an interactive infinity mirror, using FastLED and a WS2811 LED strip

#include <FastLED.h>

#define DATA_PIN    9
#define RED_POT_PIN A2
#define GREEN_POT_PIN A6
#define BLUE_POT_PIN A3
#define RED_BUTTON_PIN 7
#define GREEN_BUTTON_PIN 3
#define BLUE_BUTTON_PIN 8

#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
#define NUM_LEDS    15
CRGB leds[NUM_LEDS];

#define BRIGHTNESS          96
#define FRAMES_PER_SECOND  120

void setup() {
  delay(3000);
  
  FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

  FastLED.setBrightness(BRIGHTNESS);
}


// List of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*SimplePatternList[])();
SimplePatternList gPatterns = { rgb, rainbow, singleDot};

uint8_t gCurrentPatternNumber = 0;
uint8_t gHue = 0;
  
void loop()
{
  gPatterns[gCurrentPatternNumber]();

  FastLED.show();  
  FastLED.delay(1000/FRAMES_PER_SECOND); 

  if (readButtonInput(RED_BUTTON_PIN))
  {
    nextPattern();
  }

  // do some periodic updates
  EVERY_N_MILLISECONDS( 20 ) { gHue++; } // slowly cycle the "base color" through the rainbow
}

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

void nextPattern()
{
  gCurrentPatternNumber = (gCurrentPatternNumber + 1) % ARRAY_SIZE( gPatterns);
}

void singleDot()
{
    int8_t activeLED = floor(analogRead(RED_POT_PIN) / floor(1024 / (NUM_LEDS - 1)));
    int8_t hue = floor(analogRead(GREEN_POT_PIN) / (1024 / 255));

    fadeToBlackBy( leds, NUM_LEDS, 20);
    leds[activeLED] += CHSV( hue, 255, 192);
}

void rainbow() 
{
  int8_t hue = floor(analogRead(RED_POT_PIN) / (1024 / 255));
  int8_t numLeds = floor(analogRead(GREEN_POT_PIN) / (1024 / NUM_LEDS));

  fill_rainbow(leds, NUM_LEDS, hue, numLeds);
}

void rgb()
{
  fadeToBlackBy( leds, NUM_LEDS, 20);

  int8_t red = floor(analogRead(RED_POT_PIN) / (1024 / 255));
  int8_t green = floor(analogRead(GREEN_POT_PIN) / (1024 / 255));
  int8_t blue = floor(analogRead(BLUE_POT_PIN) / (1024 / 255));

  // UNCOMMENT AFTER ADDING ENCODER MODE CHANGE
  // if (readButtonInput(RED_BUTTON_PIN))
  // {
  //   red = 255;
  // }
  if (readButtonInput(GREEN_BUTTON_PIN))
  {
    green = 255;
  }
  if (readButtonInput(BLUE_BUTTON_PIN))
  {
    blue = 255;
  }

  fill_solid(leds, NUM_LEDS, CRGB(red, green, blue));
}

// void rainbowWithGlitter() 
// {
//   rainbow();
//   addGlitter(80);
// }

// void addGlitter( fract8 chanceOfGlitter) 
// {
//   if( random8() < chanceOfGlitter) {
//     leds[ random16(NUM_LEDS) ] += CRGB::White;
//   }
// }

// void confetti() 
// {
//   // random colored speckles that blink in and fade smoothly
//   fadeToBlackBy( leds, NUM_LEDS, 10);
//   int pos = random16(NUM_LEDS);
//   leds[pos] += CHSV( gHue + random8(64), 200, 255);
// }

// void sinelon()
// {
//   // a colored dot sweeping back and forth, with fading trails
//   fadeToBlackBy( leds, NUM_LEDS, 20);
//   int pos = beatsin16( 13, 0, NUM_LEDS-1 );
//   leds[pos] += CHSV( gHue, 255, 192);
// }

// void bpm()
// {
//   // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
//   uint8_t BeatsPerMinute = 62;
//   CRGBPalette16 palette = PartyColors_p;
//   uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
//   for( int i = 0; i < NUM_LEDS; i++) { //9948
//     leds[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
//   }
// }

// void juggle() {
//   // eight colored dots, weaving in and out of sync with each other
//   fadeToBlackBy( leds, NUM_LEDS, 20);
//   uint8_t dothue = 0;
//   for( int i = 0; i < 8; i++) {
//     leds[beatsin16( i+7, 0, NUM_LEDS-1 )] |= CHSV(dothue, 200, 255);
//     dothue += 32;
//   }
// }

// boolean checkTrigger(int index)
// {
//   boolean newState = digitalRead(inputs[index]);

//   if ((newState == HIGH) && (triggerStates[index] == LOW))
//   {
//     delay(3);
//     newState = digitalRead(inputs[index]);

//     if (newState == HIGH)
//     {
//       triggerStates[index] = newState;
//       return true;
//     }
//   }
//   triggerStates[index] = newState;
//   return false;
// }

boolean readButtonInput(int pin)
{
  if (digitalRead(pin) == HIGH)
  {
    delay(3);
    if (digitalRead(pin) == HIGH)
    {
      return true;
    }
  }

  return false;
}