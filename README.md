Please note: This is a toy project built for learning pupose.

# String Defined Call Routine library

<p align="center"><img width=20% src="./.github/a_random_led_panel.jfif"></p>


This library core idea is offers simple API to define routine in the time domain.
It can be useful to schedule simple tasks with complex timing. For example, a specific led-blink pattern.

The main idea is to use callback function to define the routine behavior and a string to define when this function will be called.

```c
// Define the routine.
//   `C` is for call the callback.
//   `.` is for wait.
char* redLedRoutine = ".C..C...";

// Create the routine.
sdcr_routine_new(
    .id = "red led",                    //< Use this to call the routine
    .routine = redLedRoutine,           //< Routine time domain definition
    .callbackFunction = toggle_red_led, //< Routine callback
    .routineStepTimeMs = 500            //< 
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
- I wanted to solve an old problem I encounter (more info in the [design document](DESIGN.md)).

If you want more design choices information, you can consult this [design document](DESIGN.md).

## How to build

It assume you have `cmake`, `make` and `gcc` installed.

```Shell
$ cd build
$ cmake ..
$ make
$ ./sdrc-main
```

## How to test

```Shell
$ # All tests
$ cd build
$ cmake ..
$ make test
```

```Shell
$ # Individual tests
$ cd build
$ cmake ..
$ make
$ ./unittest_interface # Test API solidity
$ ./unittest_behavior  # Test lib behavior
```

## License

BSD 3-Clause License.       
See the [license document](./LICENSE).
