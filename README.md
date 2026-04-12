# TinyRTOS

Unleash the power of multitasking *FreeRTOS*-like (Real Time Operating System) for your tiny MCU!  
Well, kind of. TinyRTOS is a **cooperative task scheduler** for the **ATtiny44/45/84/85**.

Enables multiple tasks to run seemingly simultaneously on the popular microcontrollers with as little as 256 bytes of RAM.  

Using **Arduino UNO/Nano**? Take a look at MiniRTOS big sister: [MiniRTOS](https://github.com/NikolaiRadke/MiniRTOS).


🆕 What's new?  
* **11.04.2026** First release **V1.0.0**.    
    -- More news? Check the [newsblog](https://github.com/NikolaiRadke/FreeRTOS/blob/main/NEWS.md).

## Supported MCUs

| MCU | Flash | RAM | Package |
|---|---|---|---|
| ATtiny45 | 4 KB | 256 B | DIP-8 |
| ATtiny85 | 8 KB | 512 B | DIP-8 |
| ATtiny44 | 4 KB | 256 B | DIP-14 |
| ATtiny84 | 8 KB | 512 B | DIP-14 |

The kernel runs unchanged on all four MCUs. On ATtiny44/45 (256 bytes RAM), keep stack sizes small.

## Requirements

- Arduino IDE with **ATTinyCore** (Spence Konde)
- Board settings:
  - Chip: `ATtiny45`, `ATtiny85`, `ATtiny44` or `ATtiny84`
  - Clock: `8 MHz (internal)`
  - millis()/micros(): `Enabled`

## Quick Start

```cpp
#include "TinyRTOS.h"

void taskA() {
    pinMode(3, OUTPUT);
    while (true) {
        digitalWrite(3, HIGH); rtos_delay(500);
        digitalWrite(3, LOW);  rtos_delay(500);
    }
}

void taskB() {
    pinMode(4, OUTPUT);
    while (true) {
        digitalWrite(4, HIGH); rtos_delay(125);
        digitalWrite(4, LOW);  rtos_delay(125);
    }
}

void setup() {
    rtos_add_task(taskA);
    rtos_add_task(taskB);
    rtos_run(); // Never returns
}

void loop() {} // Will never run
```

PB3 and PB4 are free I/O pins used in the example sketch.

## API

| Function | Description |
|---|---|
| `rtos_add_task(func)` | Register a task (before `rtos_run()`) |
| `rtos_run()` | Start the scheduler |
| `rtos_yield()` | Yield the CPU cooperatively |
| `rtos_delay(ms)` | Wait and yield during the delay |

## RAM Usage

```
2 tasks × 64 bytes stack = 128 bytes
+ kernel overhead        =  10 bytes
+ program variables      =   5 bytes
─────────────────────────────────────
Total                    ≈ 143 bytes 
```

Stack size and task count can be adjusted in `TinyRTOS.h`:

```cpp
#define TINYRTOS_MAX_TASKS   2
#define TINYRTOS_STACK_SIZE  64
```

> ⚠️ Minimum: 40 bytes per task. Deep call stacks require more.

## What TinyRTOS Is (and Is Not)

TinyRTOS is a **cooperative task scheduler** – not an operating system. There is no memory protection, no process isolation, and no dynamic memory management. A misbehaving task can corrupt others and will not be caught.

On an ATtiny45 with 256 bytes of RAM this is not a limitation but the only sensible approach. TinyRTOS replaces nested state machines with readable, linear task code. Nothing more, nothing less.  

So why call it an **RTOS?** Because *TinyCooperativeRoundRobinTaskScheduler* or *TinyCRRTS* sounds really weird. And FreeRTOS calls itself an RTOS too – it's a preemptive scheduler with considerably more RAM, a much longer feature list and better marketing. But on a chip so tiny you can lose it between your keyboard keys, *RTOS*feels just about right.

## The Four Rules

> [!NOTE]
> TinyRTOS relies on programming discipline instead of preemptive interruption. Alas.

1. **No `delay()`** – always use `rtos_delay()`
2. **No long loops** without `rtos_yield()` in between
3. **Never call `rtos_yield()`** from an ISR
4. **Tasks must be infinite loops** – returning causes undefined behavior

## License

Apache 2.0 – free to use, including commercially.
