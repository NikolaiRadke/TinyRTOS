/*
 * TinyRTOS V1.4.0 – Cooperative Task Scheduler
 * Nikolai Radke, 2026
 *
 * Footprint: ~350 bytes flash (kernel) | 3 bytes RAM + 2 bytes and TINYRTOS_STACK_SIZE per task
 *
 * Supported MCUs:
 * ATtiny44/45/84/85 and other ATtiny MCUs with 32 GPRs and ≥256 bytes RAM.
 * ATmega328P and any other AVR MCU with up to 128 KB flash.
 * ATmega2560/2561 (experimental, 3-byte PC).
 *
 * RAM usage example (ATtiny85):
 *   2 tasks × 64 bytes stack  = 128 bytes
 *   + kernel overhead         =   7 bytes
 *   Total                     ≈ 135 bytes
 *
 * RAM usage example (ATmega328):
 *   3 tasks × 192 bytes stack = 576 bytes
 *   + kernel overhead         =   9 bytes
 *   Total                     ≈ 585 bytes
 *
 * Minimum stack size per task: 20 bytes context frame
 *   + call depth + ~20 bytes for timer0 ISR (millis)
 *   64 bytes recommended for simple tasks without Serial
 *
 * Requirements: ATTinyCore (Spence Konde) for ATtiny, Arduino AVR Core for ATmega
 *               millis() support must be enabled */

#pragma once
//#define TINYRTOS_NO_IDLE_SLEEP  // Uncomment to disable idle sleep: frees ~55 bytes flash, raises current draw

#include <stdint.h>
#include <avr/io.h>

#if (RAMEND - RAMSTART + 1) <= 512
  #ifndef TINYRTOS_MAX_TASKS
    #define TINYRTOS_MAX_TASKS   2
  #endif
  #ifndef TINYRTOS_STACK_SIZE
    #define TINYRTOS_STACK_SIZE  64
  #endif
  #define _TINYRTOS_RAM_WARN  150
#elif FLASHEND > 0x1FFFF  // 3-byte PC: ATmega2560/2561
  #ifndef TINYRTOS_MAX_TASKS
    #define TINYRTOS_MAX_TASKS   4
  #endif
  #ifndef TINYRTOS_STACK_SIZE
    #define TINYRTOS_STACK_SIZE  192
  #endif
  #define _TINYRTOS_RAM_WARN  6000
  #define _TINYRTOS_PC3
#else  // ATmega and others
  #ifndef TINYRTOS_MAX_TASKS
    #define TINYRTOS_MAX_TASKS   3
  #endif
  #ifndef TINYRTOS_STACK_SIZE
    #define TINYRTOS_STACK_SIZE  192
  #endif
  #define _TINYRTOS_RAM_WARN  800
#endif

#ifndef TINYRTOS_NO_IDLE_SLEEP
  #define TINYRTOS_IDLE_SLEEP
#endif

#if TINYRTOS_STACK_SIZE < 40
  #error "TINYRTOS_STACK_SIZE must be at least 40 bytes."
#endif

#if TINYRTOS_MAX_TASKS * TINYRTOS_STACK_SIZE > _TINYRTOS_RAM_WARN
  #warning "Stack usage exceeds recommended limit – check available RAM!"
#endif

typedef void (*TaskFunc)(void);

// Register a task. Call before rtos_run()
void rtos_add_task(TaskFunc func);

// Start the scheduler. Never returns
// At least one task must be registered before calling.
void rtos_run(void);

// Cooperatively yield the CPU. Call from within a task.
// Always returns with interrupts enabled.
void rtos_yield(void);

// Wait ms milliseconds, yielding the CPU while waiting
void rtos_delay(uint16_t ms);

// Lock a shared resource – waits if already in use
typedef volatile uint8_t RtosLock;
#define RTOS_LOCK_INIT  0

void rtos_lock(RtosLock *lock);   // Wait until free, then lock
void rtos_unlock(RtosLock *lock); // Release lock
