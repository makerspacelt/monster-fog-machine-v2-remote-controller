#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <Ewma.h>

#define TINY4KOLED_QUICK_BEGIN
#include <Tiny4kOLED.h>

#define NO_DOUBLE_BUFFERING

#define SWITCH_PIN PB1
#define DURATION_POT A2
#define INTERVAL_POT A3
#define TIME_FACTOR 120 // increase this to add more time

long duration;
long interval;
unsigned long durationMillis;
unsigned long intervalMillis;
unsigned long lastMillis;
int outputState;

// needed for smoother ADC reading
Ewma durationFilter(0.4);
Ewma intervalFilter(0.4);

void printDurationToScr(long secs) {
    oled.setCursor(96, 0);
    oled.print(secs);
    if (secs < 100) oled.print(" ");
}

void printIntervalToScr(int secs) {
    oled.setCursor(96, 2);
    oled.print(secs);
    if (secs < 100) oled.print(" ");
}

void setupLcd() {
    oled.begin();
    oled.clear();
    oled.setFont(FONT8X16);
    oled.setCursor(0, 0);
    oled.print("DURATION s:");
    oled.setCursor(0, 2);
    oled.print("INTERVAL s:");
    // oled.print(oled.getCursorX());
    oled.on();
}

long calcMillis(long val) {
    long t = ceil((val * TIME_FACTOR) / 1000);
    return ((t < 1) ? 1 : t) * 1000;
}

void processInput() {
    duration = durationFilter.filter(analogRead(DURATION_POT)+1);
    duration = (duration < 8) ? 8 : duration;
    interval = intervalFilter.filter(analogRead(INTERVAL_POT)+1);
    interval = (interval < 8) ? 8 : interval;

    /*
        we must divide the final millis by 2 because the internal clock somehow
        reports twice as much millis and so timekeeping becomes very inaccurate
    */
    durationMillis = calcMillis(duration) / 2;
    printDurationToScr((durationMillis * 2) / 1000);
    intervalMillis = calcMillis(interval) / 2;
    printIntervalToScr((intervalMillis * 2) / 1000);
}

void setup() {
    setupLcd();
    pinMode(SWITCH_PIN, OUTPUT);
    pinMode(DURATION_POT, INPUT);
    pinMode(INTERVAL_POT, INPUT);
    digitalWrite(SWITCH_PIN, LOW);
}

void loop() {
    processInput();

    bool changeState = false;
    if ((outputState == LOW) && (unsigned long)(millis() - lastMillis) >= intervalMillis) changeState = true;
    else if ((outputState == HIGH) && (unsigned long)(millis() - lastMillis) >= durationMillis) changeState = true;

    if (changeState) {
        outputState = (outputState == LOW) ? HIGH : LOW;
        digitalWrite(SWITCH_PIN, outputState);
        lastMillis = millis();
    }

    
}