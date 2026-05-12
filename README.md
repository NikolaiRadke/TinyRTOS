# TinyRTOS

Unleash the power of multitasking *FreeRTOS*-like (Real Time Operating System) for your tiny MCU!  
Well, kind of. TinyRTOS is a TinyRTOS is a **cooperative task scheduler** for **8-bit AVR MCUs**
with at least 256 bytes of RAM.

Enables multiple tasks to run seemingly simultaneously on the popular microcontrollers with as little as 256 bytes of RAM.  

🆕 What's new?  
* **11.05.2026** Release **V1.2.0** with experimental ATmega2560 support (3-byte PC).  
    -- More news? Check the [newsblog](https://github.com/NikolaiRadke/TinyRTOS/blob/main/NEWS.md).

## Supported MCUs

* **Tested:** Attiny45/85/44/84, ATmega328 (Arduino Uno/Nano).
  
* **Any other ATtiny MCU** with 32 general-purpose registers and at least 256 bytes of RAM
should be compatible (e.g. ATtiny461, ATtiny861). ATtinys with only 16 registers
(e.g. ATtiny13, ATtiny2313) or less than 256 Byte RAM (ATtiny25) are **not** supported.
   
* **Any other AVR MCU** with up to 128 KB flash should be compatible (e.g. ATmega88,
ATmega1284P, ATmega32U4) – the kernel uses only standard AVR instructions and registers.
  
* **Newer AVR Dx MCUs** (e.g. AVR128DA28) should be compatible via DxCore, but are not yet tested.
  
* **Experimental:**  ATmega2560 (No board available)
  
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

ATmega2560:
4 tasks × 192 bytes stack = 768 bytes
+ kernel overhead         =  10 bytes
─────────────────────────────────────
Total                     ≈ 778 bytes

```

Stack size and task count can be adjusted before including the library:

```cpp
#define TINYRTOS_MAX_TASKS   3
#define TINYRTOS_STACK_SIZE  96
#include "TinyRTOS.h"
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

## 💙 Support TinyRTOS

TinyRTOS is free and open source. If it saved your day, consider buying me a coffee! ☕

[![GitHub Sponsors](https://img.shields.io/github/sponsors/NikolaiRadke?style=for-the-badge&logo=github&color=ea4aaa)](https://github.com/sponsors/NikolaiRadke)
[![Ko-fi](https://img.shields.io/badge/Ko--fi-Buy%20me%20a%20coffee-FF5E5B?style=for-the-badge&logo=ko-fi&logoColor=white)](https://ko-fi.com/nikolairadke)

Every contribution helps keep this project alive! 🚀
