/*
 * testing SDCR
 */
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>

#include "../src/sdrc.h" //< library to test
#include "minunit.h"     //< Test framewok

//-----------------------------------------------
// TESTS "FRAMEWORK"
//-----------------------------------------------
int mu_tests_run = 0;

//-----------------------------------------------
// prototype
//-----------------------------------------------
static void dummy_callback();

//-----------------------------------------------
// TESTS
//-----------------------------------------------
static char *test_ok_new_config()
{
    // init
    sdcr_status res = 0;
    sdcr_routine_clear_all();

    // test
    res = sdcr_routine_new(.id = "green led",
                           .routine = "C..",
                           .callbackFunction = dummy_callback,
                           .routineStepTimeMs = 1);
    mu_assert("error, res != SDCR_SUCCESS", res == SDCR_SUCCESS);
    return 0;
}

static char *test_bad_new_config_bad_id()
{
    // init
    sdcr_status res = 0;
    sdcr_routine_clear_all();

    // test
    res = sdcr_routine_new(.routine = "C..",
                           .callbackFunction = dummy_callback,
                           .routineStepTimeMs = 1);
    mu_assert("error, res != SDCR_ERROR_NULL_PTR", res == SDCR_ERROR_INVALID_ROUTINE_CONFIG);
    return 0;
}

static char *test_bad_new_config_id_exist()
{
    // init
    sdcr_status res = 0;
    sdcr_routine_clear_all();

    // test
    res = sdcr_routine_new(.id = "green led",
                           .routine = "C..",
                           .callbackFunction = dummy_callback,
                           .routineStepTimeMs = 1);
    mu_assert("error, res != SDCR_SUCCESS", res == SDCR_SUCCESS);

    res = sdcr_routine_new(.id = "green led",
                           .routine = "C..",
                           .callbackFunction = dummy_callback,
                           .routineStepTimeMs = 1);
    mu_assert("error, res != SDCR_ERROR_ID_ALREADY_EXIST", res == SDCR_ERROR_ID_ALREADY_EXIST);
    return 0;
}

static char *test_bad_new_config_bad_callback()
{
    // init
    sdcr_status res = 0;
    sdcr_routine_clear_all();

    // test
    res = sdcr_routine_new(.id = "green led",
                           .routine = "C..",
                           .routineStepTimeMs = 1);
    mu_assert("error, res != SDCR_ERROR_INVALID_ROUTINE_CONFIG", res == SDCR_ERROR_INVALID_ROUTINE_CONFIG);
    return 0;
}

static char *test_bad_new_config_bad_routinesteptimems()
{
    // init
    sdcr_status res = 0;
    sdcr_routine_clear_all();

    // test
    res = sdcr_routine_new(.id = "green led",
                           .routine = "C..",
                           .callbackFunction = dummy_callback,
                           .routineStepTimeMs = 0);
    mu_assert("error, res != SDCR_ERROR_INVALID_ROUTINE_CONFIG", res == SDCR_ERROR_INVALID_ROUTINE_CONFIG);
    return 0;
}

static char *test_bad_new_config_bad_routine()
{
    // init
    sdcr_status res = 0;
    sdcr_routine_clear_all();

    // test
    res = sdcr_routine_new(.id = "green led",
                           .callbackFunction = dummy_callback,
                           .routineStepTimeMs = 0);
    mu_assert("error, res != SDCR_ERROR_INVALID_ROUTINE_CONFIG", res == SDCR_ERROR_INVALID_ROUTINE_CONFIG);
    return 0;
}

static char *test_bad_new_config_too_much_routine()
{
    // init
    sdcr_status res = 0;
    sdcr_routine_clear_all();
    char arr[][2] = {
        "1",
        "2",
        "3",
        "4",
        "5",
        "5",
        "7",
        "8",
        "9",
        "10",
    };

    // test
    for (size_t i = 0; i < 10; i++)
    {
        res = sdcr_routine_new(.id = arr[i],
                               .routine = "C..",
                               .callbackFunction = dummy_callback,
                               .routineStepTimeMs = 100);
        mu_assert("error, res != SDCR_SUCCESS", res == SDCR_SUCCESS);
    }

    res = sdcr_routine_new(.id = "a bit too much",
                           .routine = "C..",
                           .callbackFunction = dummy_callback,
                           .routineStepTimeMs = 100);
    mu_assert("error, res != SDCR_ERROR_ROUTINE_MEMORY_IS_FULL", res == SDCR_ERROR_ROUTINE_MEMORY_IS_FULL);
    return 0;
}

static char *test_id_are_null()
{
    // init
    sdcr_status res = 0;
    sdcr_routine_clear_all();
    const char *badId = NULL;

    // tests
    res = sdcr_routine_clear(badId);
    mu_assert("error in sdcr_routine_clear, res != SDCR_ERROR_NULL_PTR", res == SDCR_ERROR_NULL_PTR);

    res = sdcr_routine_start_inf(badId);
    mu_assert("error in sdcr_routine_start_inf, res != SDCR_ERROR_NULL_PTR", res == SDCR_ERROR_NULL_PTR);

    res = sdcr_routine_start_for_n_cycles(badId, 42);
    mu_assert("error in sdcr_routine_start_for_n_cycles, res != SDCR_ERROR_NULL_PTR", res == SDCR_ERROR_NULL_PTR);

    res = sdcr_routine_stop(badId);
    mu_assert("error in sdcr_routine_stop, res != SDCR_ERROR_NULL_PTR", res == SDCR_ERROR_NULL_PTR);

    return 0;
}

static char *test_id_doesnt_exist()
{
    // init
    sdcr_status res = 0;
    sdcr_routine_clear_all();
    const char badId[] = "I dont really exist.";

    // tests
    res = sdcr_routine_clear(badId);
    mu_assert(
        "error in sdcr_routine_clear, res != SDCR_ERROR_ID_DOESNT_EXIST",
        res == SDCR_ERROR_ID_DOESNT_EXIST);

    res = sdcr_routine_start_inf(badId);
    mu_assert(
        "error in sdcr_routine_start_inf, res != SDCR_ERROR_ID_DOESNT_EXIST",
        res == SDCR_ERROR_ID_DOESNT_EXIST);

    res = sdcr_routine_start_for_n_cycles(badId, 42);
    mu_assert(
        "error in sdcr_routine_start_for_n_cycles, res != SDCR_ERROR_ID_DOESNT_EXIST",
        res == SDCR_ERROR_ID_DOESNT_EXIST);

    res = sdcr_routine_stop(badId);
    mu_assert(
        "error in sdcr_routine_stop, res != SDCR_ERROR_ID_DOESNT_EXIST",
        res == SDCR_ERROR_ID_DOESNT_EXIST);
    return 0;
}

static char *test_id_is_correctly_cleared()
{
    // init
    sdcr_status res = 0;
    sdcr_routine_clear_all();

    // tests
    res = sdcr_routine_new(.id = "some id",
                           .routine = "C..",
                           .callbackFunction = dummy_callback,
                           .routineStepTimeMs = 100);
    mu_assert("error new, res != SDCR_SUCCESS", res == SDCR_SUCCESS);

    res = sdcr_routine_clear("some id");
    mu_assert("error clear, res != SDCR_SUCCESS", res == SDCR_SUCCESS);

    res = sdcr_routine_start_inf("some id");
    mu_assert("error inf, res != SDCR_ERROR_ID_DOESNT_EXIST", res == SDCR_ERROR_ID_DOESNT_EXIST);
    return 0;
}

static char *all_tests()
{
    mu_run_test(test_ok_new_config);
    mu_run_test(test_bad_new_config_bad_id);
    mu_run_test(test_bad_new_config_id_exist);
    mu_run_test(test_bad_new_config_bad_callback);
    mu_run_test(test_bad_new_config_bad_routinesteptimems);
    mu_run_test(test_bad_new_config_bad_routine);
    mu_run_test(test_bad_new_config_too_much_routine);
    mu_run_test(test_id_are_null);
    mu_run_test(test_id_doesnt_exist);
    mu_run_test(test_id_is_correctly_cleared);
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

static void dummy_callback()
{
}