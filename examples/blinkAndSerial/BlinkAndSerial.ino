/*
 * TinyRTOS – Blink and serial monitor example
 * Arduino Uno/Nano/Leonardo (ATmega328/32U4)
 *
 * Three tasks:
 *   taskBlink   – blinks the built-in LED at 1 Hz
 *   taskCounter – prints an incrementing counter to Serial every 500 ms
 *   taskInput   – waits for Serial input and echoes it back
 *
 * Open the Serial Monitor at 9600 baud to see the counter running.
 * Type anything and press Enter – the counter keeps going while you type.
 * The LED keeps blinking too.
 *
 * This demonstrates cooperative multitasking: all three tasks run
 * independently without blocking each other.
 *
 * Board settings:
 *   Board: Arduino Uno, Nano or Leonardo
 *   Baud:  9600
 *
 * RAM usage for TinyRTOS:
 *   3 tasks × 192 bytes stack = 576 bytes
 *   + kernel overhead         =  10 bytes
 *   Total                     ≈ 586 bytes 
 */

#include "TinyRTOS.h"

// Task 1: Blink built-in LED at 1 Hz
void taskBlink() {
    pinMode(LED_BUILTIN, OUTPUT);
    while (true) {
        digitalWrite(LED_BUILTIN, HIGH);
        rtos_delay(500);
        digitalWrite(LED_BUILTIN, LOW);
        rtos_delay(500);
    }
}

// Task 2: Print incrementing counter every 500 ms
void taskCounter() {
    uint32_t count = 0;
    while (true) {
        Serial.print("Count: ");
        Serial.println(count++);
        rtos_delay(500);
    }
}

// Task 3: Echo Serial input
void taskInput() {
    while (true) {
        if (Serial.available()) {
            char c = Serial.read();
            Serial.print("Echo: ");
            Serial.println(c);
        }
        rtos_yield();
    }
}

// Setup
void setup() {
    Serial.begin(9600);
    Serial.println("TinyRTOS Example – type anything!");

    rtos_add_task(taskBlink);
    rtos_add_task(taskCounter);
    rtos_add_task(taskInput);

    rtos_run(); // Start scheduler and never come back
}

void loop() {
    // Will never be called
}
