#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
<<<<<<< HEAD
// #include <TimeLib.h>

#define OLED_ADDR 0x3C
Adafruit_SSD1306 display(128, 32, &Wire, -1);

void setup() {
  Wire.begin(1,0);
=======
#include "Adafruit_MPR121.h"
// #include <TimeLib.h>

#define OLED_ADDR   0x3C
Adafruit_SSD1306 display(128, 32, &Wire, -1);

Adafruit_MPR121 cap = Adafruit_MPR121();

int counter = 0;
int started = 0;
int stopped = 0;

void setup() {
  
  Serial.begin(9600);
  while (!Serial) { // needed to keep leonardo/micro from starting too fast!
    delay(10);
  }

  Wire.begin(1,0);

  /* Touch Sensor Config */
  if (!cap.begin(0x5A, &Wire)) {
    Serial.println("MPR121 not found, check wiring?");
    while (1);
  }
  Serial.println("MPR121 found!");

  /* Display Config */
  display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR);
  display.clearDisplay();
  display.setTextColor(INVERSE);
  display.setTextSize(2.5);
  display.setCursor(15,15);
  display.println("0:00.000");
  display.display();
}

void loop() {

  if ((cap.filteredData(0) < 6) && (cap.filteredData(1) < 6) && !started) {

    while ((cap.filteredData(0) < 6) && (cap.filteredData(1) < 6));

    started = 1;
    counter = 0;
  }

  if (started) {
    if ((cap.filteredData(0) < 6) && (cap.filteredData(1) < 6)) {

      while ((cap.filteredData(0) < 6) && (cap.filteredData(1) < 6));

      started = 0;
    }

    display.clearDisplay();
    display.setTextColor(INVERSE);
    display.setTextSize(2.5);
    display.setCursor(15,15);
    display.print(counter / 100);
    display.print(".");
    display.print(counter % 100);
    display.display();

    counter++;
    
    // put a delay so it isn't overwhelming
    delay(10);
  }
}