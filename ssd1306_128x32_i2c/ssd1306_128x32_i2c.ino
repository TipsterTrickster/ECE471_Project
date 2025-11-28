#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "Adafruit_MPR121.h"
// #include <TimeLib.h>

#define OLED_ADDR   0x3C
#define DEBOUNCE_DELAY 50
#define TOUCH_SENSOR_THRESHOLD 6

Adafruit_SSD1306 display(128, 32, &Wire, -1);
Adafruit_MPR121 cap = Adafruit_MPR121();

int started = 0;
int stopped = 0;
int touched = 0;
unsigned long static startTime = 0;

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
  display.setCursor(5,15);
  display.println("00:00.000");
  display.display();


}

void loop() {

  // unsigned long timeNow = millis();

  // // Debouncing
  // if((millis() - timeNow) > DEBOUNCE_DELAY){
  //   touched = 1;
  // }
  
  // if ((cap.filteredData(0) < TOUCH_SENSOR_THRESHOLD) && (cap.filteredData(1) < TOUCH_SENSOR_THRESHOLD)) {
  //   unsigned long touch_time = millis();
  //   if((millis() - touch_time) > DEBOUNCE_DELAY) touched = 1;
  // }
  
  // Start timer only if both sensors are touched simultaneously for a period longer than debounce delay
  if ((cap.filteredData(0) < TOUCH_SENSOR_THRESHOLD) && (cap.filteredData(1) < TOUCH_SENSOR_THRESHOLD) && !started) {
    unsigned long static touch_time = millis();
    if((millis() - touch_time) > DEBOUNCE_DELAY){
      while ((cap.filteredData(0) < TOUCH_SENSOR_THRESHOLD) && (cap.filteredData(1) < TOUCH_SENSOR_THRESHOLD));
      started = 1;
      touched = 0;
      startTime = millis();
    }

  }

  if (started) {
    if ((cap.filteredData(0) < TOUCH_SENSOR_THRESHOLD) && (cap.filteredData(1) < TOUCH_SENSOR_THRESHOLD)) {

      while ((cap.filteredData(0) < TOUCH_SENSOR_THRESHOLD) && (cap.filteredData(1) < TOUCH_SENSOR_THRESHOLD));
      started = 0;
    }
    
    // Find elapsed time from start of stopwatch
    unsigned long elapsed = millis() - startTime;

    unsigned int minutes = elapsed / 60000;
    unsigned int seconds = (elapsed % 60000) / 1000;
    unsigned int milliseconds = elapsed % 1000;

    display.clearDisplay();
    display.setTextColor(INVERSE);
    display.setTextSize(2.5);
    display.setCursor(5,15);


    /* Display the time on LCD screen */
    if(minutes<10) display.print("0"); // Padding zeros
    display.print(minutes);
    display.print(":");
    if(seconds<10) display.print("0"); // Padding zeros
    display.print(seconds);
    display.print(".");
    if(milliseconds<10) display.print("00"); // Padding zeros
    if(milliseconds<100) display.print("0"); // Padding zeros
    display.print(milliseconds);

    display.display();

    // put a delay so it isn't overwhelming
    delay(10);
  }
}