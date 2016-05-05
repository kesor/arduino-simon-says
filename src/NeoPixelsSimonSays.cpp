#include <stdlib.h>
#include <Keypad.h>
#include <NeoPixelBus.h>
#include <avr/pgmspace.h>

// Keypad configuration
const byte rowCount = 4;
const byte columnCount = 4;
byte rowPins[rowCount] = { 7, 6, 5, 4 };
byte colPins[columnCount] = { 11, 10, 9, 8 };
char hexaKeys[rowCount][columnCount] = {
  {'0','4','8','C'},
  {'1','5','9','D'},
  {'2','6','A','E'},
  {'3','7','B','F'}
};

// NeoPixels configuration
#define colorSaturation 16
const uint8_t pixelPin = 2;
const uint16_t pixelCount = 16;
const uint8_t flashPause = 250;

// named preset colors
RgbColor red   ( colorSaturation, 0,               0 );
RgbColor green ( 0, colorSaturation,               0 );
RgbColor blue  ( 0,               0, colorSaturation );
RgbColor white ( colorSaturation );
RgbColor black ( 0 );

// Simon Says memory buffer
const int simonMaxSize = 100;
int sumonTurnNum = 1;
int simonOrder[simonMaxSize] = { -1 };

// function declarations
void SetRandomSeed();
void flash(int times, RgbColor flash_color, RgbColor background_color);

// Instantiate a Keypad and a NeoPixel pixels
NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> pixels(pixelCount, pixelPin);
Keypad customKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, rowCount, columnCount);

void setup() {
  // clear pixels into black
  pixels.Begin();
  pixels.Show();

  // make random numbers more random
  SetRandomSeed();
}

void loop() {
  char c = customKeypad.waitForKey();
  int pixel = strtol(&c, NULL, 16);
  flash(2, green, black);
  pixels.SetPixelColor(pixel, red);
  pixels.Show();
  pixels.SetPixelColor(pixel, black);
}

void SetRandomSeed() {
  uint32_t seed;
  seed = analogRead(0);
  delay(1);
  for (int shifts = 3; shifts < 31; shifts += 3) {
    seed ^= analogRead(0) << shifts;
    delay(1);
  }
  randomSeed(seed);
}

void flash(int times, RgbColor flash_color, RgbColor background_color) {
  for (int iteration=0; iteration<times; iteration++) {
    for (uint pixel=0; pixel<pixelCount; pixel++) {
      pixels.SetPixelColor(pixel, flash_color);
    }
    pixels.Show();
    delay(flashPause);
    for (uint pixel=0; pixel<pixelCount; pixel++) {
      pixels.SetPixelColor(pixel, background_color);
    }
    pixels.Show();
    if (iteration<times) {
      delay(flashPause);
    }
  }
}
