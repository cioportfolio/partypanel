#include "common.h"
#include "displayData.h"

uint8_t hue=random8();
uint16_t EPM = 120;

void sweep(boolean beat)
{
  //ms per beat = 60*1000/BPM
  //ms per update = 1000/UPDATES_PER_SECOND
  //updates per beat = (60*1000/BPM)/(1000/UPDATES_PER_SECOND)
  //da = 65536/((60*1000/BPM)/(1000/UPDATES_PER_SECOND))
  //   = 65536/(60*1000/BPM/1000*UPS)
  //   = 65536/60*BPM/UPS

  const uint16_t da = (long)65536 * EPM / 120 / UPDATES_PER_SECOND;
  static uint16_t a = 0;
  static int8_t t = 0, r = 0, dir = 1;
  uint16_t x, y;

  x = (long)cos16(a) * r / 256;
  y = (long)sin16(a) * r / 256;
  CRGB col = CHSV(hue, 255, 128);
  if (beat)
  {
    hue += 160;
    r = LAST_COL;
    dir = -dir;
    col = CHSV(hue, 127, 255);
  }
  a += da * dir;
  DrawLineOld16(MID_COL * 256 - x, MID_ROW * 256 - y, MID_COL * 256 + x, MID_ROW * 256 + y, col);
  Refresh();
  AllFade();
  if (t++ > 10)
  {
    t = 0;
    if (r > 0)
      r--;
  }
}

void mixup(boolean beat)
{
  AllFall();
  if (beat)
  {
    DrawLine16(random8(0, LAST_COL) << 8, 0, random8(0, LAST_COL) << 8, random8(0, LAST_ROW) << 7, 400, CHSV(random8(), 255, 200));
  }
  Refresh();
}


uint16_t variant(uint16_t val)
{
  uint8_t r = random8(4);
  if (r = 4)
    return val << 1;
  return val >> r;
}

/* void weave(byte beat)
  {
  static uint8_t beatx, beaty;
  static uint8_t x = 0;

  if (x==0) {
    beatx = variant();
    beaty = variant();
  }
  if (x++ > 500) x=0;
  if (beat) hue+=32;
  UpdatePixelRaw(beatsin8(beatx, 0, LAST_COL), beatsin8(beaty, 0, LAST_ROW), CHSV(hue, 255, 255), 255);
  Refresh();
  AllFade();
  } */

void weave16(boolean beat)
{
  //ms per beat = 60*1000/BPM
  //ms per update = 1000/UPDATES_PER_SECOND
  //updates per beat = (60*1000/BPM)/(1000/UPDATES_PER_SECOND)

  static uint16_t ax, ay, sx, sy, x1, y1, s = 0;
  uint16_t x2, y2;
  CRGB col = CHSV(hue,255,128);
  uint16_t lw = 384;

  if (s == 0)
  {
    ax = random16();
    ay = random16();
    sx = random16(4 << 8, 16 << 8);
    sy = (long)65536 * EPM / 120 / UPDATES_PER_SECOND;
    x1 = (long)cos16(ax) * LAST_COL / 255 + MID_COL * 255;
    y1 = (long)sin16(ay) * LAST_ROW / 255 + MID_ROW * 255;

//            Serial.println("Angles (" + String(ax)+","+String(ay)+")");
//        Serial.println("Sin (" + String(sin16(ax))+","+String(sin16(ay))+")");
//        Serial.println("Coord (" + String(x1/256)+","+String(y1/256)+")");
  }

  if (s++ > 500)
    s = 0;
  ax += sx;
  ay += sy;
  if (beat)
  {
    hue += 160;
    col = CHSV(hue,127,255);
    lw = 896;
  }
  x2 = (long)cos16(ax) * LAST_COL / 255 + MID_COL * 255;
  y2 = (long)sin16(ay) * LAST_ROW / 255 + MID_ROW * 255;

  DrawLine16(x1, y1, x2, y2, lw, col);
  Refresh();
  AllFade();
  x1 = x2;
  y1 = y2;
}

void quad(boolean beat)
{
  int16_t x[4]; //={256,2560,2560,256};
  int16_t y[4]; //={256,2560,256,2560};
  AllFall();
  if (beat)
  {
    for (int8_t i = 0; i < 4; i++)
    {
      x[i] = random16(0, LAST_COL << 8);
      y[i] = random16(0, LAST_ROW << 7);
    }
    DrawPoly16(4, x, y, CHSV(random8(), 255, 255));
  }
  Refresh();
}

void spin(boolean beat)
{
  static uint16_t t = 0, a = 0;
  const uint16_t BPM = analOut.tempo>70?analOut.tempo:120;

  int16_t x[4], xc = MID_COL << 8;
  int16_t y[4], yc = MID_ROW << 8;
  a += (long)65536 * EPM / 240 / UPDATES_PER_SECOND;
  int16_t z = sin16(a);
  float fadj = (65536.0 + z) / 65536;
  float badj = (65536.0 - z) / 65536;
  //    Serial.println("ang="+String(a)+" z="+String(z)+" front scale="+String(fadj)+" back scale="+String(badj));
  int16_t xadj = (long)cos16(a) * MID_COL / 256;
  int16_t yadj = MID_ROW << 7;
  x[0] = xc - xadj * badj;
  y[0] = yc - yadj * badj;
  x[1] = xc + xadj * fadj;
  y[1] = yc - yadj * fadj;
  x[2] = x[1];
  y[2] = yc + yadj * fadj;
  x[3] = x[0];
  y[3] = yc + yadj * badj;
  if (beat)
  {
    hue += 160;
    DrawPoly16(4, x, y, CRGB::White);
  }
  else
  {
    DrawBorder16(4, x, y, CHSV(hue, 255, 255));
  }
  Refresh();
  AllBlack();
}

void bounce(boolean beat)
{
  static int16_t t = 0, x[4], y[4];
  int8_t i;
  static int8_t f = 1;
  static int8_t dx, dy;

  if (beat || f)
  {
    for (i = 0; i < 4; i++)
    {
      x[i] = random16(0, LAST_COL << 8);
      y[i] = random16(0, LAST_ROW << 8);
    }
    dx = random8(20, 60);
    dy = random8(20, 60);
    f = 0;
  }
  if (beat)
    hue += 160;
  AllBlack();
  DrawPoly16(4, x, y, CHSV(hue, 255, 255));
  Refresh();
  for (i = 0; i < 4; i++)
  {
    x[i] += dx;
    y[i] += dy;
  }
  for (i = 0; i < 4; i++)
  {
    if (x[i] < 0)
    {
      dx = -dx;
      i = 4;
    }
    else
    {
      if (x[i] > (LAST_COL << 8))
      {
        dx = -dx;
        i = 4;
      }
    }
  }
  for (i = 0; i < 4; i++)
  {
    if (y[i] < 0)
    {
      dy = -dy;
      i = 4;
    }
    else
    {
      if (y[i] > (LAST_ROW << 8))
      {
        dy = -dy;
        i = 4;
      }
    }
  }
}

void drawEffect(boolean beatnow)
{
  static uint8_t effect=0;
  static uint16_t frameCount=0;
  uint16_t BPM = analOut.tempo>70?analOut.tempo:120;

  frameCount++;
  if (frameCount > 400)
  {
    frameCount=0;
    EPM=variant(BPM);
    effect++;
    if (effect > 5)
    {
      effect=0;
    }
    Serial.print("Effect no:");
    Serial.println(effect);
  }

  switch (effect)
  {
    case 0:
      bounce(beatnow);
      break;
    case 1:
      spin(beatnow);
      break;
    case 2:
      quad(beatnow);
      break;
    case 3:
      mixup(beatnow);
      break;
    case 4:
      sweep(beatnow);
      break;
    case 5:
     weave16(beatnow);
      break;
  }
}