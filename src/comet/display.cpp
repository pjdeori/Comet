#include "utils.h"
#include "pin_config.h"

Adafruit_SSD1306 display(screen_width, screen_height, &Wire, oled_reset);

void setup_display() {
  if (!display.begin(SSD1306_SWITCHCAPVCC, screen_address)) {
    Serial.println(F("SSD1306 allocation failed"));
    while (true)
      ;  // Infinite loop to halt
  }

  display.setTextColor(SSD1306_WHITE);
  display.clearDisplay();
  display.display();
}

void print_display(String message, int font_size) {
  display.clearDisplay();
  display.setTextSize(font_size);
  display.setCursor(0, 0);
  display.println(message);
  display.display();
}

void testdrawrect() {
  display.clearDisplay();
  for (int16_t i = 0; i < display.height() / 2; i += 2) {
    display.drawRect(i, i, display.width() - 2 * i, display.height() - 2 * i, SSD1306_WHITE);
    display.display();  // Update screen with each newly-drawn rectangle
    delay(1);
  }
}

void testfillrect() {
  display.clearDisplay();
  for (int16_t i = 0; i < display.height() / 2; i += 3) {
    // The INVERSE color is used so rectangles alternate white/black
    display.fillRect(i, i, display.width() - i * 2, display.height() - i * 2, SSD1306_INVERSE);
    display.display();  // Update screen with each newly-drawn rectangle
    delay(1);
  }
}

void testdrawcircle() {
  display.clearDisplay();
  for (int16_t i = 0; i < max(display.width(), display.height()) / 2; i += 2) {
    display.drawCircle(display.width() / 2, display.height() / 2, i, SSD1306_WHITE);
    display.display();
    delay(1);
  }
}

void testfillcircle() {
  display.clearDisplay();
  for (int16_t i = max(display.width(), display.height()) / 2; i > 0; i -= 3) {
    // The INVERSE color is used so circles alternate white/black
    display.fillCircle(display.width() / 2, display.height() / 2, i, SSD1306_INVERSE);
    display.display();  // Update screen with each newly-drawn circle
    delay(1);
  }
}

void testdrawroundrect() {
  display.clearDisplay();
  for (int16_t i = 0; i < display.height() / 2 - 2; i += 2) {
    display.drawRoundRect(i, i, display.width() - 2 * i, display.height() - 2 * i, display.height() / 4, SSD1306_WHITE);
    display.display();
    delay(1);
  }
}

void testfillroundrect() {
  display.clearDisplay();
  for (int16_t i = 0; i < display.height() / 2 - 2; i += 2) {
    // The INVERSE color is used so round-rects alternate white/black
    display.fillRoundRect(i, i, display.width() - 2 * i, display.height() - 2 * i, display.height() / 4, SSD1306_INVERSE);
    display.display();
    delay(1);
  }
}

void testdrawchar() {
  display.clearDisplay();
  display.setTextSize(1);               // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);  // Draw white text
  display.setCursor(0, 0);              // Start at top-left corner
  display.cp437(true);                  // Use full 256 char 'Code Page 437' font

  // Not all the characters will fit on the display. This is normal.
  // Library will draw what it can and the rest will be clipped.
  for (int16_t i = 0; i < 256; i++) {
    if (i == '\n') display.write(' ');
    else display.write(i);
  }
  display.display();
}

void testdrawstyles() {
  display.clearDisplay();
  display.setTextSize(1);               // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);  // Draw white text
  display.setCursor(0, 0);              // Start at top-left corner
  display.println(F("Hello, world!"));
  display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);  // Draw 'inverse' text
  display.println(3.141592);
  display.setTextSize(2);  // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.print(F("0x"));
  display.println(0xDEADBEEF, HEX);
  display.display();
}

void test_display() {
  testdrawrect();       // Draw rectangles (outlines)
  testfillrect();       // Draw rectangles (filled)
  testdrawcircle();     // Draw circles (outlines)
  testfillcircle();     // Draw circles (filled)
  testdrawroundrect();  // Draw rounded rectangles (outlines)
  testfillroundrect();  // Draw rounded rectangles (filled)
  testdrawchar();       // Draw characters of the default font
  testdrawstyles();     // Draw 'stylized' characters
}