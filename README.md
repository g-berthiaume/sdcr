# String Defined Call Routine library

![A random LED panel](./.github/a_random_led_panel.jfif)

This library core idea is offers simple API to define routine in the time domain.
It can be useful to schedule simple tasks with complex timing. For example, a specific led blink pattern.

```c
// Define the routine
char* redLedRoutine = ".C..C...";

// Create the routine.
sdcr_routine_new(
    .id = "red led",
    .routine = redLedRoutine,
    .callbackFunction = toggle_red_led,
    .routineStepTimeMs = 500
);

// Now you can start or stop the routine in any place or scope
// in your code with a simple id !
sdcr_routine_start_inf("red led");

// Note that `sdcr_routine_start_inf` doesnt execute the routine.
// The work is done by the `sdcr_task` function in your infinit loop.
while (1)
{
    sdcr_task(get_tick_count_ms);
}
```

This C library is an experiment born from these desires:

- I wanted to learn how to use `cmake`.
- I wanted to test an idea for an API that would use inline strings as a Unique ID system.

If you want more information on the design choice, you can consult this [design document](DESIGN.md)

## How to build

Assume you have `cmake`, `make` and `gcc` installed.

```Shell
$ cd build
$ cmake ..
$ make
$ ./sdrc-main
```

## How to test

```Shell
$ cd build
$ cmake ..
$ make test
```

