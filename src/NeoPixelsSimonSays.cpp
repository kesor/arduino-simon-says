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
int simonTurnNum = 0;
int simonOrder[simonMaxSize] = { -1 };
int simonTurnPause = 500;

// function declarations
void SetRandomSeed();
void flash(int times, RgbColor flash_color, RgbColor background_color);
void updateSimonSequence();
void displaySimonSequence();
void inputSimonSequence();
void simonIsAWinner();
void simonIsALoser();

// Instantiate a Keypad and a NeoPixel pixels
NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> pixels(pixelCount, pixelPin);
Keypad customKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, rowCount, columnCount);

void setup() {
  // clear pixels into black
  pixels.Begin();
  pixels.Show();
  flash(1, white, black);

  // make random numbers more random
  SetRandomSeed();
}

void loop() {
  updateSimonSequence();
  displaySimonSequence();
  inputSimonSequence();
}

void updateSimonSequence() {
  if (simonTurnNum < simonMaxSize-1) {
    // add a new value as latest flashing
    simonOrder[simonTurnNum] = random(16);
    simonTurnNum++;
    // set the "no more flashing" marker (-1)
    simonOrder[simonTurnNum] = -1;
  }
  if (simonTurnNum == simonMaxSize) {
    simonIsAWinner();
  }
}

void displaySimonSequence() {
  delay(simonTurnPause);
  for (int flashing=0; flashing<simonMaxSize; flashing++) {
    if (simonOrder[flashing] == -1) {
      break;
    }
    pixels.SetPixelColor(simonOrder[flashing], blue);
    pixels.Show();
    delay(simonTurnPause);
    pixels.SetPixelColor(simonOrder[flashing], black);
    pixels.Show();
    delay(flashPause);
  }
  // blank the pixels
  pixels.Begin();
  pixels.Show();
}

void inputSimonSequence() {
  for (int input=0; input<simonMaxSize; input++) {
    // finishing the sequence is a won turn
    if (input > 0 && simonOrder[input] == -1) {
      delay(simonTurnPause);
      flash(1, green, black);
      break;
    }
    char c = customKeypad.waitForKey();
    int pixel = strtol(&c, NULL, 16);
    if (pixel == simonOrder[input]) {
      pixels.SetPixelColor(pixel, blue);
      pixels.Show();
      delay(flashPause);
      pixels.SetPixelColor(pixel, black);
      pixels.Show();
    } else {
      simonIsALoser();
      break;
    }
  }
}

void simonIsAWinner() {
  flash(4, green, black);
  simonOrder[0] = -1;
  simonTurnNum = 0;
  delay(flashPause*4);
}

void simonIsALoser() {
  flash(2, red, black);
  simonOrder[0] = -1;
  simonTurnNum = 0;
  delay(simonTurnPause);
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
