# V1.4.0

* Context switch saves only call-saved registers – kernel ~30 % smaller, 15 bytes less stack per task
* Fixed idle sleep making rtos_delay() overshoot by up to one tick per task
* rtos_yield() now always returns with interrupts enabled
