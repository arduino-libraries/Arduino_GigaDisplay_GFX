
#include "Arduino_GigaDisplay_GFX.h"

#ifdef __MBED__
#include "platform/mbed_critical.h"
#endif

GigaDisplay_GFX::GigaDisplay_GFX() : Adafruit_GFX(480, 800) { }

GigaDisplay_GFX::~GigaDisplay_GFX(void) {
  if (buffer)
    free(buffer);
}

#ifdef __MBED__
void GigaDisplay_GFX::refresh_if_needed() {
  while (1) {
    rtos::ThisThread::flags_wait_any(0x1);
    dsi_lcdDrawImage((void *) this->getBuffer(), (void *)(dsi_getActiveFrameBuffer()), 480, 800, DMA2D_INPUT_RGB565);
    delay(10);
  }
}
#endif

void GigaDisplay_GFX::begin() {
    display = new Arduino_Video(480, 800, GigaDisplayShield);
    display->begin();

    #ifdef __MBED__
      buffer = (uint16_t*)ea_malloc(this->width() * this-> height() * 2);
      _refresh_thd = new rtos::Thread(osPriorityHigh);
      _refresh_thd->start(mbed::callback(this, &GigaDisplay_GFX::refresh_if_needed));
    #elif defined(__ZEPHYR__)
      #ifdef CONFIG_SHARED_MULTI_HEAP
        void* ptrFB = this->display->getFramebuffer();
        if (ptrFB == nullptr){
          while(1){}
        }
        // Cast the void pointer to an int pointer to use it
        buffer = static_cast<uint16_t*>(ptrFB);
      #else
        SDRAM.begin();
        buffer = (uint16_t*)SDRAM.malloc(this->width() * this-> height() * sizeof(uint16_t));
      #endif   
      this->display->setFrameDesc(this->width(), this->height(), this->width(), (this->width() * this-> height() * sizeof(uint16_t)));
    #endif
}

void GigaDisplay_GFX::startWrite() { }

void GigaDisplay_GFX::endWrite() {
#ifdef __MBED__
  if (!buffering)
    _refresh_thd->flags_set(0x1);
#elif defined(__ZEPHYR__)
  if (!buffering)
     this->display->drawBuffer(0, 0, buffer);
#endif
}

// If buffering, defer endWrite calls until endBuffering is called.
void GigaDisplay_GFX::startBuffering() {
  buffering = true;
}

void GigaDisplay_GFX::endBuffering() {
  if (buffering) {
    buffering = false;
    endWrite();
  }
}

void GigaDisplay_GFX::drawPixel(int16_t x, int16_t y, uint16_t color) {
  if (hasBuffer()) {
    if ((x < 0) || (y < 0) || (x >= _width) || (y >= _height))
      return;

    int16_t t;
    switch (rotation) {
      case 1:
        t = x;
        x = WIDTH - 1 - y;
        y = t;
        break;
      case 2:
        x = WIDTH - 1 - x;
        y = HEIGHT - 1 - y;
        break;
      case 3:
        t = x;
        x = y;
        y = HEIGHT - 1 - t;
        break;
    }

    buffer[x + y * WIDTH] = color;
  }
}

uint16_t GigaDisplay_GFX::getPixel(int16_t x, int16_t y) {
  int16_t t;
  switch (rotation) {
    case 1:
      t = x;
      x = WIDTH - 1 - y;
      y = t;
      break;
    case 2:
      x = WIDTH - 1 - x;
      y = HEIGHT - 1 - y;
      break;
    case 3:
      t = x;
      x = y;
      y = HEIGHT - 1 - t;
      break;
  }
  return getRawPixel(x, y);
}

uint16_t GigaDisplay_GFX::getRawPixel(int16_t x, int16_t y) {
  if ((x < 0) || (y < 0) || (x >= WIDTH) || (y >= HEIGHT))
    return 0;
  if (hasBuffer()) {
    return buffer[x + y * WIDTH];
  }
  return 0;
}

void GigaDisplay_GFX::fillScreen(uint16_t color) {
  if (hasBuffer()) {
	startWrite();	// PR #3
    uint8_t hi = color >> 8, lo = color & 0xFF;
    if (hi == lo) {
      memset(buffer, lo, WIDTH * HEIGHT * 2);
    } else {
      uint32_t i, pixels = WIDTH * HEIGHT;
      for (i = 0; i < pixels; i++)
        buffer[i] = color;
    }
    endWrite();		// PR #3
  }
}

void GigaDisplay_GFX::byteSwap(void) {
  if (hasBuffer()) {
    uint32_t i, pixels = WIDTH * HEIGHT;
    for (i = 0; i < pixels; i++)
      buffer[i] = __builtin_bswap16(buffer[i]);
  }
}

void GigaDisplay_GFX::drawFastVLine(int16_t x, int16_t y, int16_t h,
                                    uint16_t color) {
  if (h < 0) { // Convert negative heights to positive equivalent
    h *= -1;
    y -= h - 1;
    if (y < 0) {
      h += y;
      y = 0;
    }
  }

  // Edge rejection (no-draw if totally off canvas)
  if ((x < 0) || (x >= width()) || (y >= height()) || ((y + h - 1) < 0)) {
    return;
  }

  if (y < 0) { // Clip top
    h += y;
    y = 0;
  }
  if (y + h > height()) { // Clip bottom
    h = height() - y;
  }

  if (getRotation() == 0) {
    drawFastRawVLine(x, y, h, color);
  } else if (getRotation() == 1) {
    int16_t t = x;
    x = WIDTH - 1 - y;
    y = t;
    x -= h - 1;
    drawFastRawHLine(x, y, h, color);
  } else if (getRotation() == 2) {
    x = WIDTH - 1 - x;
    y = HEIGHT - 1 - y;

    y -= h - 1;
    drawFastRawVLine(x, y, h, color);
  } else if (getRotation() == 3) {
    int16_t t = x;
    x = y;
    y = HEIGHT - 1 - t;
    drawFastRawHLine(x, y, h, color);
  }
}

void GigaDisplay_GFX::drawFastHLine(int16_t x, int16_t y, int16_t w,
                                    uint16_t color) {
  if (w < 0) { // Convert negative widths to positive equivalent
    w *= -1;
    x -= w - 1;
    if (x < 0) {
      w += x;
      x = 0;
    }
  }

  // Edge rejection (no-draw if totally off canvas)
  if ((y < 0) || (y >= height()) || (x >= width()) || ((x + w - 1) < 0)) {
    return;
  }

  if (x < 0) { // Clip left
    w += x;
    x = 0;
  }
  if (x + w >= width()) { // Clip right
    w = width() - x;
  }

  if (getRotation() == 0) {
    drawFastRawHLine(x, y, w, color);
  } else if (getRotation() == 1) {
    int16_t t = x;
    x = WIDTH - 1 - y;
    y = t;
    drawFastRawVLine(x, y, w, color);
  } else if (getRotation() == 2) {
    x = WIDTH - 1 - x;
    y = HEIGHT - 1 - y;

    x -= w - 1;
    drawFastRawHLine(x, y, w, color);
  } else if (getRotation() == 3) {
    int16_t t = x;
    x = y;
    y = HEIGHT - 1 - t;
    y -= w - 1;
    drawFastRawVLine(x, y, w, color);
  }
}

void GigaDisplay_GFX::drawFastRawVLine(int16_t x, int16_t y, int16_t h,
                                       uint16_t color) {
  startWrite();
  // x & y already in raw (rotation 0) coordinates, no need to transform.
  uint16_t *buffer_ptr = buffer + y * WIDTH + x;
  for (int16_t i = 0; i < h; i++) {
    (*buffer_ptr) = color;
    buffer_ptr += WIDTH;
  }
  endWrite();
}

void GigaDisplay_GFX::drawFastRawHLine(int16_t x, int16_t y, int16_t w,
                                       uint16_t color) {
  startWrite();
  // x & y already in raw (rotation 0) coordinates, no need to transform.
  uint32_t buffer_index = y * WIDTH + x;
  for (uint32_t i = buffer_index; i < buffer_index + w; i++) {
    buffer[i] = color;
  }
  endWrite();
}
