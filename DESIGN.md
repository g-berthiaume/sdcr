# DESIGN : String Defined Call Routine library

## What this library is trying to solve

While working on a bare-metal embedded platform, I encounter an UX problem.
The client wanted a 7 LED interfaces that would blink in different pattern depending on the device state.
Having to manage all the state machines for those LED behaviour could be complex, and more importantly, the client could change the design at any moment.

Funny to have the problem of a UX designer, as a bare metal programmer.  

A library that could **define a routine with a simple string API** could be handy...

```c
// Define the routine
char* redLedRoutine = ".CC...";

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

## design choices

### Timing accuracy

This library is non-blocking, meaning it will not use any blocking function (like `sleep`). Instead, this library rely's on a user-implementation of a `get_tick_ms()` function to do its scheduling.

This library rely the periodic call of the `sdcr_task()` to perform it's time sensitive computation. The `sdcr_task()` call frequency and consistency will define the precision of the library.
Therefore, this library **should not be use for time critical applications**.

### `Malloc`less library vs global variables

While it would be certainly useful, this library won't use `malloc`, as some embedded standard forbid it (like the _MISRA-C_ standards).
To maintained the encapsulation without using `malloc`, internal global variables are defined. The memory footprint is define with `SDCR_MAX_NUMBER_OF_ROUTINE = 10` in the `.h`.
If you have a more elegant solution, I would like to hear it.

### Clean API

This library tries to be easy to use while being flexible.

#### 1. Using string to define routine

Usually you could use the duty-cycle and the time period to produce a simple pattern. But what if you want to define a _3 rapid blinks followed by a long pause_ pattern ?

This library solve this problem by its simple string base routine definition. Inspired by REGEX, each character in the string routine represent an action to do.

- The `C` character represent a callback function call. 
- The `.` character represent no action. 

After performing the first action in the string, the library must wait a defined time period before performing the next action. The time to wait between action is defined as `.timebetweenActionMs = 100`.

Let's interpret the following routine configuration :

```C
sdcr_routine_configuration my_config = {
    .id = "Green led",
    .routine = "C...",
    .callbackFunction = toggle_green_led,
    .routineStepTimeMs = 100
};
```

The chronogram of `"C..."` would look like this

```txt
    CHRONOGRAM of my_config routine
    --------------------------------


    Start                                   The pattern restart
    |                   |< 100ms >|         |
    V                   ¦         ¦         v
         C         .    ¦     .   ¦     .        C         .
    |-------------------|---------|---------|---------|---------|--> t
     ^                                       ^
     |                                       |
     Callback function is called             Callback function is called

```

#### 2. The ID system

The ID system make the library easy to use without the need to pass structures around.
When creating a new pattern, user give it an `char*` ID. As this ID is an inline string, ID will always be unique and available **in every scopes**.

```C
// create
sdcr_routine_new_base(my_config)

// use
sdcr_routine_stop("Green led");

// use it in some scope
void foo()
{
    SDCR_pattern_stop("Green led");
}
```

### Platform agnostic

This library is fully encapsulated. The only call to the HAL (Hardware Abstraction Layer) is done by a user-defined callback function to get tick.

For example, this function is defined as `__weak uint32_t HAL_GetTick(void)` on stm32xx HAL drivers.
