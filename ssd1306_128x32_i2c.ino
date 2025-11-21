#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
// #include <TimeLib.h>

#define OLED_ADDR   0x3C
Adafruit_SSD1306 display(128, 32, &Wire, -1);

void setup() {
  Wire.begin(1,0);
  display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR);
  display.clearDisplay();
  display.setTextColor(INVERSE);
  display.setTextSize(2.5);
  display.setCursor(15,15);
  display.println("67:42.180");
  display.display();

}

void loop() {

}