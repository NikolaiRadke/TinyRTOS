# V1.1

* Merged with MiniRTOS – TinyRTOS now supports ATmega and tinyAVR 0/1/2-series MCUs with auto-detected defaults
* Added RtosLock **rtos_lock()** and **rtos_unlock()** to safely share resources between tasks. Zero overhead if unused. (Binary mutex)
* Added lock example **lockDemo.ino**
