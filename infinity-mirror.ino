/// @file    infinity-mirror.ino
/// @brief   firmware for an interactive infinity mirror, using FastLED and a WS2811 LED strip

#include <FastLED.h>
#include <RotaryEncoder.h>

#define DATA_PIN 9
#define RED_POT_PIN A0
#define GREEN_POT_PIN A1
#define BLUE_POT_PIN A2
#define RED_BUTTON_PIN 7
#define GREEN_BUTTON_PIN 3
#define BLUE_BUTTON_PIN 8

#define ENCODER_BUTTON_INPUT 13
#define ENCODER_A 10
#define ENCODER_B 11
#define ROTARYMIN 0
#define ROTARYMAX 6 // NUMBER OF PATTERNS - 1 * 2

#define LED_TYPE WS2811
#define COLOR_ORDER GRB
#define NUM_LEDS 15
CRGB leds[NUM_LEDS];

#define BRIGHTNESS 96
#define FRAMES_PER_SECOND 120

RotaryEncoder encoder(ENCODER_A, ENCODER_B, RotaryEncoder::LatchMode::TWO03);

void setup()
{
  Serial.begin(9600);
  delay(3000);

  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

  FastLED.setBrightness(BRIGHTNESS);
}

// List of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*SimplePatternList[])();
SimplePatternList gPatterns = {
    rotation,
    rainbow,
    singleDot,
    rgb};

int8_t currentPattern = 0;
uint8_t gHue = 0;

void loop()
{
  gPatterns[currentPattern]();

  FastLED.show();
  FastLED.delay(1000 / FRAMES_PER_SECOND);

  readEncoder();

  if (readButtonInput(ENCODER_BUTTON_INPUT))
  {
    nextPattern();
  }

  // do some periodic updates
  EVERY_N_MILLISECONDS(20) { gHue++; }
}

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

void nextPattern()
{
  currentPattern = (currentPattern + 1) % ARRAY_SIZE(gPatterns);
}

void singleDot()
{
  int8_t activeLED = floor(analogRead(RED_POT_PIN) / floor(1024 / (NUM_LEDS - 1)));
  int8_t hue = floor(analogRead(GREEN_POT_PIN) / (1024 / 255));

  fadeToBlackBy(leds, NUM_LEDS, 20);
  leds[activeLED] += CHSV(hue, 255, 192);
}

void rainbow()
{
  int8_t hue = floor(analogRead(RED_POT_PIN) / (1024 / 255));
  int8_t numLeds = floor(analogRead(GREEN_POT_PIN) / (1024 / NUM_LEDS));

  fill_rainbow(leds, NUM_LEDS, hue, numLeds);
}

void rgb()
{
  int8_t red = floor(analogRead(RED_POT_PIN) / (1024 / 255));
  int8_t green = floor(analogRead(GREEN_POT_PIN) / (1024 / 255));
  int8_t blue = floor(analogRead(BLUE_POT_PIN) / (1024 / 255));

  if (readButtonInput(RED_BUTTON_PIN))
  {
    red = 255;
  }
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

void rotation()
{
  int16_t bpm = floor((analogRead(RED_POT_PIN) / 1024.0) * 255);
  int16_t hue = floor((analogRead(GREEN_POT_PIN) / 1024.0) * 255);
  int16_t fadeTime = floor((analogRead(BLUE_POT_PIN) / 1024.0) * 255);

  int16_t fadeTimeScaled = max(floor(fadeTime / 7) - 3, 0);

  fadeToBlackBy(leds, NUM_LEDS, fadeTimeScaled);
  int16_t pos = beat8(bpm) / (NUM_LEDS + 2);

  leds[pos] += CHSV(hue, 255, 192);
}

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

void readEncoder()
{
  encoder.tick();

  int newPos = encoder.getPosition();

  if (newPos < ROTARYMIN)
  {
    encoder.setPosition(ROTARYMAX);
    newPos = ROTARYMAX;
  }
  else if (newPos > ROTARYMAX)
  {
    encoder.setPosition(ROTARYMIN);
    newPos = ROTARYMIN;
  }

  int8_t newPosReduced = floor(newPos / 2);

  currentPattern = (newPosReduced);
}