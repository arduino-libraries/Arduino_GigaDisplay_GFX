/*
  GigaDisplay GFX - Basic
  This sketch initializes the Arduino Giga Display in Landscape mode (rotated 90°)
  and draws a solid red rectangle perfectly centered on the screen.
  Default orientation is Portrait (480x800).
*/

#include "Arduino_GigaDisplay_GFX.h"

GigaDisplay_GFX display;

void setup() {
  Serial.begin(115200);
  display.begin();

  // Set rotation to 90 degrees (Landscape) 
  // Default is 0 (Portrait, where width is the short side: 480)
  display.setRotation(1);

  // Clear screen to black
  display.fillScreen(0x0000);

  // Define rectangle dimensions
  int rectW = 200;
  int rectH = 100;

  // Calculate centered coordinates (now based on 800x480)
  int x = (display.width() - rectW) / 2;
  int y = (display.height() - rectH) / 2;

  // Generate 16-bit RGB565 red color
  uint16_t red = display.color565(255, 0, 0);

  // Draw filled red rectangle at center
  display.fillRect(x, y, rectW, rectH, red);
}

void loop() {
}