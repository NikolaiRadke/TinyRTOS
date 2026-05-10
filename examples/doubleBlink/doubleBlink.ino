/*
 * TinyRTOS – Double blink example
 * ATtiny85 with ATTinyCore (Spence Konde)
 *
 * Two tasks:
 *   taskLedSlow – LED at Pin 3 (1000 ms = 1 Hz)
 *   taskLedFast – LED at Pin 4 (250 ms = 4 Hz)
 *
 * Board settings (ATTinyCore):
 *   Board:             ATtiny25/45/85
 *   Chip:              ATtiny85
 *   Clock:             8 MHz (internal)
 *   millis()/micros(): Enabled
 *   BOD:               Disabled  (saves flash)
 *   LTO:               Enabled (saves flash)
 *
 * RAM usage:
 *   2 tasks × 64 bytes stack = 128 Bytes
 *   + kernel oerhead         =  10 Bytes
 *   + program  variables     =   5 Bytes
 *   Total                      143 Bytes
 */

#include "TinyRTOS.h"

#define PIN_LED_SLOW 3 // PB3 (1 Hz)
#define PIN_LED_FAST 4 // PB4 (4 Hz)

// Task 1: Slow LED (1 Hz) 
void taskLedSlow() {
    pinMode(PIN_LED_SLOW, OUTPUT);
    while (true) {
        digitalWrite(PIN_LED_SLOW, HIGH);
        rtos_delay(500);
        digitalWrite(PIN_LED_SLOW, LOW);
        rtos_delay(500);
    }
}

// Task 2: Fast LED (4 Hz)
void taskLedFast() {
    pinMode(PIN_LED_FAST, OUTPUT);
    while (true) {
        digitalWrite(PIN_LED_FAST, HIGH);
        rtos_delay(125);
        digitalWrite(PIN_LED_FAST, LOW);
        rtos_delay(125);
    }
}

// Setup
void setup() {
    rtos_add_task(taskLedSlow);
    rtos_add_task(taskLedFast);

    rtos_run(); // Start scheduler and never come back
}

void loop() {
    // Will never be called
}
