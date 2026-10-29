# V1.5.0

* First task runs on the Arduino stack – saves one full task stack (64 bytes RAM on ATtiny, 192 bytes on ATmega)
* Idle sleep is now race-free – rtos_delay() never sleeps a tick too long
* Tasks waiting in rtos_lock() no longer prevent idle sleep
* Context switch merged into rtos_yield(), shared millis() call – smaller kernel
