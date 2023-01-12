#include "common.h"
#include "displayData.h"

uint16_t XY(uint8_t x, uint8_t y)
{
  uint16_t i;
  uint8_t h = y;

  if (kMatrixTopToBottom == false)
  {
    h = kMatrixHeight - 1 - y;
  }

  if (kMatrixSerpentineLayout == false)
  {
    i = (h * kMatrixWidth) + x;
  }

  if (kMatrixSerpentineLayout == true)
  {
    if (y & 0x01)
    {
      // Even rows run backwards
      i = (h * kMatrixWidth) + x;
    }
    else
    {
      // Odd rows run forwards
      uint8_t reverseX = (kMatrixWidth - 1) - x;
      i = (h * kMatrixWidth) + reverseX;
    }
  }

  return i;
}

int16_t XYsafe(uint8_t x, uint8_t y)
{
  if (x >= kMatrixWidth)
    return -1;
  if (y >= kMatrixHeight)
    return -1;
  return XY(x, y);
}

void AllFade()
{
  //  fadeToBlackBy(leds, NUM_LEDS, FADE_RATE);
  for (uint16_t l = 0; l < NUM_LEDS; l++)
  {
    leds[l].r = scale8(leds[l].r, 255 - FADE_RATE);
    leds[l].g = scale8(leds[l].g, 255 - FADE_RATE);
    leds[l].b = scale8(leds[l].b, 255 - FADE_RATE);
  }
}

void AllFall()
{
  static byte c = 0;

  if (c++ > 2)
  {
    c = 0;

    for (int8_t x = 0; x < kMatrixWidth; x++)
    {
      for (int8_t y = kMatrixHeight - 1; y >= 0; y--)
      {
        if (y == 0)
        {
          leds[XYsafe(x, y)] = CRGB::Black;
        }
        else
        {
          leds[XYsafe(x, y)] = leds[XYsafe(x, y - 1)];
        }
      }
    }
  }
}

void AllBlack()
{
  fill_solid(leds, NUM_LEDS, CRGB::Black);
}

void ScrollFrame()
{
  for (int16_t x = 0; x < kMatrixWidth - 1; x++)
  {
    for (int16_t y = 0; y < kMatrixHeight; y++)
    {
      leds[XY(x, y)] = leds[XY(x + 1, y)];
    }
  }
  for (int16_t y = 0; y < kMatrixHeight; y++)
  {
    leds[XY(kMatrixWidth - 1, y)] = CRGB::Black;
  }
}