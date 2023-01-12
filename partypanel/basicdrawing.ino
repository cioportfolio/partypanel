#include "common.h"
#include "displayData.h"

void UpdatePixelRaw(int8_t x, int8_t y, CRGB col, uint8_t alpha)
{
  if (alpha == 0)
    return;

  CRGB old = leds[XYsafe(x, y)];
  CRGB nc = col.fadeToBlackBy(255 - alpha);
  uint8_t oldA = 255 - alpha;

  leds[XYsafe(x, y)] = blend(leds[XYsafe(x, y)], col, gamma8[alpha]);
}

void DrawLine16(int16_t x1, int16_t y1, int16_t x2, int16_t y2, int16_t w, CRGB col)
{
  int16_t x[4], y[4], xd, yd;
  long dx = x2 - x1;
  long dy = y2 - y1;
  if (dy==0 && dx==0)
    return;

  float scale = (float)w / 2 / sqrt(dx * dx + dy * dy);

  //  Serial.println("("+String(x1)+","+String(y1)+") ("+String(x2)+","+String(y2)+") w="+String(w));
  if (w < 128)
    return;
  xd = scale * dy;
  yd = scale * dx;
  //  Serial.println(" dx/dy="+String(dx)+"/"+String(dy)+" sqrt="+String(sqrt(dx*dx+dy*dy))+" scale="+String(scale)+" xd/yd="+String(xd)+"/"+String(yd));
  x[0] = x1 - xd;
  x[1] = x1 + xd;
  x[2] = x2 + xd;
  x[3] = x2 - xd;
  y[0] = y1 + yd;
  y[1] = y1 - yd;
  y[2] = y2 - yd;
  y[3] = y2 + yd;
  //  Serial.println("("+String(x[0])+","+String(y[0])+") ("+String(x[1])+","+String(y[1])+") ("+String(x[2])+","+String(y[2])+") ("+String(x[3])+","+String(y[3])+")");
  //  Serial.println("{'test':0}");
  DrawPoly16(4, x, y, col);
}

void DrawLineOld16(int16_t x1, int16_t y1, int16_t x2, int16_t y2, CRGB col)
{
  // Fixed point real coords. High byte is integer part, low byte is fraction

  int8_t u1i = x1 >> 8;
  int8_t v1i = y1 >> 8;
  uint8_t u1f = (x1 & 0xFF);
  uint8_t v1f = (y1 & 0xFF);

  int8_t u2i = x2 >> 8;
  int8_t v2i = y2 >> 8;
  uint8_t u2f = (x2 & 0xFF);
  uint8_t v2f = (y2 & 0xFF);

  if (u1f + v1f + u2f + v2f == 0)
  {
    // Integer coordinates
    DrawLine(u1i, v1i, u2i, v2i, col);
    return;
  }

  int xdelt = x2 - x1;
  int ydelt = y2 - y1;

  int xdir = Sign(xdelt);
  int ydir = Sign(ydelt);

  if (xdir == 0 && ydir == 0)
  {
    // Single point

    UpdatePixelRaw(u1i, v1i, col, scale8(255 - u1f, 255 - v1f));
    UpdatePixelRaw(u1i + 1, v1i, col, scale8(u1f, 255 - v1f));
    UpdatePixelRaw(u1i, v1i + 1, col, scale8(255 - u1f, v1f));
    UpdatePixelRaw(u1i + 1, v1i + 1, col, scale8(u1f, v1f));
    return;
  }

  //Complex line
  uint8_t swap = 0;
  int16_t u1, v1, u2, v2, udelt, vdelt;
  int dir;

  // move things around so that:
  // u has more steps than v
  // u2 is greater than u1
  // line goes from u1,v1 to u2,v2

  if (xdelt * xdir >= ydelt * ydir)
  {
    //x-axis is longer so u is x-axis
    udelt = xdelt * xdir;
    vdelt = ydelt * ydir;
    if (xdir < 0)
    {
      //x2 is less than x1 so swap line around
      u1 = x2;
      u2 = x1;
      v1 = y2;
      v2 = y1;
      dir = -ydir;
    }
    else
    {
      u1 = x1;
      u2 = x2;
      v1 = y1;
      v2 = y2;
      dir = ydir;
    }
  }
  else
  {
    //y-axis equal or longer so u is y-axis
    swap = 1;
    udelt = ydelt * ydir;
    vdelt = xdelt * xdir;
    if (ydir < 0)
    {
      // y2 is less than y1 so swap line around
      u1 = y2;
      u2 = y1;
      v1 = x2;
      v2 = x1;
      dir = -xdir;
    }
    else
    {
      u1 = y1;
      u2 = y2;
      v1 = x1;
      v2 = x2;
      dir = xdir;
    }
  }
  u1i = u1 >> 8;
  v1i = v1 >> 8;
  u1f = (u1 & 0xFF);
  v1f = (v1 & 0xFF);

  u2i = u2 >> 8;
  v2i = v2 >> 8;
  u2f = (u2 & 0xFF);
  v2f = (v2 & 0xFF);

  // handle first and last points as special cases
  UpdatePixel(swap, u1i, v1i, col, scale8(255 - u1f, 255 - v1f));
  UpdatePixel(swap, u1i, v1i + 1, col, scale8(255 - u1f, v1f));

  UpdatePixel(swap, u2i + 1, v2i, col, scale8(u2f, 255 - v2f));
  UpdatePixel(swap, u2i + 1, v2i + 1, col, scale8(u2f, v2f));
  // Loop through remaining points which will be full width in u axis
  for (int16_t u = u1i + 1; u <= u2i; u++)
  {
    int16_t v = long((u * 256) - u1) * vdelt / udelt * dir + v1;
    int8_t vi = v >> 8;
    uint8_t vf = v & 0xFF;
    UpdatePixel(swap, u, vi, col, 255 - vf);
    UpdatePixel(swap, u, vi + 1, col, vf);
  }
}

void UpdatePixel(uint8_t swapAxis, int8_t u, int8_t v, CRGB col, uint8_t alpha)
{
  if (swapAxis == 0)
  {
    UpdatePixelRaw(u, v, col, alpha);
  }
  else
  {
    UpdatePixelRaw(v, u, col, alpha);
  }
}

int Sign(int val)
{
  if (val == 0)
    return 0;
  if (val > 0)
    return 1;
  return -1;
}

void SimpleLine(uint8_t swap, int8_t u1, int8_t u2, int8_t v, CRGB col)
{
  int8_t b = u2;
  int8_t e = u1;
  if (Sign(u2 - u1) > 0)
  {
    b = u1;
    e = u2;
  }
  for (int8_t p = b; p <= e; p++)
  {
    UpdatePixel(swap, p, v, col, 255);
  }
}

int8_t abuff[kMatrixWidth * 2];

void clearBuff()
{
  for (int8_t c = 0; c < kMatrixWidth * 2; c++)
  {
    abuff[c] = 0;
  }
}

int8_t cell(int16_t x, int16_t y)
{
  return ((x >> 8) << 1) + ((y >> 7) & 0x01);
}

int8_t shift(int16_t x, int16_t y)
{
  return ((x >> 6) & 0x03) + (((y >> 6) & 0x01) << 2);
}

boolean getBit(int16_t x, int16_t y)
{
  return (abuff[cell(x, y)] >> (shift(x, y))) & 0x01;
}

void setBit(int16_t x, int16_t y)
{
  int8_t mask = 0x01 << shift(x, y);
  abuff[cell(x, y)] = abuff[cell(x, y)] | mask;
}

int8_t bitCount(byte b)
{
  int8_t c;
  byte v = b;
  for (c = 0; v; c++)
  {
    v &= v - 1; // clear the least significant bit set
    //    Serial.println("btye="+String((byte)v,BIN)+" count="+String(c));
  }
  //  Serial.println("btye="+String((byte)b,BIN)+" count="+String(c));
  return c;
}

uint8_t getAlpha(int8_t x)
{
  uint8_t a = bitCount(abuff[x * 2]) + bitCount(abuff[x * 2 + 1]);
  if (a == 16)
    return 255;
  return a << 4;
}

void DrawBorder16(int8_t count, int16_t x[], int16_t y[], CRGB col)
{
  int8_t w = count - 1, v;

  for (v = 0; v < count; v++)
  {
    DrawLineOld16(x[w], y[w], x[v], y[v], col);
    w = v;
  }
}

void DrawPoly16(int8_t count, int16_t x[], int16_t y[], CRGB col)
{
  int8_t nodes, edges, edge[count], w, v, r, minX, maxX;
  int16_t r16, swap, nodex[count], c, x1, x2;

  for (r = 0; r < kMatrixHeight; r++)
  {
    //   Serial.println("Row " + String(r));
    clearBuff();
    r16 = r * 256;
    for (byte rf = 0; rf < 4; rf++)
    {
      nodes = 0;
      w = count - 1;
      for (v = 0; v < count; v++)
      {
        if ((y[w] < r16 && y[v] >= r16) || (y[w] > r16 && y[v] <= r16))
        {
          nodex[nodes] = (x[w] + int((x[v] - x[w]) * float(r16 - y[w]) / ((int)y[v] - y[w]))) & 0xFFC0;
          //        Serial.println("v=" + String(v)+" end (" + String(x[v]) + "," + String(y[v]) + ") start (" + String(x[w]) + "," + String(y[w])+")"+" r<<8 " + String(r16));
          //        Serial.println("r-y "+ String(r16 - y[w])+" y delt " + String((int)y[v] - y[w])+ " x delt " + String(x[v] - x[w])+" line frac " + String((float)(r16 - y[w])/(((int)y[v] - y[w])))+" x adj " + String((x[v] - x[w])*float(r16 - y[w])/((int)y[v] - y[w]))+" Node saved="+String(nodex[nodes]));
          if (nodex[nodes] < 0)
            nodex[nodes] = 0;
          if (nodex[nodes] > (kMatrixWidth << 8) - 1)
            nodex[nodes] = (kMatrixWidth << 8) - 1;
          nodes++;
        }
        w = v;
      }
      //    Serial.println("Nodes="+String(nodes));
      if (nodes > 1)
      {
        w = 0;
        while (w < nodes - 1)
        {
          if (nodex[w] > nodex[w + 1])
          {
            swap = nodex[w];
            nodex[w] = nodex[w + 1];
            nodex[w + 1] = swap;
            if (w)
              w--;
          }
          else
          {
            w++;
          }
        }
        for (w = 0; w < nodes; w += 2)
        {
          x1 = nodex[w];
          x2 = nodex[w + 1];
          while (x1 <= x2)
          {
            setBit(x1, r16);
            x1 += 64;
          }
        }
      }
      else
      {
        if (nodes == 1)
        {
          setBit(nodex[0], r16);
          //        Serial.println("Single bit. cell["+String(cell(nodex[0],r16))+"]="+String((byte)abuff[cell(nodex[0],r16)],BIN));
        }
      }
      r16 += 64;
    }
    for (w = 0; w < kMatrixWidth; w++)
    {
      //     Serial.println("("+String(c)+","+String(r)+") alpha="+String(getAlpha(c)));
      v = getAlpha(w);
      if (v)
        UpdatePixelRaw(w, r, col, v);
    }
  }
}

void DrawLine(int8_t x1, int8_t y1, int8_t x2, int8_t y2, CRGB col)
{
  int xdelt = x2 - x1;
  int ydelt = y2 - y1;

  int xdir = Sign(xdelt);
  int ydir = Sign(ydelt);
  if (xdir == 0)
  {
    if (ydir == 0)
    {
      // Single point
      UpdatePixelRaw(x1, y1, col, 255);
      return;
    }
    else
    {
      //Vertical line
      SimpleLine(1, y1, y2, x1, col);
      return;
    }
  }
  else
  {
    if (ydir == 0)
    {
      //Horizontal line
      SimpleLine(0, x1, x2, y1, col);
      return;
    }
    else
    {
      //Complex line
      int8_t u1, u2, v1, v2, udelt, vdelt;
      uint8_t swap;
      int dir;

      // move things around so that:
      // u has more steps than v
      // u2 is greater than u1
      // line goes from u1,v1 to u2,v2
      // dir is the sign of the change form v1 to v2

      if (xdelt * xdir > ydelt * ydir)
      {
        //x-axis is longer so u is x-axis
        swap = 0;
        udelt = xdelt * xdir;
        vdelt = ydelt * ydir;
        if (xdir < 0)
        {
          //x2 is less than x1 so swap line around
          u1 = x2;
          u2 = x1;
          v1 = y2;
          v2 = y1;
          dir = -ydir;
        }
        else
        {
          u1 = x1;
          u2 = x2;
          v1 = y1;
          v2 = y2;
          dir = ydir;
        }
      }
      else
      {
        //y-axis equal or longer so u is y-axis
        swap = 1;
        udelt = ydelt * ydir;
        vdelt = xdelt * xdir;
        if (ydir < 0)
        {
          // y2 is less than y1 so swap line around
          u1 = y2;
          u2 = y1;
          v1 = x2;
          v2 = x1;
          dir = -xdir;
        }
        else
        {
          u1 = y1;
          u2 = y2;
          v1 = x1;
          v2 = x2;
          dir = xdir;
        }
      }
      int inc = 0;
      int8_t v = v1;
      for (int8_t u = u1; u <= u2; u++)
      {
        uint8_t subA = 255 * inc / udelt;
        UpdatePixel(swap, u, v, col, 255 - subA);
        UpdatePixel(swap, u, v + dir, col, subA);
        inc += vdelt;
        if (inc >= udelt)
        {
          inc -= udelt;
          v += dir;
        }
      }
    }
  }
}
