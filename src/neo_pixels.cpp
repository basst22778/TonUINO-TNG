//Comment out if #define NeoPixels is used in constants.hpp
// #ifdef NeoPixels

#include "neo_pixels.hpp"

// #include "RGBConverterLib.h"

#include "constants.hpp"
#include "logger.hpp"
#include "tonuino.hpp"

Neo_pixels::Neo_pixels()
    : pixels(Adafruit_NeoPixel(NeoPixelCount, NeoPixelPin, NEO_GRB + NEO_KHZ800)) {}

void Neo_pixels::init()
{
  pixels.begin();
  pixels.show();
  pixels.setBrightness(NeoPixelBrightness);
  
  state = State::idle;
  currentState = State::none;
  updateFromSettings();

  animStartupOnInit();
}

void Neo_pixels::loop()
{
  if (isAutoUnblock && animStepStart + updatePause < millis())
  {
    isStateBlocking = false;
    isAutoUnblock = false;
    currentState = State::none;
  }
  // LOG(init_log, s_info, static_cast<uint8_t>(state));
  switch (state)
  {
  case State::idle:
    animIdle();
    break;
  case State::admin:
    animAdmin();
    break;
  case State::new_card:
    animNewCard();
    break;
  case State::play:
    animPlay();
    break;
  case State::pause:
    animPause();
    break;
  case State::next:
    animNextTrack();
    break;
  case State::previous:
    animPreviousTrack();
    break;
  case State::volume:
    animVolume();
    break;
  default:
    break;
  }
}

// ----------------------------------------------------------------------------
// Animation template
// Use to create new animations
//

// void Neo_pixels::animTemplate()
// {
//   if (currentState != state)
//   {
//     // setup animation
//     step = 1;
//     miniStep = 1;
//     updatePause = 200;
//     animStepStart = millis();
//     currentState = state;

//     // Neo Pixel setup
//     pixels.clear();
//     //...
//     pixels.show();
//   }

//   if (animStepStart + updatePause > millis())
//     return;

//   animStepStart = millis();

//   for (uint8_t i = 0; i < pixels.numPixels(); i++)
//   {
//     //update pixels for animation
//   }

//   pixels.show();

//   miniStep++;

//   if (miniStep == 4)
//   {
//     miniStep = 1;
//     step++;
//   }

//   if (step == 4)
//     step = 1;
// }

void Neo_pixels::animStartupOnInit()
{
  for (uint8_t i = 0; i < pixels.numPixels() / 2; i++)
  {
    pixels.setPixelColor(i, pixels.ColorHSV(hue, 255, baseValue));
    pixels.setPixelColor(pixels.numPixels() - 1 - i, pixels.ColorHSV(hue, 255, baseValue));
    pixels.show();
    delay(100);
  }
}

void Neo_pixels::animIdle()
{
  if (currentState != state)
  {
    currentState = state;
    pixels.fill(pixels.ColorHSV(hue, 255, baseValue), 0);
    pixels.show();
  }

}

void Neo_pixels::animAdmin()
{
  if (currentState != state)
  {
    displayAdmin();
  }
}

void Neo_pixels::animNewCard()
{
  if (currentState != state)
  {
    isStateBlocking = true;
    step = 1;
    miniStep = 1;
    updatePause = 50;
    animStepStart = millis();
    currentState = state;

    pixels.clear();
    for (uint8_t i = 0; i < pixels.numPixels(); i++)
    {
      pixels.setPixelColor(i, pixels.ColorHSV(hue, 255, baseValue));
    }
    pixels.show();
  }

  if (animStepStart + updatePause > millis())
    return;

  animStepStart = millis();

  for (uint8_t i = 0; i < pixels.numPixels(); i++)
  {
    pixels.setPixelColor(i, pixels.ColorHSV(hue, 255, baseValue));
    pixels.setPixelColor(pixels.numPixels() / 2 + step - 1, pixels.ColorHSV(hue, 255, 255));
    pixels.setPixelColor(pixels.numPixels() / 2 - step, pixels.ColorHSV(hue, 255, 255));
  }

  if (step > 1)
  {
    pixels.setPixelColor(pixels.numPixels() / 2 + step - 2, pixels.ColorHSV(hue, 255, 215));
    pixels.setPixelColor(pixels.numPixels() / 2 - step + 1, pixels.ColorHSV(hue, 255, 215));
  }
  if (step > 2)
  {
    pixels.setPixelColor(pixels.numPixels() / 2 + step - 3, pixels.ColorHSV(hue, 255, 175));
    pixels.setPixelColor(pixels.numPixels() / 2 - step + 2, pixels.ColorHSV(hue, 255, 215));
  }
  if (step > 3)
  {
    pixels.setPixelColor(pixels.numPixels() / 2 + step - 4, pixels.ColorHSV(hue, 255, 135));
    pixels.setPixelColor(pixels.numPixels() / 2 - step + 3, pixels.ColorHSV(hue, 255, 215));
  }

  pixels.show();

  step++;

  if (step > pixels.numPixels() + 4)
  {
    step = 1;
    isStateBlocking = false;
    state = State::play;
  }
}

void Neo_pixels::animPlay()
{
  if (currentState != state)
  {
    isPreparing = true;
    step = 1;
    miniStep = 1;
    updatePause = 100;
    animStepStart = millis();
    currentState = state;
  }

  if (animStepStart + updatePause > millis())
    return;

  animStepStart = millis();

  if (isPreparing)
  {
    isPreparing = false;
    for (uint8_t i = 0; i < pixels.numPixels(); i++)
    {
      uint8_t calcValue = valueFromColor(pixels.getPixelColor(i));
      uint8_t nextValue;
      if (calcValue < baseValue && calcValue + 16 < baseValue) 
      {
        nextValue = calcValue + 16;
        isPreparing = true;
      }
      else if (calcValue > baseValue && calcValue - 16 > baseValue) 
      {
        nextValue = calcValue - 16;
        isPreparing = true;
      }
      else
      nextValue = calcValue;

      pixels.setPixelColor(i, pixels.ColorHSV(hue, 255, nextValue));
    }
    pixels.show();

    if (isPreparing == true)
      return;
  }

  uint8_t dimUpValue;
  if (baseValue + (16 * miniStep) > 255)
    dimUpValue = 255;
  else
    dimUpValue = baseValue + (16 * miniStep);

  uint8_t dimDownValue;
  if (255 - (16 * (miniStep - 15)) < baseValue)
    dimDownValue = baseValue;
  else
    dimDownValue = 255 - (16 * (miniStep - 15));

  for (uint8_t i = 0; i < pixels.numPixels(); i++)
  {
    if ((i - step) % 4 == 0)
    {
      if (miniStep <= 15)
        pixels.setPixelColor(i, pixels.ColorHSV(hue, 255, dimUpValue));
      else
        pixels.setPixelColor(i, pixels.ColorHSV(hue, 255, dimDownValue));
    }
    else
      pixels.setPixelColor(i, pixels.ColorHSV(hue, 255, baseValue));
  }

  pixels.show();

  miniStep++;

  if (miniStep > 30)
  {
    miniStep = 1;
    step++;
  }

  if (step > 4)
    step = 1;
}

void Neo_pixels::animPause()
{
  if (currentState != state)
  {
    isPreparing = true;
    step = 1;
    miniStep = 1;
    updatePause = 100;
    animStepStart = millis();
    currentState = state;
  }

  if (animStepStart + updatePause > millis())
    return;

  animStepStart = millis();

  if (isPreparing)
  {
    isPreparing = false;
    for (uint8_t i = 0; i < pixels.numPixels(); i++)
    {
      uint8_t calcValue = valueFromColor(pixels.getPixelColor(i));
      uint8_t nextValue;
      if (calcValue - 16 < 16)
        nextValue = 16;
      else {
        nextValue = calcValue - 16;
        isPreparing = true;
      }
      pixels.setPixelColor(i, pixels.ColorHSV(hue, 255, nextValue));
    }

    pixels.show();

    if (isPreparing == true)
      return;
  }

  uint8_t dimUpValue;
  if (16 * miniStep > 255)
    dimUpValue = 255;
  else
    dimUpValue = 16 * miniStep;

  uint8_t dimDownValue;
  if (255 - (16 * (miniStep - 18)) < 16)
    dimDownValue = 16;
  else
    dimDownValue = 255 - (16 * (miniStep - 18));

  for (uint8_t i = 0; i < pixels.numPixels(); i++)
  {
    if (miniStep <= 18)
      pixels.setPixelColor(i, pixels.ColorHSV(hue, 255, dimUpValue));
    else
      pixels.setPixelColor(i, pixels.ColorHSV(hue, 255, dimDownValue));
  }

  pixels.show();

  miniStep++;

  if (miniStep > 36)
    miniStep = 1;
}

void Neo_pixels::animNextTrack()
{
  if (currentState != state)
  {
    step = 1;
    miniStep = 1;
    updatePause = 25;
    animStepStart = millis();
    currentState = state;

    pixels.clear();
    for (uint8_t i = 0; i < pixels.numPixels(); i++)
    {
      pixels.setPixelColor(i, pixels.ColorHSV(hue, 255, baseValue));
    }
    pixels.show();
  }

  if (animStepStart + updatePause > millis())
    return;

  animStepStart = millis();

  for (uint8_t i = 0; i < pixels.numPixels(); i++)
  {
    pixels.setPixelColor(i, pixels.ColorHSV(hue, 255, baseValue));
    pixels.setPixelColor(step - 1, pixels.ColorHSV(hue, 255, 255));
  }

  if (step > 1)
    pixels.setPixelColor(step - 2, pixels.ColorHSV(hue, 255, 215));
  if (step > 2)
    pixels.setPixelColor(step - 3, pixels.ColorHSV(hue, 255, 175));
  if (step > 3)
    pixels.setPixelColor(step - 4, pixels.ColorHSV(hue, 255, 135));

  pixels.show();

  step++;

  if (step > pixels.numPixels() + 4)
    state = State::play;
}

void Neo_pixels::animPreviousTrack()
{
  if (currentState != state)
  {
    step = 1;
    miniStep = 1;
    updatePause = 25;
    animStepStart = millis();
    currentState = state;

    pixels.clear();
    for (uint8_t i = 0; i < pixels.numPixels(); i++)
    {
      pixels.setPixelColor(i, pixels.ColorHSV(hue, 255, baseValue));
    }
    pixels.show();
  }

  if (animStepStart + updatePause > millis())
    return;

  animStepStart = millis();

  for (uint8_t i = 0; i < pixels.numPixels(); i++)
  {
    pixels.setPixelColor(i, pixels.ColorHSV(hue, 255, baseValue));
    pixels.setPixelColor(pixels.numPixels() - step, pixels.ColorHSV(hue, 255, 255));
  }

  if (step > 1)
    pixels.setPixelColor(pixels.numPixels() - step + 1, pixels.ColorHSV(hue, 255, 215));
  if (step > 2)
    pixels.setPixelColor(pixels.numPixels() - step + 2, pixels.ColorHSV(hue, 255, 175));
  if (step > 3)
    pixels.setPixelColor(pixels.numPixels() - step + 3, pixels.ColorHSV(hue, 255, 135));

  pixels.show();

  step++;

  if (step > pixels.numPixels() + 4)
    state = State::play;
}

void Neo_pixels::animVolume()
{
  if (currentState != state)
  {
    step = 0;
    miniStep = 0;
    updatePause = 2000;
    animStepStart = millis();
    currentState = state;
  }

  pixels.clear();

  uint8_t onPixels = map(Tonuino::getTonuino().getMp3().getVolume(), Tonuino::getTonuino().getSettings().minVolume, Tonuino::getTonuino().getSettings().maxVolume, 1, pixels.numPixels());

  for (uint8_t i = 0; i < onPixels; i++)
  {
    uint16_t volumeHue = map(i, 0, pixels.numPixels(), 65535 / 6, -1000);
    pixels.setPixelColor(i, pixels.ColorHSV(volumeHue, 255, 255));
  }
  pixels.show();

  if (animStepStart + updatePause > millis())
    return;

  animStepStart = millis();

  state = State::play;
}

// ----------------------------------------------------------------------------
// Public functions
//

void Neo_pixels::updateFromSettings()
{
  hue = Tonuino::getTonuino().getSettings().neoPixelHue;
  baseValue = Tonuino::getTonuino().getSettings().neoPixelBaseValue;
  nightLightValue = Tonuino::getTonuino().getSettings().neoPixelNightLightValue;
}

void Neo_pixels::displayAll(uint16_t hue, uint8_t baseValue)
{
  pixels.fill(pixels.ColorHSV(hue, 255, baseValue), 0);
  pixels.show();
}

void Neo_pixels::displayAdmin()
{
  currentState = state;
  uint64_t adminHue;
  if (65536 / 6 + hue <= 65536)
    adminHue = 65536 / 6 + hue;
  else
    adminHue = 65536 / 6 + hue - 65536;

  for (uint8_t i = 0; i < pixels.numPixels(); i++)
  {

    if (i % 2 == 0)
      pixels.setPixelColor(i, pixels.ColorHSV(adminHue, 255, 255));
    else
      pixels.setPixelColor(i, pixels.ColorHSV(hue, 255, 255));
  }
  pixels.show();
}

void Neo_pixels::displayNightLight()
{
  isStateBlocking = true;
  isAutoUnblock = true;
  updatePause = 500;
  animStepStart = millis();

  for (uint8_t i = 0; i < pixels.numPixels(); i++)
  {
    pixels.setPixelColor(i, pixels.ColorHSV(5000, 255, nightLightValue));
  }
  pixels.show();
}

// ----------------------------------------------------------------------------
// Helper functions
// Used in some animations
//

uint8_t Neo_pixels::valueFromColor(uint32_t color)
{
  uint8_t r = (uint8_t)(color >> 16);
  uint8_t g = (uint8_t)(color >> 8);
  uint8_t b = (uint8_t)(color >> 0);

  uint8_t s = stepMath(b, g);
  uint8_t px = mix(b, g, s);
  s = stepMath(px, r);
  uint8_t qx = mix(px, r, s);
  uint8_t value = qx;

  // uint8_t s = stepMath(b, g);
  // uint8_t px = mix(b, g, s);
  // uint8_t py = mix(g, b, s);
  // uint8_t pz = mix(-1.0, 0.0, s);
  // uint8_t pw = mix(0.6666666, -0.3333333, s);
  // s = stepMath(px, r);
  // uint8_t qx = mix(px, r, s);
  // uint8_t qz = mix(pw, pz, s);
  // uint8_t qw = mix(r, px, s);
  // uint8_t d = qx - min(qw, py);
  // hsv[0] = abs(qz + (qw - py) / (6.0 * d + 1e-10));
  // hsv[1] = d / (qx + 1e-10);
  // hsv[2] = qx;

  return value;
}

//Comment out if #define NeoPixels is used in constants.hpp
// #endif