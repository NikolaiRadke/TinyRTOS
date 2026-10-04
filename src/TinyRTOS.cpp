/*
 * TinyRTOS.cpp – Cooperative kernel for AVR MCUs
 * Nikolai Radke, 2026
 *
 * Context switch
 * rtos_yield() is only ever called from C code, never from an ISR. So the
 * avr-gcc calling convention applies: r0, r18–r27, r30 and r31 are
 * call-clobbered, r1 is always 0. Only the call-saved registers r2–r17,
 * r28 and r29 must survive a switch – 18 bytes plus the return address.
 *
 * Frame of a suspended task, upwards from the saved SP:
 *   +1..+18 : r29, r28, r17 ... r2
 *   +19..   : return address (PCE first on 3-byte-PC MCUs, then PCH, PCL)
 *
 * SREG is not saved: tasks always run with interrupts enabled. The SP is
 * switched with "cli / out SPH / sei / out SPL". SEI takes effect after the
 * next instruction, so the 16-bit SP write stays atomic.
 *
 * Global variables use __attribute__((used)) because they are referenced
 * only from inline assembly.
 */

#include "TinyRTOS.h"
#include <avr/io.h>
#include <Arduino.h>

uint8_t  _mr_stk[TINYRTOS_MAX_TASKS][TINYRTOS_STACK_SIZE] __attribute__((used));
uint8_t *_mr_spt[TINYRTOS_MAX_TASKS]                      __attribute__((used));
uint8_t  _mr_n   __attribute__((used)) = 0;
uint8_t  _mr_cur __attribute__((used)) = 0;

#ifdef _TINYRTOS_PC3
  #define _MR_FRAME 21  // 18 registers + PCE + PCH
#else
  #define _MR_FRAME 20  // 18 registers + PCH
#endif

// Build a fake frame so _mr_switch + RET jumps into the task.
// _mr_stk lives in .bss and is zeroed at startup: the register start
// values don't matter for a fresh task, PCE (3-byte PC) is already 0.

static void initStack(uint8_t idx, TaskFunc func) {
    uint8_t *sp = &_mr_stk[idx][TINYRTOS_STACK_SIZE - 1 - _MR_FRAME];
    sp[_MR_FRAME]     = (uint16_t)func & 0xFF;  // PCL
    sp[_MR_FRAME - 1] = (uint16_t)func >> 8;    // PCH
    _mr_spt[idx] = sp;
}

void __attribute__((noinline)) rtos_add_task(TaskFunc func) {
    if (_mr_n < TINYRTOS_MAX_TASKS)
        initStack(_mr_n++, func);
}

// Load SP from *X, restore registers and return into the task.
// Jumped to (not called) by rtos_yield() and rtos_run().

extern "C" void __attribute__((naked, used)) _mr_switch(void) {
    asm volatile (
        "ld   r24, X+                   \n\t"
        "ld   r25, X                    \n\t"
        "cli                            \n\t"
        "out  __SP_H__, r25             \n\t"
        "sei                            \n\t" // effective after next instruction
        "out  __SP_L__, r24             \n\t"
        "pop  r29                       \n\t"
        "pop  r28                       \n\t"
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
        "ret                            \n\t"
        ::: "memory"
    );
}

// Context switch:
//   1. Push call-saved registers onto the current task stack
//   2. Store SP in _mr_spt[_mr_cur] – X then points to the next entry
//   3. Advance _mr_cur round robin, reset X on wrap-around
//   4. Jump to _mr_switch

void __attribute__((naked, used)) rtos_yield(void) {
    asm volatile (
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
        "push r28                       \n\t"
        "push r29                       \n\t"
        "lds  r25, _mr_cur              \n\t"
        "ldi  r26, lo8(_mr_spt)         \n\t"
        "ldi  r27, hi8(_mr_spt)         \n\t"
        "add  r26, r25                  \n\t"
        "adc  r27, __zero_reg__         \n\t"
        "add  r26, r25                  \n\t"
        "adc  r27, __zero_reg__         \n\t"
        "in   r24, __SP_L__             \n\t"
        "st   X+, r24                   \n\t"
        "in   r24, __SP_H__             \n\t"
        "st   X+, r24                   \n\t" // X -> _mr_spt[_mr_cur + 1]
        "inc  r25                       \n\t"
        "lds  r24, _mr_n                \n\t"
        "cp   r25, r24                  \n\t"
        "brlo 1f                        \n\t"
        "clr  r25                       \n\t" // wrap around to task 0
        "ldi  r26, lo8(_mr_spt)         \n\t"
        "ldi  r27, hi8(_mr_spt)         \n\t"
        "1:                             \n\t"
        "sts  _mr_cur, r25              \n\t"
        "%~jmp _mr_switch               \n\t"
        ::: "r18", "r19", "r20", "r21", "r22", "r23", "r24", "r25",
            "r26", "r27", "r30", "r31", "memory"
    );
}

// Point X at task 0 and jump to _mr_switch.
// No save needed – the Arduino stack is abandoned from here.

void __attribute__((naked)) rtos_run(void) {
    asm volatile (
        "ldi  r26, lo8(_mr_spt)         \n\t"
        "ldi  r27, hi8(_mr_spt)         \n\t"
        "%~jmp _mr_switch               \n\t"
        ::: "memory"
    );
}

#ifdef TINYRTOS_IDLE_SLEEP
static uint8_t _mr_wait = 0; // Number of tasks waiting in rtos_delay()

#ifdef SLPCTRL  // tinyAVR 0/1/2, AVR Dx: dedicated sleep controller
static inline void _mr_idle(void) {
    SLPCTRL.CTRLA = SLPCTRL_SMODE_IDLE_gc | SLPCTRL_SEN_bm;
    __asm__ __volatile__ ("sleep");
    SLPCTRL.CTRLA = 0;
}
#else           // Classic AVR: SMCR (ATmega) or MCUCR (ATtiny)
#ifdef SMCR
  #define _MR_SLEEP_REG  SMCR
#else
  #define _MR_SLEEP_REG  MCUCR
#endif
#ifdef SM2
  #define _MR_SM_MASK ((uint8_t)(_BV(SM0) | _BV(SM1) | _BV(SM2)))
#else
  #define _MR_SM_MASK ((uint8_t)(_BV(SM0) | _BV(SM1)))
#endif
static inline void _mr_idle(void) {
    _MR_SLEEP_REG = (_MR_SLEEP_REG & ~_MR_SM_MASK) | _BV(SE); // SM = 000: idle
    __asm__ __volatile__ ("sleep");
    _MR_SLEEP_REG &= (uint8_t)~_BV(SE);
}
#endif
#endif

// 16-bit arithmetic is exact here: ms is 16 bit, the unsigned
// subtraction handles the wrap-around of millis().
// Idle sleep happens only in task 0's turn: after each tick every task
// checks its deadline once before the CPU sleeps again.

void rtos_delay(uint16_t ms) {
    uint16_t start = (uint16_t)millis();
#ifdef TINYRTOS_IDLE_SLEEP
    _mr_wait++;
#endif
    while ((uint16_t)((uint16_t)millis() - start) < ms) {
#ifdef TINYRTOS_IDLE_SLEEP
        if (_mr_wait == _mr_n && _mr_cur == 0) _mr_idle(); // once per round
#endif
        rtos_yield();
    }
#ifdef TINYRTOS_IDLE_SLEEP
    _mr_wait--;
#endif
}

void rtos_lock(RtosLock *lock) {
    while (*lock)
        rtos_yield();
    *lock = 1;
}

void rtos_unlock(RtosLock *lock) {
    *lock = 0;
}
