/*
 * TinyRTOS – Shared Lock Example
 * ATtiny85 with ATTinyCore (Spence Konde)
 *
 * Two tasks share both LEDs as a common resource.
 * Each task has a recognizable pattern:
 *
 *   taskChase – PB3/PB4 alternate (left–right chase)
 *   taskFlash – PB3/PB4 flash together (synchronized)
 *
 * rtos_lock() ensures each pattern completes without interruption.
 *
 * Try it: comment out rtos_lock() and rtos_unlock() in both tasks.
 * The patterns will overlap into unrecognizable chaos.
 *
 * Board settings (ATTinyCore):
 *   Board:             ATtiny25/45/85
 *   Chip:              ATtiny85
 *   Clock:             8 MHz (internal)
 *   millis()/micros(): Enabled
 *   BOD:               Disabled
 *   LTO:               Enabled
 *
 * Wiring:
 *   PB3 (Pin 2) – LED + resistor to GND
 *   PB4 (Pin 3) – LED + resistor to GND
 */

#include "TinyRTOS.h"

#define PIN_A 3 // PB3
#define PIN_B 4 // PB4

RtosLock led_lock = RTOS_LOCK_INIT; // Protects both LEDs

// Task 1: Chase – PB3 and PB4 alternate (3 cycles)
void taskChase() {
    pinMode(PIN_A, OUTPUT);
    pinMode(PIN_B, OUTPUT);
    while (true) {
        rtos_lock(&led_lock); // Acquire – wait if taskFlash is running

        for (uint8_t i = 0; i < 3; i++) {
            digitalWrite(PIN_A, HIGH); digitalWrite(PIN_B, LOW);
            rtos_delay(200);
            digitalWrite(PIN_A, LOW);  digitalWrite(PIN_B, HIGH);
            rtos_delay(200);
        }
        digitalWrite(PIN_A, LOW); digitalWrite(PIN_B, LOW);

        rtos_unlock(&led_lock); // Release – taskFlash may now proceed
        rtos_delay(400);
    }
}

// Task 2: Flash – PB3 and PB4 flash together (3 times)
void taskFlash() {
    while (true) {
        rtos_lock(&led_lock); // Acquire – wait if taskChase is running

        for (uint8_t i = 0; i < 3; i++) {
            digitalWrite(PIN_A, HIGH); digitalWrite(PIN_B, HIGH);
            rtos_delay(100);
            digitalWrite(PIN_A, LOW);  digitalWrite(PIN_B, LOW);
            rtos_delay(400);
        }

        rtos_unlock(&led_lock); // Release – taskChase may now proceed
        rtos_delay(200);
    }
}

void setup() {
    rtos_add_task(taskChase);
    rtos_add_task(taskFlash);

    rtos_run(); // Start scheduler – never returns
}

void loop() {
    // Will never be called
}
