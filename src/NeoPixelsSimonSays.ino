#include <stdlib.h>
#include <Keypad.h>
#include <NeoPixelBus.h>

#define colorSaturation 16

const byte rowCount = 4; //four rowCount
const byte columnCount = 4; //four columns
byte rowPins[rowCount] = {7, 6, 5, 4}; //connect to the row pinouts of the keypad
byte colPins[columnCount] = {11, 10, 9, 8}; //connect to the column pinouts of the keypad
const uint8_t PixelPin = 2;
const uint16_t PixelCount = 16;
char hexaKeys[rowCount][columnCount] = {
  {'0','4','8','C'},
  {'1','5','9','D'},
  {'2','6','A','E'},
  {'3','7','B','F'}
};

NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> strip(PixelCount, PixelPin);
Keypad customKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, rowCount, columnCount);

RgbColor red   ( colorSaturation, 0,               0 );
RgbColor green ( 0, colorSaturation,               0 );
RgbColor blue  ( 0,               0, colorSaturation );
RgbColor white ( colorSaturation );
RgbColor black ( 0 );

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

void setup() {
  strip.Begin();
  strip.Show();

  SetRandomSeed();
}

void flash(int times, RgbColor flash_color, RgbColor background_color, int pause=250) {
  for (int iteration=0; iteration<times; iteration++) {
    for (int pixel=0; pixel<PixelCount; pixel++) {
      strip.SetPixelColor(pixel, flash_color);
    }
    strip.Show();
    delay(pause);
    for (int pixel=0; pixel<PixelCount; pixel++) {
      strip.SetPixelColor(pixel, background_color);
    }
    strip.Show();
    if (iteration<times) {
      delay(pause);
    }
  }
}

void loop() {
  char c = customKeypad.waitForKey();
  int pixel = strtol(&c, NULL, 16);
  flash(2, green, black);
  strip.SetPixelColor(pixel, red);
  strip.Show();
  strip.SetPixelColor(pixel, black);
}
