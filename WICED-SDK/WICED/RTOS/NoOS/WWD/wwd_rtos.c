/*
 * Copyright 2015, Broadcom Corporation
 * All Rights Reserved.
 *
 * This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom Corporation;
 * the contents of this file may not be disclosed to third parties, copied
 * or duplicated in any form, in whole or in part, without the prior
 * written permission of Broadcom Corporation.
 */

/** @file
 *  Implementation of wiced_rtos.c for the case where no RTOS is used
 *
 *  This is a special implementation of the Wiced RTOS
 *  abstraction layer, which is designed to make Wiced work even with
 *  no RTOS.
 *  It provides Wiced with alternates to the functions for threads,
 *  semaphores and time functions.
 *
 *  Semaphores are simple integers, since accesses to an integer will be atomic.
 *
 */


#include "wwd_rtos.h"
#include <stdint.h>
#include "wwd_poll.h"
#include "RTOS/wwd_rtos_interface.h"
#include "platform_assert.h"



extern volatile uint32_t noos_total_time;

/******************************************************
 *             Function definitions
 ******************************************************/

/**
 * Would normally create a new thread
 * This version just returns success without any action
 *
 * @param thread         : unused - pointer to variable which will receive handle of created thread
 * @param entry_function : unused - main thread function
 * @param name           : a string thread name used for a debugger
 *
 * @returns WWD_SUCCESS on success, WICED_ERROR otherwise
 */
wwd_result_t host_rtos_create_thread( host_thread_type_t* thread, void(*entry_function)( uint32_t ), const char* name, /*@null@*/ void* stack, uint32_t stack_size, uint32_t priority )
{
    *thread = (unsigned char) 0;
    UNUSED_PARAMETER( entry_function );
    UNUSED_PARAMETER( name );
    UNUSED_PARAMETER( stack );
    UNUSED_PARAMETER( stack_size );
    UNUSED_PARAMETER( priority );

    return WWD_SUCCESS;
}

wwd_result_t host_rtos_create_thread_with_arg( host_thread_type_t* thread, void(*entry_function)( uint32_t ), const char* name, /*@null@*/ void* stack, uint32_t stack_size, uint32_t priority, uint32_t arg )
{
    *thread = (unsigned char) 0;
    UNUSED_PARAMETER( entry_function );
    UNUSED_PARAMETER( name );
    UNUSED_PARAMETER( stack );
    UNUSED_PARAMETER( stack_size );
    UNUSED_PARAMETER( priority );
    UNUSED_PARAMETER( arg );

    return WWD_SUCCESS;
}

/**
 * Would normally exit a thread
 * This version just returns success without any action
 *
 * @param thread         : unused - handle of thread
 *
 * @returns WWD_SUCCESS on success, WICED_ERROR otherwise
 */
wwd_result_t host_rtos_finish_thread( host_thread_type_t* thread )
{
    UNUSED_PARAMETER( thread);

    return WWD_SUCCESS;
}

/**
 * Would normally allow a thread to delete a terminated thread
 *
 * @param thread         : unused - handle of the terminated thread to delete
 *
 * @returns WWD_SUCCESS on success, WICED_ERROR otherwise
 */
wwd_result_t host_rtos_delete_terminated_thread( host_thread_type_t* thread )
{
    UNUSED_PARAMETER( thread );

    return WWD_SUCCESS;
}

/**
 * Creates a semaphore
 * Semaphore is just the variable passed in this case, no setup is necessary.
 * Semaphore is just initialised to zero.
 *
 * @param semaphore         : pointer to variable which will receive handle of created semaphore
 *
 * @returns WWD_SUCCESS on success, WICED_ERROR otherwise
 */
wwd_result_t host_rtos_init_semaphore( host_semaphore_type_t* semaphore )
{
    *semaphore = (unsigned char) 0;
    return WWD_SUCCESS;
}

/**
 * Would normally get a semaphore
 *
 * Since there is only one thread, this function calls the @ref wwd_thread_poll_all to cause Wiced to
 * repeatedly poll and process the Transmit and receive queues, until the given semaphore is
 * set by a function within the transmit/receive processing. Once the set semaphore has been seen,
 * it is decremented back to zero and the function returns to simulate a resuming thread
 * Timeout is ignored in this function.
 *
 * Must not be called from interrupt context.
 *
 * @param semaphore   : Pointer to variable which will receive handle of created semaphore
 * @param timeout_ms  : unsupported - Maximum period to block for. Can be passed NEVER_TIMEOUT to request no timeout
 * @param will_set_in_isr : True if the semaphore will be set in an ISR. Currently only used for NoOS/NoNS
 *
 * @return wwd_result_t : WWD_SUCCESS if semaphore was successfully acquired
 *                     : WICED_TIMEOUT if semaphore was not acquired before timeout_ms period
 */

wwd_result_t host_rtos_get_semaphore( host_semaphore_type_t* semaphore, uint32_t timeout_ms, wiced_bool_t will_set_in_isr )
{
    UNUSED_PARAMETER( timeout_ms);

    /*@-infloops@*/ /* If this function has been called correctly, the wwd_thread_poll_all() call should post this semaphore, and cause the loop to exit */
    while ( *semaphore == (unsigned char) 0 )
    {
        /* Call wiced poll repeatedly until semaphore is set. */
        if ( will_set_in_isr != WICED_TRUE )
        {
            volatile uint32_t i;

#ifndef WICED_NO_WIFI
            wwd_thread_poll_all( );
#endif
            /* Delay loop to give 802.11 device some breathing room between requests */

            for ( i = 0; i < (uint32_t) 1000; i++ )
            {
                /* do nothing */
            }

        }
    }
    /*@+infloops@*/

    WICED_DISABLE_INTERRUPTS();
    (*semaphore)--;
    WICED_ENABLE_INTERRUPTS();

    return WWD_SUCCESS;
}

/**
 * Would normally set a semaphore
 *
 * This function simply sets the value of the semaphore to 1.
 *
 * Can be called from interrupt context
 *
 * @param semaphore       : Pointer to variable which will receive handle of created semaphore
 * @param called_from_ISR : ignored - Value of WICED_TRUE indicates calling from interrupt context
 *                                    Value of WICED_FALSE indicates calling from normal thread context
 *
 * @return wwd_result_t : WWD_SUCCESS if semaphore was successfully set
 *                        : WICED_ERROR if an error occurred
 *
 */

wwd_result_t host_rtos_set_semaphore( host_semaphore_type_t* semaphore, wiced_bool_t called_from_ISR )
{
    UNUSED_PARAMETER( called_from_ISR );

    WICED_DISABLE_INTERRUPTS();
    (*semaphore)++;
    WICED_ENABLE_INTERRUPTS();

    return WWD_SUCCESS;
}

/**
 * Would normally delete a semaphore
 * Does nothing.
 *
 * @param semaphore         : Pointer to the semaphore handle
 *
 * @return wwd_result_t : WWD_SUCCESS if semaphore was successfully deleted
 *                        : WICED_ERROR if an error occurred
 *
 */

wwd_result_t host_rtos_deinit_semaphore( host_semaphore_type_t* semaphore )
{
    UNUSED_PARAMETER( semaphore );

    return WWD_SUCCESS;
}


/**
 * Gets time in miiliseconds since RTOS start
 *
 * @Note: since this is only 32 bits, it will roll over every 49 days, 17 hours.
 *
 * @returns Time in milliseconds since RTOS started.
 */
wwd_time_t host_rtos_get_time( void ) /*@modifies internalState@*/
{
    return (wwd_time_t) noos_total_time;
}

/**
 * Delay for a number of milliseconds
 *
 * Simply implemented with a tight loop
 *
 * @return wwd_result_t : WWD_SUCCESS if delay was successful
 *                        : WICED_ERROR if an error occurred
 *
 */
wwd_result_t host_rtos_delay_milliseconds( uint32_t num_ms )
{
    wwd_time_t start = host_rtos_get_time( );

    while ( ( host_rtos_get_time( ) - start ) < num_ms )
    {
        /* do nothing */
    }

    return WWD_SUCCESS;
}


