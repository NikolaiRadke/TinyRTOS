# V1.1

* Merged with MiniRTOS – TinyRTOS now supports ATmega AVR MCUs with auto-detected defaults
* Added (untested) support for more classic ATtinys
* Added RtosLock **rtos_lock()** and **rtos_unlock()** to safely share resources between tasks. Zero overhead if unused. (Binary mutex)
* Added lock example **lockDemo.ino**
