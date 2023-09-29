
#ifndef __ARDUINO_GIGADISPLAY_GFX__
#define __ARDUINO_GIGADISPLAY_GFX__

#include "Arduino_H7_Video.h"
#include "Adafruit_GFX.h"
#include "Adafruit_SPITFT.h"
#include "dsi.h"
#include "SDRAM.h"

class GigaDisplay_GFX : public Adafruit_GFX {
  public:
    GigaDisplay_GFX();
    ~GigaDisplay_GFX(void);
    void begin();
    void drawPixel(int16_t x, int16_t y, uint16_t color);
    void fillScreen(uint16_t color);
    void byteSwap(void);
    void drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
    void drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);
    uint16_t getPixel(int16_t x, int16_t y);
    uint16_t *getBuffer(void) {
      return buffer;
    }
    uint16_t *hasBuffer(void) {
      if (!buffer) {
        begin();
      }
      return buffer;
    }

    void startWrite();
    void endWrite();

    uint16_t color565(uint8_t red, uint8_t green, uint8_t blue) {
      return ((red & 0xF8) << 8) | ((green & 0xFC) << 3) | (blue >> 3);
    }

  protected:
    uint16_t getRawPixel(int16_t x, int16_t y);
    void drawFastRawVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
    void drawFastRawHLine(int16_t x, int16_t y, int16_t w, uint16_t color);
    uint16_t *buffer = nullptr; ///< Raster data: no longer private, allow subclass access

  private:
    Arduino_H7_Video* display;
    void refresh_if_needed();
    bool need_refresh = false;
    uint32_t last_refresh = 0;
    rtos::Thread* _refresh_thd;
};

#endif //__ARDUINO_GIGADISPLAY_GFX__