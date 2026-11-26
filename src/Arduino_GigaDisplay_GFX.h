
#ifndef __ARDUINO_GIGADISPLAY_GFX__
#define __ARDUINO_GIGADISPLAY_GFX__

#if defined(ARDUINO_ARCH_ZEPHYR)
#include "Arduino_Video.h"
#else
#include "Arduino_H7_Video.h"
using Arduino_Video = Arduino_H7_Video;
#endif
#include "Adafruit_GFX.h"

#if __MBED__
#include "Adafruit_SPITFT.h"
#include "dsi.h"
#include "SDRAM.h"
#endif

class ArduinoVideoDisplay_GFX : public Adafruit_GFX {
  public:
    ArduinoVideoDisplay_GFX(uint16_t w, uint16_t h);
    ~ArduinoVideoDisplay_GFX(void);
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
    void startBuffering();
    void endBuffering();

    uint16_t color565(uint8_t red, uint8_t green, uint8_t blue) {
      return ((red & 0xF8) << 8) | ((green & 0xFC) << 3) | (blue >> 3);
    }

  protected:
    uint16_t getRawPixel(int16_t x, int16_t y);
    void drawFastRawVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
    void drawFastRawHLine(int16_t x, int16_t y, int16_t w, uint16_t color);
    uint16_t *buffer = nullptr; ///< Raster data: no longer private, allow subclass access
    virtual Arduino_Video* allocateArduinoVideoDevice() = 0;  // allow the sub-classes to allocate for us.

  private:
    Arduino_Video* display;
#ifdef __MBED__
    rtos::Thread* _refresh_thd;
    void refresh_if_needed();
#endif
    bool buffering = false;
    uint32_t last_refresh = 0;
};

class GigaDisplay_GFX : public ArduinoVideoDisplay_GFX {
  public:
    GigaDisplay_GFX();
    virtual Arduino_Video* allocateArduinoVideoDevice();

};

#if defined(ARDUINO_PORTENTA_H7_M7)
class ArduinoUSBCVideo_GFX : public ArduinoVideoDisplay_GFX {
  public:
    ArduinoUSBCVideo_GFX();
    virtual Arduino_Video* allocateArduinoVideoDevice();

};
#endif // ARDUINO_PORTENTA_H7_M7

#endif //__ARDUINO_GIGADISPLAY_GFX__