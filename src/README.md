# TinyRTOS

Cooperative task scheduler for ATtiny44/45/84/85.

## Requirements

- ATTinyCore (Spence Konde)
- Board setting: millis()/micros() : Enabled

## API

| Function | Description |
|---|---|
| `rtos_add_task(func)` | Register a task (before `rtos_run()`) |
| `rtos_run()` | Start the scheduler |
| `rtos_yield()` | Yield the CPU cooperatively |
| `rtos_delay(ms)` | Wait and yield during the delay |

## Rules

1. Never use `delay()` – always use `rtos_delay()`
2. No long loops without `rtos_yield()` in between
3. Never call `rtos_yield()` from an ISR
4. Tasks must be infinite loops – returning causes undefined behavior

## License

Apache 2.0
