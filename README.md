# TinyRTOS

Unleash the power of multitasking *FreeRTOS*-like (Real Time Operating System) for your tiny MCU!  
Well, kind of. TinyRTOS is a **cooperative task scheduler** for **ATtiny and ATmega AVR MCUs** 
with at least 256 bytes of RAM.

Enables multiple tasks to run seemingly simultaneously on the popular microcontrollers with as little as 256 bytes of RAM.  

🆕 What's new?  
* **10.05.2026** Release **V1.1.0** merged with **MiniRTOS**, with resource locks and support for more MCUs. 
    -- More news? Check the [newsblog](https://github.com/NikolaiRadke/TinyRTOS/blob/main/NEWS.md).

## Supported MCUs

### Tested

| MCU | Flash | RAM | Package |
|---|---|---|---|
| ATtiny45 | 4 KB | 256 B | DIP-8 |
| ATtiny85 | 8 KB | 512 B | DIP-8 |
| ATtiny44 | 4 KB | 256 B | DIP-14 |
| ATtiny84 | 8 KB | 512 B | DIP-14 |
| ATmega168 | 16 KB | 1 KB | DIP-28 |
| ATmega328/P | 32 KB | 2 KB | DIP-28 |
| ATmega32U4 | 32 KB | 2.5 KB | QFP-44 |

Requires **ATTinyCore** (Spence Konde) for ATtiny MCUs, **Arduino AVR Core** for ATmega MCUs.

### Supported but untested

| MCU | Flash | RAM | Package |
|---|---|---|---|
| ATtiny441 | 4 KB | 256 B | DIP-14 |
| ATtiny841 | 8 KB | 512 B | DIP-14 |
| ATtiny461 | 4 KB | 256 B | DIP-20 |
| ATtiny861 | 8 KB | 512 B | DIP-20 |
| ATtiny4313 | 4 KB | 256 B | DIP-20 |

Requires **ATTinyCore** (Spence Konde).

### Supported but untested (SMD only)

| Series | Example | Flash | RAM | Package |
|---|---|---|---|---|
| tinyAVR 0/1/2 – 8-pin (x02, x12) | ATtiny412 | 4–8 KB | 256–512 B | SOIC-8 |
| tinyAVR 0/1/2 – 14-pin (x04, x14, x24) | ATtiny1614 | 4–16 KB | 256 B–2 KB | SOIC-14 |
| tinyAVR 0/1/2 – 20-pin (x06, x16, x26) | ATtiny3216 | 4–32 KB | 256 B–2 KB | SOIC-20 |
| tinyAVR 0/1/2 – 24-pin (x07, x17, x27) | ATtiny3217 | 8–32 KB | 512 B–3 KB | SOIC-24 |

Requires **megaTinyCore** (Spence Konde). MCUs with less than 256 bytes RAM not supported.

## Requirements
- Arduino IDE with **ATTinyCore** (Spence Konde) for classic ATtiny MCUs
- Arduino IDE with **megaTinyCore** (Spence Konde) for tinyAVR 0/1/2-series MCUs
- Arduino IDE with **Arduino AVR Core** for ATmega MCUs
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
| `rtos_lock(lock)` | Lock a shared resource – waits if already in use |
| `rtos_unlock(lock)` | Release the lock |

## RAM Usage

```
ATtiny:
2 tasks × 64 bytes stack = 128 bytes
+ kernel overhead        =  10 bytes
+ program variables      =   5 bytes
────────────────────────────────────
Total                    ≈ 143 bytes 

ATmega328:
3 tasks × 192 bytes stack = 576 bytes
+ kernel overhead         =  10 bytes
─────────────────────────────────────
Total                     ≈ 586 bytes
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

So why call it an **RTOS?** Because *TinyCooperativeRoundRobinTaskScheduler* or *TinyCRRTS* sounds really weird. And FreeRTOS calls itself an RTOS too – it's a preemptive scheduler with considerably more RAM, a much longer feature list and better marketing. But on a chip so tiny you can lose it between your keyboard keys, *RTOS* feels just about right.

## The Five Rules

> [!NOTE]
> TinyRTOS relies on programming discipline instead of preemptive interruption. Alas.

1. Never use `delay()` – always use `rtos_delay()`
2. No long loops without `rtos_yield()` in between
3. Never call `rtos_yield()` from an ISR
4. Always call `rtos_unlock()` – a missing unlock blocks the other task forever.
5. Tasks must be infinite loops – returning causes undefined behavior

## License

Apache 2.0 – free to use, including commercially.

## 💙 Support AI.duino  

TinyRTOS is free and open source. If it saved your day, consider buying me a coffee! ☕

[![GitHub Sponsors](https://img.shields.io/github/sponsors/NikolaiRadke?style=for-the-badge&logo=github&color=ea4aaa)](https://github.com/sponsors/NikolaiRadke)
[![Ko-fi](https://img.shields.io/badge/Ko--fi-Buy%20me%20a%20coffee-FF5E5B?style=for-the-badge&logo=ko-fi&logoColor=white)](https://ko-fi.com/nikolairadke)

Every contribution helps keep this project alive! 🚀
