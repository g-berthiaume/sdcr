/*

*/
//
#include <stdio.h> //todo clean
#include <stdint.h>
#include <stdbool.h>
#include <time.h>

// library
#include "sdrc.h"

#define DEBUG_PRINT(str) \
    printf("%d - " str, get_tick_count_ms()) //TODO Delete

// prototype
static void toggle_red_led();
static void toggle_green_led();
static uint32_t get_tick_count_ms();

// Main
int main()
{
    // Example
    sdcr_status res = 0;
    res += sdcr_routine_new(.id = "red led",
                            .routine = "C..",
                            .callbackFunction = toggle_red_led,
                            .routineStepTimeMs = 1);

    res += sdcr_routine_new(.id = "green led",
                            .routine = "C..",
                            .callbackFunction = toggle_green_led,
                            .routineStepTimeMs = 1);
    if (res != SDCR_SUCCESS)
    {
        return -1; // handle error
    }

    res += sdcr_routine_start_for_n_cycles("green led", 2);
    res += sdcr_routine_start_inf("red led");
    if (res != SDCR_SUCCESS)
    {
        return -1; // handle error
    }

    while (1)
    {
        sdcr_task(get_tick_count_ms);
    }
    return 0;
}

static void toggle_red_led()
{
    DEBUG_PRINT("[91mRED[0m\n"); // toggle led here
}

static void toggle_green_led()
{
    DEBUG_PRINT("[92mGREEN[0m\n"); // toggle led here
}

/// Returns the number of ticks since an undefined time (usually system startup).
static uint32_t get_tick_count_ms()
{
    time_t now = time(NULL);
    return (uint32_t)now;
}