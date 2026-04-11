/*
 * TinyRTOS.cpp – Cooperative kernel, ATtiny44/45/84/85
 * Based on MiniRTOS by Nikolai Radke, 2026
 *
 * The AVR instruction set of the ATtiny44/45/84/85 is identical to the
 * ATmega328: all 32 registers, same stack mechanism, 2-byte program counter.
 *
 * Important: ICALL requires a word address (byte address / 2).
 * Use pm_lo8/pm_hi8 instead of lo8/hi8 for function addresses in ICALL.
 *
 * Global variables use __attribute__((used)) to prevent the linker from
 * removing symbols that are only referenced in inline assembly.
 */

#include "TinyRTOS.h"
#include <avr/io.h>
#include <string.h>
#include <Arduino.h>

uint8_t  _mr_stk[TINYRTOS_MAX_TASKS][TINYRTOS_STACK_SIZE] __attribute__((used));
uint8_t *_mr_spt[TINYRTOS_MAX_TASKS]                      __attribute__((used));
uint8_t  _mr_n   __attribute__((used)) = 0;
uint8_t  _mr_cur __attribute__((used)) = 0;

extern "C" void __attribute__((naked, used)) _mr_schedule(void) {
    asm volatile (
        "lds  r24, _mr_cur          \n\t"
        "inc  r24                   \n\t"
        "lds  r25, _mr_n            \n\t"
        "cp   r24, r25              \n\t"
        "brlo .+2                   \n\t"  // skip if r24 < r25
        "clr  r24                   \n\t"
        "sts  _mr_cur, r24          \n\t"
        "ret                        \n\t"
        ::: "memory"
    );
}

// Build a fake context frame so _mr_restore + RET jumps into the task.
//
// Stack layout from _mr_spt (offsets +1..+35):
//   +1..+31 : r31..r1 = 0
//   +32     : r0 (as SREG) = 0x80 (I-bit set)
//   +33     : r0 original  = 0
//   +34     : PCH = func >> 8
//   +35     : PCL = func & 0xFF

static void initStack(uint8_t idx, TaskFunc func) {
    uint8_t *s   = _mr_stk[idx];
    uint16_t top = TINYRTOS_STACK_SIZE - 1;

    memset(s, 0, TINYRTOS_STACK_SIZE);

    s[top]     = (uint16_t)func & 0xFF;
    s[top - 1] = ((uint16_t)func >> 8) & 0xFF;
    s[top - 2] = 0;
    s[top - 3] = 0x80;

    _mr_spt[idx] = &s[top - 35];
}

void rtos_add_task(TaskFunc func) {
    if (_mr_n < TINYRTOS_MAX_TASKS)
        initStack(_mr_n++, func);
}

// Shared restore sequence – jumped to (not called) by rtos_yield and rtos_run.
// Pops all registers from the current task stack, restores SREG, and returns
// into the next task via RET.

extern "C" void __attribute__((naked, used)) _mr_restore(void) {
        asm volatile (
        "pop  r31                       \n\t"
        "pop  r30                       \n\t"
        "pop  r29                       \n\t"
        "pop  r28                       \n\t"
        "pop  r27                       \n\t"
        "pop  r26                       \n\t"
        "pop  r25                       \n\t"
        "pop  r24                       \n\t"
        "pop  r23                       \n\t"
        "pop  r22                       \n\t"
        "pop  r21                       \n\t"
        "pop  r20                       \n\t"
        "pop  r19                       \n\t"
        "pop  r18                       \n\t"
        "pop  r17                       \n\t"
        "pop  r16                       \n\t"
        "pop  r15                       \n\t"
        "pop  r14                       \n\t"
        "pop  r13                       \n\t"
        "pop  r12                       \n\t"
        "pop  r11                       \n\t"
        "pop  r10                       \n\t"
        "pop  r9                        \n\t"
        "pop  r8                        \n\t"
        "pop  r7                        \n\t"
        "pop  r6                        \n\t"
        "pop  r5                        \n\t"
        "pop  r4                        \n\t"
        "pop  r3                        \n\t"
        "pop  r2                        \n\t"
        "pop  r1                        \n\t"
        "pop  r0                        \n\t"
        "out  __SREG__, r0              \n\t"
        "pop  r0                        \n\t"
        "ret                            \n\t"
        ::: "memory"
    );
}

// Context switch:
//   1. Save all registers + SREG onto current task stack (33 pushes)
//   2. Save SP to _mr_spt[_mr_cur]
//   3. Call _mr_schedule() to update _mr_cur
//   4. Load SP from _mr_spt[_mr_cur]
//   5. Jump to _mr_restore → pops all registers, RET into next task

void __attribute__((naked)) rtos_yield(void) {
    asm volatile (
        "push r0                        \n\t"
        "in   r0, __SREG__              \n\t"
        "cli                            \n\t"
        "push r0                        \n\t"
        "push r1                        \n\t"
        "clr  r1                        \n\t"
        "push r2                        \n\t"
        "push r3                        \n\t"
        "push r4                        \n\t"
        "push r5                        \n\t"
        "push r6                        \n\t"
        "push r7                        \n\t"
        "push r8                        \n\t"
        "push r9                        \n\t"
        "push r10                       \n\t"
        "push r11                       \n\t"
        "push r12                       \n\t"
        "push r13                       \n\t"
        "push r14                       \n\t"
        "push r15                       \n\t"
        "push r16                       \n\t"
        "push r17                       \n\t"
        "push r18                       \n\t"
        "push r19                       \n\t"
        "push r20                       \n\t"
        "push r21                       \n\t"
        "push r22                       \n\t"
        "push r23                       \n\t"
        "push r24                       \n\t"
        "push r25                       \n\t"
        "push r26                       \n\t"
        "push r27                       \n\t"
        "push r28                       \n\t"
        "push r29                       \n\t"
        "push r30                       \n\t"
        "push r31                       \n\t"
        "lds  r24, _mr_cur              \n\t"
        "lsl  r24                       \n\t"
        "clr  r25                       \n\t"
        "ldi  r26, lo8(_mr_spt)         \n\t"
        "ldi  r27, hi8(_mr_spt)         \n\t"
        "add  r26, r24                  \n\t"
        "adc  r27, r25                  \n\t"
        "in   r24, __SP_L__             \n\t"
        "in   r25, __SP_H__             \n\t"
        "st   x+, r24                   \n\t"
        "st   x,  r25                   \n\t"
        "ldi  r30, pm_lo8(_mr_schedule) \n\t"
        "ldi  r31, pm_hi8(_mr_schedule) \n\t"
        "icall                          \n\t"
        "lds  r24, _mr_cur              \n\t"
        "lsl  r24                       \n\t"
        "clr  r25                       \n\t"
        "ldi  r26, lo8(_mr_spt)         \n\t"
        "ldi  r27, hi8(_mr_spt)         \n\t"
        "add  r26, r24                  \n\t"
        "adc  r27, r25                  \n\t"
        "ld   r24, x+                   \n\t"
        "ld   r25, x                    \n\t"
        "out  __SP_L__, r24             \n\t"
        "out  __SP_H__, r25             \n\t"
        "rjmp _mr_restore               \n\t"  // restore registers and RET into next task
        ::: "memory"
    );
}

// Load SP from task 0 and jump to _mr_restore.
// No save needed – the Arduino stack is abandoned from here.

void __attribute__((naked)) rtos_run(void) {
    asm volatile (
        "cli                            \n\t"
        "ldi  r26, lo8(_mr_spt)         \n\t"
        "ldi  r27, hi8(_mr_spt)         \n\t"
        "ld   r24, x+                   \n\t"
        "ld   r25, x                    \n\t"
        "out  __SP_L__, r24             \n\t"
        "out  __SP_H__, r25             \n\t"
        "rjmp _mr_restore               \n\t"  // restore registers and RET into task 0
        ::: "memory"
    );
}

void rtos_delay(uint16_t ms) {
    uint32_t start = millis();
    while (millis() - start < ms)
        rtos_yield();
}