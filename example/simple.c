/* 
 * main 
 * An example of the String Defined Call Routine library
 * It use Linux timer to simulate a MCU environement.
 */
#include <stdio.h>
#include <stdint.h>
#include <time.h>

// library
#include "../src/sdrc.h"

// prototype
static void toggle_red_led();
static void toggle_green_led();
static uint32_t get_tick_count_ms();

// macro (quite unsafe, I must add...)
#define ESC_RED_CHAR    "[91m"
#define ESC_GREEN_CHAR  "[92m"
#define ESC_RESET       "[0m"

#define RED(str) \
    ESC_RED_CHAR str ESC_RESET

#define GREEN(str) \
    ESC_GREEN_CHAR str ESC_RESET

#define DEBUG_PRINT(str) \
    printf("%d - " str "\n", get_tick_count_ms());

// Function
int main()
{
    //
    // Example: Building two routines
    //
    sdcr_status res = 0;
    res += sdcr_routine_new(.id = "red led",
                            .routine = "C..",
                            .callbackFunction = toggle_red_led,
                            .routineStepTimeMs = 1000);

    res += sdcr_routine_new(.id = "green led",
                            .routine = "C..",
                            .callbackFunction = toggle_green_led,
                            .routineStepTimeMs = 1000);
    if (res != SDCR_SUCCESS)
    {
        return -1; // handle error
    }
    // 
    // Example: Starting the two routines
    //          The green led will perform 3 cycles
    //          The red led will perform cycles infinitely
    //
    res += sdcr_routine_start_for_n_cycles("green led", 3);
    res += sdcr_routine_start_inf("red led");
    if (res != SDCR_SUCCESS)
    {
        return -1; // handle error
    }

    //
    // Example: Starting the main loop
    //
    while (1)
    {
        sdcr_task(get_tick_count_ms);
    }
}

static void toggle_red_led()
{
    DEBUG_PRINT(RED("Toggle !")); // toggle led here
}

static void toggle_green_led()
{
    DEBUG_PRINT(GREEN("Toggle !")); // toggle led here
}

static uint32_t get_tick_count_ms()
{
    time_t now = time(NULL); //< only in second. But it doesnt matter for this example.
    uint32_t nowMs = (uint32_t)(now*1000);
    return nowMs;
}