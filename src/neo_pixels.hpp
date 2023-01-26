#ifdef NeoPixels

#ifndef SRC_NEO_PIXELS_HPP_
#define SRC_NEO_PIXELS_HPP_

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

#include "constants.hpp"
#include "logger.hpp"

class Neo_pixels
{
public:
  Neo_pixels();

  enum class State : uint8_t
  {
    none,
    idle,
    admin,
    new_card,
    play,
    pause,
    next,
    previous,
    volume,
    night_light
  };

  void init();
  void loop();

  State &getState() { return state; }
  void setState(State state)
  {
    if (!isStateBlocking)
      this->state = state;
  }
  void resetUpdate() { animStepStart = millis(); }
  void updateFromSettings();
  void displayAll(uint16_t hue, uint8_t baseValue);
  void displayAdmin();
  void displayNightLight();

private:
  Adafruit_NeoPixel pixels;
  State state;
  State currentState;
  uint16_t hue;
  uint8_t baseValue;
  uint8_t nightLightValue;
  uint8_t step;
  uint8_t miniStep;
  unsigned long updatePause;
  unsigned long animStepStart;
  bool isPreparing;
  bool isStateBlocking;
  bool isAutoUnblock;

  void animStartup();
  void animStartupOnInit();
  void animIdle();
  void animNewCard();
  void animAdmin();
  void animPlay();
  void animPause();
  void animNextTrack();
  void animPreviousTrack();
  void animVolume();

  uint8_t valueFromColor(uint32_t color);
  float mix(float a, float b, float t) { return a + (b - a) * t; }
  float stepMath(float e, float x) { return x < e ? 0.0 : 1.0; }
};

#endif /* SRC_NEO_PIXELS_HPP_ */
#endif