/* 
 * srdc.h
 * String Defined Call Routine library
 * 
 * USAGE:
 *      // First define your callback function­.
 *      // For example, if you want to control a LED.
 *      void toggle_red_led();
 *      
 *      // Then create a call pattern.
 *      // Let say your team UX designer wants the folowing led blink pattern:
 *      // 1. off for   500 ms
 *      // 2. on  for   500 ms
 *      // 3. off for  2000 ms
 *      // 4. repeat
 *      //
 *      // Translated in state machine, this request looks like this:
 *      // 1. wait for                   500 ms
 *      // 2. toggle led and wait for    500 ms 
 *      // 3. toggle led and wait for    500 ms
 *      // 4. wait for                   500 ms
 *      // 5. wait for                   500 ms
 *      // 6. wait for                   500 ms
 *      // 7. repeat     
 *      //
 *      // Described in a string call routine :
 *      char* redLedRoutine = ".CC...";   //< Each char are 500 ms
 * 
 *      // We can now create the routine.
 *      sdcr_routine_new(.id = "red led",
 *                       .routine = redLedRoutine,
 *                       .callbackFunction = toggle_red_led,
 *                       .routineStepTimeMs = 500);
 *
 *      // Now you can start or stop the routine in any place or scope
 *      // in your code.
 *      sdcr_routine_start_inf("red led");
 * 
 *      // note that `sdcr_routine_start_inf` doesn't execute the routine.
 *      // The work is done by the `sdcr_task` function in your infinite loop.
 *      while (1)
 *      {
 *           sdcr_task(get_tick_count_ms);
 *      }
 * 
 * 
 * Copyright (c) 2019 G.Berthiaume, All rights reserved.
 * BSD 3-Clause License
 */
#ifndef _SDCR_H_
#define _SDCR_H_

//-----------------------------------------------
// INCLUDES
//-----------------------------------------------

#include <stdint.h>

//-----------------------------------------------
// USER CONFIG
//-----------------------------------------------

/* This library internal memory size definition.
 * If user plan to use more than 10 routines at the same time, 
 * he/she should modify this definition.
 */
#ifndef SDCR_MAX_NUMBER_OF_ROUTINE
#define SDCR_MAX_NUMBER_OF_ROUTINE 10
#endif

//-----------------------------------------------
// DEFINITIONS
//-----------------------------------------------

/* User defined callback function.
 * This function will be called following the 
 * routine definition.
 */
typedef void (*sdcr_callback_function)(void);

/* User defined callback function to get time in ms
 * since the start of the plateform.
 * The user HAL should already provide this kind of 
 * function. For example, `HAL_getTick()` on stm32 or
 * `millis()` on arduino.
 * return: time in ms.
 */
typedef uint32_t (*sdcr_get_tick_function)(void);

/* Enumarates all possible sdcr return messages.
 * !0 value are errors.
 */
typedef enum
{
    SDCR_SUCCESS = 0,                    //< No error.
    /* ERROR - General */
    SDCR_ERROR_NULL_PTR,                 //< Error: User should check if `id` or fct_ptr are valid.
    /* ERROR - Building new routine */
    SDCR_ERROR_ID_ALREADY_EXIST,         //< Error: User need to create a routine with unique `id` string.
    SDCR_ERROR_INVALID_ROUTINE_CONFIG,   //< Error: User should check if its routine configuration are valid.
    SDCR_ERROR_ROUTINE_MEMORY_IS_FULL,   //< Error: User tried to create more than `SDCR_MAX_NUMBER_OF_ROUTINE` routine.
    /* ERROR - Using routine */
    SDCR_ERROR_ID_DOESNT_EXIST,          //< Error: User tried to use a non-created routine. Check `id` for typos.
    SDCR_ERROR_INVALID_API_USAGE,        //< Error: User tried to use the API with invalid parameter.
} sdcr_status;

/* routine configurations
 * User will use this structure to configure the routine behavior.
 */
typedef struct
{
    const char *id;                          //< The routine ID, defined as an inline string.
    char *routine;                           //< The routine, defined as an inline string.
    uint32_t routineStepTimeMs;              //< The time for each step (char) in the routine,
                                             //  defined as a uint32_t in millisecond (ms).
    sdcr_callback_function callbackFunction; //< The callback function that the routine will call,
                                             //  defined as an function ptr.
} sdcr_routine_configuration;

//-----------------------------------------------
// API
//-----------------------------------------------

/* Will do the call routine management.
 * This function should be called periodically to ensure
 * good response time for the callbacks.
 * param:   getTickMs - function_ptr to a function that return
 *                      the number of milliseconds elapsed since startup.
 *                      See `sdcr_get_tick_function` for more info. 
 * return:  A sdcr status. 0 is success. 
 */
sdcr_status sdcr_task(sdcr_get_tick_function getTickMs);

/* Will create a new routine with the configuration.
 * note: The maximal number of routine is define in `sdcr_MAX_NUMBER_OF_routine`. 
 * note: See `sdcr_routine_new` for cleaner api.
 * note: All structure field shall be defined.
 * param: config - a structure that will define the routine behavior.
 * return: A sdcr status. 0 is success. 
 */
sdcr_status sdcr_routine_new_base(sdcr_routine_configuration config);

/* Will create a new routine with the configuration.
 * note: All structure field shall be defined.
 * param: config - a structure that will define how the call routine routine.
 * return: A sdcr status. 0 is success.
 * usage: 
 *      sdcr_routine_new(.id = "red led",
 *                       .routine = redLedRoutine,
 *                       .callbackFunction = toggle_red_led,
 *                       .routineStepTimeMs = 500);
 */
#define sdcr_routine_new(...) sdcr_routine_new_base((sdcr_routine_configuration){__VA_ARGS__});

/* Will clear a routine from memory.
 * The routine wont exist anymore and a new routine can
 * replace it in the library allocated memory.
 * note: This function complementaty to `sdcr_routine_new`.
 * param: The routine id, an unique inline string.
 * return: A sdcr status. 0 is success. 
 */
sdcr_status sdcr_routine_clear(const char *id);

/* Will start a routine that will cycle infinitely.
 * param: The routine id, an unique inline string.
 * return: A sdcr status. 0 is success. 
 */
sdcr_status sdcr_routine_start_inf(const char *id);

/* Will start a routine for a specific number of cycle.
 * param: The routine id, an unique inline string.
 * param: The number of cycle.
 * return: A sdcr status. 0 is success. 
 */
sdcr_status sdcr_routine_start_for_n_cycles(const char *id, uint16_t n);

/* Will stop a routine.
 * note: This function complementaty to `sdcr_routine_start_inf`.
 * param: The routine id, an unique inline string.
 * return: A sdcr status. 0 is success. 
 */
sdcr_status sdcr_routine_stop(const char *id);

/* Will clear all routine from memory.
 * return: A sdcr status. 0 is success. 
 */
sdcr_status sdcr_routine_clear_all();


#endif // _SDCR_H_