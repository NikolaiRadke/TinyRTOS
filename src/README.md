# TinyRTOS V1.1

Cooperative task scheduler with resource locks for classic ATtiny and ATmega AVR MCUs.

## Requirements

- ATTinyCore (Spence Konde) for classic ATtiny MCUs
- megaTinyCore (Spence Konde) for tinyAVR 0/1/2-series MCUs
- Arduino AVR Core for ATmega MCUs
- Board setting: millis()/micros() : Enabled

## API

| Function | Description |
|---|---|
| `rtos_add_task(func)` | Register a task (before `rtos_run()`) |
| `rtos_run()` | Start the scheduler |
| `rtos_yield()` | Yield the CPU cooperatively |
| `rtos_delay(ms)` | Wait and yield during the delay |
| `rtos_lock(lock)` | Lock a shared resource – waits if already in use |
| `rtos_unlock(lock)` | Release the lock |


## Rules

1. Never use `delay()` – always use `rtos_delay()`
2. No long loops without `rtos_yield()` in between
3. Never call `rtos_yield()` from an ISR
4. Always call `rtos_unlock()` – a missing unlock blocks the other task forever.
5. Tasks must be infinite loops – returning causes undefined behavior

## License

Apache 2.0
