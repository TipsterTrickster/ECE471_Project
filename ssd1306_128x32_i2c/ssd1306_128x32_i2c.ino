#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "Adafruit_MPR121.h"

#define OLED_ADDR   0x3C
#define DEBOUNCE_DELAY 50
#define TOUCH_SENSOR_THRESHOLD 6
#define HOLD_DOWN_MILLIS  420
#define LED_DELAY         4

#define GREEN_LED_PIN 6
#define RED_LED_PIN   7
#define BUTTON_PIN    4

#define RESET     0
#define RUNNING   1
#define HELD_DOWN 2
#define NOT_RESET 3

Adafruit_SSD1306 display(128, 32, &Wire, -1);
Adafruit_MPR121 cap = Adafruit_MPR121();

/* States */
bool lastTouchState = false;
int started = 0;
int state = RESET;

bool green_LED_state = 0;
bool red_LED_state = 1;

/* Timestamps */
unsigned long startTime = 0;
unsigned long elapsed = 0;
int led_toggle_count = 0;


void write_time_to_display(unsigned long elapsed) {

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
    else if(milliseconds<100) display.print("0"); // Padding zeros
    display.print(milliseconds);

    display.display();

}


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

  /* LED GPIOs */
  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(RED_LED_PIN, OUTPUT);

}

void loop() {

  // Check if both sensors are touched
  bool bothTouched = (cap.filteredData(0) < TOUCH_SENSOR_THRESHOLD) && (cap.filteredData(1) < TOUCH_SENSOR_THRESHOLD);

  // If timer currently running
  if (state == RUNNING) {
    
    // Find elapsed time from start of stopwatch
    elapsed = millis() - startTime;

    write_time_to_display(elapsed);

    // Blink the LEDs to show timer is running
    if (led_toggle_count > LED_DELAY) {

      red_LED_state = !red_LED_state;
      green_LED_state = !green_LED_state;

      digitalWrite(RED_LED_PIN, red_LED_state);
      digitalWrite(GREEN_LED_PIN, green_LED_state);

      led_toggle_count = 0;
    }
    led_toggle_count++;

    // Check for touch and release then change state
    if (bothTouched) {
      delay(10);
      digitalWrite(RED_LED_PIN, 1);
      digitalWrite(GREEN_LED_PIN, 0);
      while(bothTouched) {
        bothTouched = (cap.filteredData(0) < TOUCH_SENSOR_THRESHOLD) && (cap.filteredData(1) < TOUCH_SENSOR_THRESHOLD);
      }
      digitalWrite(RED_LED_PIN, 0);
      state = NOT_RESET;
    }

  // If the timer is reset
  } else if (state == RESET) {
    
    // Light red LED and switch state
    if (bothTouched) {
      digitalWrite(RED_LED_PIN, 1);
      startTime = millis();
      state = HELD_DOWN;
    }
  
  // If both sensors are currently being touched before starting the timer
  } else if (state == HELD_DOWN) {

    // Still held down, delay time is over
    if (bothTouched && (millis() - startTime) > HOLD_DOWN_MILLIS) {
      
      // Light green LED
      digitalWrite(RED_LED_PIN, 0);
      digitalWrite(GREEN_LED_PIN, 1);

    // Started
    } else if ((millis() - startTime) > HOLD_DOWN_MILLIS) {
      startTime = millis();
      state = RUNNING;
      digitalWrite(GREEN_LED_PIN, 0);
    } else if (!bothTouched) {
      digitalWrite(RED_LED_PIN, 0);
      state = RESET;
    }

  // If timer has been stopped but not reset just display last time and have LEDs respond to touch
  } else if (state == NOT_RESET) {

    if (bothTouched) digitalWrite(RED_LED_PIN, 1);
    else digitalWrite(RED_LED_PIN, 0);

    write_time_to_display(elapsed);

  }
  
}