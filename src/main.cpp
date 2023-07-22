#include <Arduino.h>
#include <TinyWireM.h>
#include <MagicPot.h>

#define TINY4KOLED_QUICK_BEGIN
#include <Tiny4kOLED.h>

#define NO_DOUBLE_BUFFERING

#define SWITCH_PIN PB1
#define DURATION_POT A2
#define INTERVAL_POT A3
#define DURATION_MIN_READ 1
#define DURATION_MAX_READ 60
#define INTERVAL_MIN_READ 1
#define INTERVAL_MAX_READ 30
#define DURATION_STR "DURATION s:"
#define INTERVAL_STR "INTERVAL s:"

MagicPot durationPot(DURATION_POT, DURATION_MIN_READ, DURATION_MAX_READ);
MagicPot intervalPot(INTERVAL_POT, INTERVAL_MIN_READ, INTERVAL_MAX_READ);

int duration;
int interval;
int outputState;
unsigned long durationMillis;
unsigned long intervalMillis;
unsigned long lastMillis;
unsigned long lastPollMillis;

void onDurationChange() {
    duration = durationPot.getValue();
    durationMillis = duration * 1000;
    oled.setCursor(96, 0);
    oled.print(duration);
    if (duration < 10) oled.print(" ");
}

void onIntervalChange() {
    interval = intervalPot.getValue();
    intervalMillis = interval * 1000;
    oled.setCursor(96, 2);
    oled.print(interval);
    if (interval < 10) oled.print(" ");
}

void setup() {
    pinMode(SWITCH_PIN, OUTPUT);
    pinMode(DURATION_POT, INPUT);
    pinMode(INTERVAL_POT, INPUT);
    digitalWrite(SWITCH_PIN, LOW);

    oled.begin();
    oled.clear();
    oled.setFont(FONT8X16);
    oled.setCursor(0, 0);
    oled.print(DURATION_STR);
    oled.setCursor(0, 2);
    oled.invertOutput(true);
    oled.print(INTERVAL_STR);
    oled.invertOutput(false);
    // oled.print(oled.getCursorX());
    oled.on();

    durationPot.begin();
    durationPot.onChange(onDurationChange);
    intervalPot.begin();
    intervalPot.onChange(onIntervalChange);
}

void loop() {
    // prevent ultra fast polling for less noise
    if ((unsigned long)millis() - lastPollMillis >= 50) {
        durationPot.read();
        intervalPot.read();
        lastPollMillis = millis();
    }

    bool changeState = false;
    if ((outputState == LOW) && (unsigned long)(millis() - lastMillis) >= intervalMillis) changeState = true;
    else if ((outputState == HIGH) && (unsigned long)(millis() - lastMillis) >= durationMillis) changeState = true;

    if (changeState) {
        outputState = (outputState == LOW) ? HIGH : LOW;
        digitalWrite(SWITCH_PIN, outputState);
        lastMillis = millis();

        oled.setCursor(0, 0);
        oled.invertOutput((outputState == HIGH));
        oled.print(DURATION_STR);

        oled.setCursor(0, 2);
        oled.invertOutput((outputState == LOW));
        oled.print(INTERVAL_STR);
        
        oled.invertOutput(false);
    }
}