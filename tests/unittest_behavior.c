/*
 * testing SDCR
 */
#include <stdio.h>

#include "minunit.h"     //< Test framewok
#include "../src/sdrc.h" //< library to test

//-----------------------------------------------
// TESTS "FRAMEWORK"
//-----------------------------------------------
int mu_tests_run = 0;
static uint32_t g_fakeTick = 0;
static uint32_t g_callbackCounter = 0;

//-----------------------------------------------
// prototype
//-----------------------------------------------
static uint32_t get_fake_tick();
static void callback_counter();

//-----------------------------------------------
// MAIN
//-----------------------------------------------
static char *test_blink_pattern_call_everytime()
{
    // init
    g_fakeTick = 0;        //< reset global flag
    g_callbackCounter = 0; //< reset global flag

    sdcr_status res = 0;
    char *pattern = "C";
    sdcr_routine_clear_all();

    res = sdcr_routine_new(.id = "green led",
                           .routine = pattern,
                           .callbackFunction = callback_counter,
                           .routineStepTimeMs = 1);
    mu_assert("error, res != SDCR_SUCCESS", res == SDCR_SUCCESS);

    res = sdcr_routine_start_inf("green led");
    mu_assert("error, res != SDCR_SUCCESS", res == SDCR_SUCCESS);

    // tests
    for (size_t i = 0; i < 100; i++)
    {
        g_fakeTick++; //< 1 tick pass every time
        sdcr_task(get_fake_tick);
    }
    // callback should be called 100 times
    // printf("callback counter=%d \n", g_callbackCounter);
    // printf("fake tick=%d \n", g_fakeTick);
    mu_assert("error, g_callbackCounter != 100", g_callbackCounter == 100);
    return 0;
}

static char *test_blink_pattern_call_everytime_with_delay()
{
    // init
    g_fakeTick = 0;        //< reset global flag
    g_callbackCounter = 0; //< reset global flag
    sdcr_routine_clear_all();

    sdcr_status res = 0;
    char *pattern = "C";
    res = sdcr_routine_new(.id = "green led",
                           .routine = pattern,
                           .callbackFunction = callback_counter,
                           .routineStepTimeMs = 100);
    mu_assert("error, res != SDCR_SUCCESS", res == SDCR_SUCCESS);

    res = sdcr_routine_start_inf("green led");
    mu_assert("error, res != SDCR_SUCCESS", res == SDCR_SUCCESS);

    // tests
    for (size_t i = 0; i < 100 * 100; i++)
    {
        g_fakeTick++; //< 1 tick pass every time
        sdcr_task(get_fake_tick);
    }
    // callback should be called 100 times
    mu_assert("error, g_callbackCounter != 100", g_callbackCounter == 100);
    return 0;
}

static char *test_blink_pattern_checkcursor()
{
    // init
    g_fakeTick = 0;        //< reset global flag
    g_callbackCounter = 0; //< reset global flag
    sdcr_routine_clear_all();

    sdcr_status res = 0;
    char *pattern = "C...CC....CC........";
    res = sdcr_routine_new(.id = "green led",
                           .routine = pattern,
                           .callbackFunction = callback_counter,
                           .routineStepTimeMs = 100);
    mu_assert("error, res != SDCR_SUCCESS", res == SDCR_SUCCESS);

    res = sdcr_routine_start_inf("green led");
    mu_assert("error, res != SDCR_SUCCESS", res == SDCR_SUCCESS);

    // tests
    for (size_t i = 0; i < 100 * 20; i++)
    {
        g_fakeTick++; //< 1 tick pass every time
        sdcr_task(get_fake_tick);
    }
    mu_assert("error, g_callbackCounter != 5", g_callbackCounter == 5);

    g_fakeTick = 10000;       //< big number to ensure time is due
    sdcr_task(get_fake_tick); //< execute one more time

    // callback should be called because we are exactly at the start of the pattern
    mu_assert("error, g_callbackCounter != 6", g_callbackCounter == 6);
    return 0;
}

static char *test_blink_pattern_cddd()
{
    // init
    g_fakeTick = 0;        //< reset global flag
    g_callbackCounter = 0; //< reset global flag
    sdcr_routine_clear_all();

    sdcr_status res = 0;
    char *pattern = "C...";
    res = sdcr_routine_new(.id = "green led",
                           .routine = pattern,
                           .callbackFunction = callback_counter,
                           .routineStepTimeMs = 100);
    mu_assert("error, res != SDCR_SUCCESS", res == SDCR_SUCCESS);

    res = sdcr_routine_start_inf("green led");
    mu_assert("error, res != SDCR_SUCCESS", res == SDCR_SUCCESS);

    // tests
    for (size_t i = 0; i < 100 * 100; i++)
    {
        g_fakeTick++; //< 1 tick pass every time
        sdcr_task(get_fake_tick);
    }
    mu_assert("error, g_callbackCounter != 25", g_callbackCounter == 25);
    return 0;
}
static char *test_blink_pattern_for_n_cylce()
{
    // init
    g_fakeTick = 0;        //< reset global flag
    g_callbackCounter = 0; //< reset global flag
    sdcr_routine_clear_all();

    sdcr_status res = 0;
    char *pattern = "C...";
    res = sdcr_routine_new(.id = "green led",
                           .routine = pattern,
                           .callbackFunction = callback_counter,
                           .routineStepTimeMs = 10);
    mu_assert("error, res != SDCR_SUCCESS", res == SDCR_SUCCESS);

    res = sdcr_routine_start_for_n_cycles("green led", 10);
    mu_assert("error, res != SDCR_SUCCESS", res == SDCR_SUCCESS);

    // tests
    for (size_t i = 0; i < 10 * 1000; i++)
    {
        g_fakeTick++; //< 1 tick pass every time
        sdcr_task(get_fake_tick);
    }
    mu_assert("error, g_callbackCounter != 25", g_callbackCounter == 10);
    return 0;
}

static char *all_tests()
{
    mu_run_test(test_blink_pattern_call_everytime);
    mu_run_test(test_blink_pattern_call_everytime_with_delay);
    mu_run_test(test_blink_pattern_checkcursor);
    mu_run_test(test_blink_pattern_cddd);
    mu_run_test(test_blink_pattern_for_n_cylce);
    return 0;
}

//-----------------------------------------------
// MAIN
//-----------------------------------------------
int main()
{
    char *result = all_tests();
    if (result != 0)
    {
        printf("%s\n", result);
    }
    else
    {
        printf("ALL TESTS PASSED\n");
    }
    printf("Tests run: %d\n", mu_tests_run);

    return result != 0;
}

static uint32_t get_fake_tick()
{
    return g_fakeTick;
}

static void callback_counter()
{
    ++g_callbackCounter;
}