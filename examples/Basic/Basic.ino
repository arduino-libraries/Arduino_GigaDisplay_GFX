#include "Arduino_GigaDisplay_GFX.h"

GigaDisplay_GFX display;

void setup() {
  Serial.begin(115200);
}

void loop() {
  Serial.println(millis());
  display.fillScreen(0);
  display.fillRect(120, 180, 120, 180, 0x8888);
  display.print(false);
  delay(100);
}