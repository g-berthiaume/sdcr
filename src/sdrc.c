/* 
 * srdc.c
 * String Defined Call Routine library
 * 
 * Copyright (c) 2019 G.Berthiaume , All rights reserved.
 * BSD 3-Clause License (Revised)
 */

//-----------------------------------------------
// INCLUDES
//-----------------------------------------------
#include <stdio.h> //todo delete
#include <stdbool.h>

#include "sdrc.h"

//-----------------------------------------------
// DEFINITION
//-----------------------------------------------
typedef struct
{
    /* user config */
    sdcr_routine_configuration config;
    /* flags */
    bool isEnable;
    bool isInfinite;
    /* State and time variables */
    int32_t cyclesLeft;
    uint32_t timestampLastAction;
    char *routineCursor;
} sdcr_routine_state_machine;

typedef struct
{
    const char *routineIDs[SDCR_MAX_NUMBER_OF_ROUTINE];
    sdcr_routine_state_machine routines[SDCR_MAX_NUMBER_OF_ROUTINE];
} sdcr_memory;

//-----------------------------------------------
// MACROS
//-----------------------------------------------
#define ARRAY_LENGTH(arr) (sizeof(arr) / sizeof((arr)[0])) //< Wont work with ptr.

//-----------------------------------------------
// GLOBAL VARIABLES
//-----------------------------------------------
static sdcr_memory gMemory = {0};

//-----------------------------------------------
// INTERNAL PROTOTYPES
//-----------------------------------------------
static sdcr_routine_state_machine *sdcr_get_routine_from_id(const char *id);
static char sdcr_get_action(sdcr_routine_state_machine *routine);
static uint32_t sdcr_get_elapsed_time(uint32_t then, sdcr_get_tick_function getTickMs);

//-----------------------------------------------
// API FUNCTIONS
//-----------------------------------------------
sdcr_status sdcr_task(sdcr_get_tick_function getTickMs)
{
    if (getTickMs == NULL)
        return SDCR_ERROR_NULL_PTR;

    for (size_t routineIndex = 0;
         routineIndex < ARRAY_LENGTH(gMemory.routineIDs);
         routineIndex++)
    {
        const char *currentId = gMemory.routineIDs[routineIndex];
        sdcr_routine_state_machine *currentroutine = &gMemory.routines[routineIndex];

        const bool routineIsEnable = (currentroutine->isEnable == true);
        const bool routineExist = (currentId != 0);
        const bool routineIsActive = (routineExist && routineIsEnable);
        if (routineIsActive)
        {
            const bool actionIsNeeded = (sdcr_get_elapsed_time(currentroutine->timestampLastAction, getTickMs) >=
                                         currentroutine->config.routineStepTimeMs);
            if (actionIsNeeded)
            {
                const char action = sdcr_get_action(currentroutine);
                if (action != '.')
                {
                    currentroutine->config.callbackFunction();
                }
                // update timestamp
                currentroutine->timestampLastAction = getTickMs();
            }
        }
    }
    return SDCR_SUCCESS;
}

sdcr_status sdcr_routine_new_base(sdcr_routine_configuration config)
{
    // check if ID is valid
    if (config.id == NULL)
        return SDCR_ERROR_INVALID_ROUTINE_CONFIG;

    // Check if ID doesnt already exist
    for (size_t i = 0;
         i < ARRAY_LENGTH(gMemory.routineIDs);
         i++)
    {
        const bool idFound = (gMemory.routineIDs[i] == config.id);
        if (idFound)
            return SDCR_ERROR_ID_ALREADY_EXIST;
    }
    // Check if config is valid
    if (config.routineStepTimeMs <= 0)
        return SDCR_ERROR_INVALID_ROUTINE_CONFIG;
    if (config.callbackFunction == NULL)
        return SDCR_ERROR_INVALID_ROUTINE_CONFIG;
    if (config.routine == NULL)
        return SDCR_ERROR_INVALID_ROUTINE_CONFIG;
    char *routineToCheck = config.routine;
    while (*routineToCheck != '\0') //todo dangeureux ?
    {
        char unit = *routineToCheck++;
        switch (unit)
        {
        case '.':
            break;
        case 'C':
            break;
        case 'c':
            break;
        default: //< config contains invalid char.
            return SDCR_ERROR_INVALID_ROUTINE_CONFIG;
        }
    }
    // Everything seems fine: Store new id and config
    for (size_t i = 0;
         i < ARRAY_LENGTH(gMemory.routineIDs);
         i++)
    {
        const bool memoryIsFree = (gMemory.routineIDs[i] == 0);
        if (memoryIsFree)
        {           
            gMemory.routines[i].config = config;                    //< Stores routine's configuration
            gMemory.routineIDs[i] = config.id;                      //< Stores routine's id
            gMemory.routines[i].routineCursor = config.routine;     //< Point routine cursor to the routine's start 
            gMemory.routines[i].isEnable = false;                  //< Routine is not enabled yet.
            return SDCR_SUCCESS; //< stored this configuration succesfully
        }
    }
    return SDCR_ERROR_ROUTINE_MEMORY_IS_FULL;
}

sdcr_status sdcr_routine_clear(const char *id)
{
    if (id == NULL)
        return SDCR_ERROR_NULL_PTR;

    for (size_t i = 0;
         i < ARRAY_LENGTH(gMemory.routineIDs);
         i++)
    {
        if (gMemory.routineIDs[i] == id)
        {
            // id found, erase this routine from memory
            gMemory.routineIDs[i] = NULL;
            gMemory.routines[i] = (sdcr_routine_state_machine){0};
            return SDCR_SUCCESS;
        }
    }
    return SDCR_ERROR_ID_DOESNT_EXIST;
}

sdcr_status sdcr_routine_start_inf(const char *id)
{
    if (id == NULL)
        return SDCR_ERROR_NULL_PTR;

    sdcr_routine_state_machine *routine = sdcr_get_routine_from_id(id);
    if (routine == NULL)
        return SDCR_ERROR_ID_DOESNT_EXIST;

    routine->isEnable = true;
    routine->isInfinite = true;
    return SDCR_SUCCESS;
}

sdcr_status sdcr_routine_start_for_n_cycles(const char *id, uint16_t n)
{
    if (id == NULL)
        return SDCR_ERROR_NULL_PTR;
    if (n <= 0)
        return SDCR_ERROR_INVALID_API_USAGE;

    sdcr_routine_state_machine *routine = sdcr_get_routine_from_id(id);
    if (routine == NULL)
        return SDCR_ERROR_ID_DOESNT_EXIST;

    routine->isEnable = true;
    routine->isInfinite = false;
    routine->cyclesLeft = n;
    return SDCR_SUCCESS;
}

sdcr_status sdcr_routine_stop(const char *id)
{
    if (id == NULL)
        return SDCR_ERROR_NULL_PTR;

    sdcr_routine_state_machine *routine = sdcr_get_routine_from_id(id);
    if (routine == NULL)
        return SDCR_ERROR_ID_DOESNT_EXIST;

    routine->isEnable = false;
    return SDCR_SUCCESS;
}

sdcr_status sdcr_routine_clear_all()
{
    gMemory = (sdcr_memory){0}; //< reset everything
    return SDCR_SUCCESS;
}

//-----------------------------------------------
// INTERNAL FUNCTIONS
//-----------------------------------------------
static sdcr_routine_state_machine *sdcr_get_routine_from_id(const char *id)
{
    for (size_t i = 0;
         i < ARRAY_LENGTH(gMemory.routineIDs);
         i++)
    {
        if (gMemory.routineIDs[i] == id)
            return &gMemory.routines[i];
    }
    return NULL;
}

static char sdcr_get_action(sdcr_routine_state_machine *routine)
{
    const bool routineNeedToLoop = (*routine->routineCursor == '\0');
    if (routineNeedToLoop)
    {
        routine->routineCursor = routine->config.routine; //< return to the begining
        if (!!!routine->isInfinite)
        {
            routine->cyclesLeft--;

            const bool isThisTheLastCycle = (routine->cyclesLeft <= 1);
            if (isThisTheLastCycle)
            {
                routine->isEnable = false;
            }
        }
    }
    char action = *routine->routineCursor;
    routine->routineCursor++; //< Advance the cursor
    return action;
}

static uint32_t sdcr_get_elapsed_time(uint32_t then, sdcr_get_tick_function getTickMs)
{
    uint32_t now = getTickMs();
    uint32_t elapsed = now - then;
    return elapsed;
}