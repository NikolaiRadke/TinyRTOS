/*
 * TinyRTOS – Cooperative Task Scheduler
 * Based on MiniRTOS by Nikolai Radke, 2026
 *
 * Footprint: ~600 bytes flash  |  10 bytes RAM + TINYRTOS_STACK_SIZE per task
 *
 * Supported MCUs:
 *  ATtiny44/45/84/85
 *
 * RAM usage example (ATtiny85):
 * 2 tasks × 64 bytes stack = 128 bytes
 * + kernel overhead        =  10 bytes
 * + program variables      =   5 bytes
 * ─────────────────────────────────────
*  Total                    = 143 bytes 
 *
 * Minimum stack size per task: 40 bytes kernel overhead
 *   + call depth + ~20 bytes for timer0 ISR (millis)
 *   64 bytes recommended for simple tasks without Serial
 *
 * Requirements: ATTinyCore (Spence Konde) with millis() support
 */

#pragma once
#include <stdint.h>

#ifndef TINYRTOS_MAX_TASKS
  #define TINYRTOS_MAX_TASKS   2  // max. number of tasks
#endif

#ifndef TINYRTOS_STACK_SIZE
  #define TINYRTOS_STACK_SIZE  64 // bytes per task stack
#endif

#if TINYRTOS_STACK_SIZE < 40
  #error "TINYRTOS_STACK_SIZE must be at least 40 bytes."
#endif

#if TINYRTOS_MAX_TASKS * TINYRTOS_STACK_SIZE > 150
  #warning "Stack usage > 150 bytes – watch RAM limits on ATtiny44/45 (256 bytes)!"
#endif

typedef void (*TaskFunc)(void);

// Register a task. Call before rtos_run()
void rtos_add_task(TaskFunc func);

// Start the scheduler. Never returns
// At least one task must be registered before calling.
void rtos_run(void);

// Cooperatively yield the CPU. Call from within a task
void rtos_yield(void);

// Wait ms milliseconds, yielding the CPU while waiting
void rtos_delay(uint16_t ms);
