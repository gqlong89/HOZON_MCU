/*
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * THIS SOFTWARE IS PROVIDED BY NXP "AS IS" AND ANY EXPRESSED OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL NXP OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef OSIF_H
#define OSIF_H

#include <stdbool.h>
#include <sys/_stdint.h>
#include <S32K144.h>

/**
 * @page misra_violations MISRA-C:2012 violations
 *
 * @section [global]
 * Violates MISRA 2012 Advisory Rule 2.5, Global macro not referenced.
 * The macro defines a value that will be interpreted as an infinite timeout.
 *
 */

/*! @file */

/*!
 * @addtogroup osif
 * @{
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @cond DRIVER_INTERNAL_USE_ONLY */

#ifdef USING_OS_FREERTOS
/* FreeRTOS implementation */
#include "FreeRTOS.h"
#include "semphr.h"
/*! @brief Type for a mutex. */
typedef SemaphoreHandle_t mutex_t;
/*! @brief Type for a semaphore. */
typedef SemaphoreHandle_t semaphore_t;

#else
/* Bare-metal implementation */
/*! @brief Type for a mutex. */
typedef uint8_t mutex_t;
/*! @brief Type for a semaphore. */
typedef volatile uint8_t semaphore_t;
#endif /* ifdef USING_OS_FREERTOS */

/*! @endcond */

#define OSIF_WAIT_FOREVER 0xFFFFFFFFu
#define TICKS_PER_SECOND      (100u)

#include "status.h"

/*******************************************************************************
 * API
 ******************************************************************************/

#if defined (__cplusplus)
extern "C" {
#endif

/*!
 * @brief Delays execution for a number of milliseconds.
 *
 * @param[in] delay Time delay in milliseconds.
 */
void OSIF_TimeDelay(const uint32_t delay);

/*!
 * @brief Returns the number of miliseconds elapsed since starting the internal timer
 * or starting the scheduler.
 *
 * @return the number of miliseconds elapsed
 */
uint32_t OSIF_GetMilliseconds(void);

/*!
 * @brief Waits for a mutex and locks it.
 *
 * @param[in] pMutex reference to the mutex object
 * @param[in] timeout time-out value in milliseconds
 * @return  One of the possible status codes:
 * - STATUS_SUCCESS: mutex lock operation success
 * - STATUS_ERROR: mutex already owned by current thread
 * - STATUS_TIMEOUT: mutex lock operation timed out
 *
 */
status_t OSIF_MutexLock(const mutex_t * const pMutex,
                        const uint32_t timeout);

/*!
 * @brief Unlocks a previously locked mutex.
 *
 * @param[in] pMutex reference to the mutex object
 * @return  One of the possible status codes:
 * - STATUS_SUCCESS: mutex unlock operation success
 * - STATUS_ERROR: mutex unlock failed
 */
status_t OSIF_MutexUnlock(const mutex_t * const pMutex);


/*!
 * @brief Create an unlocked mutex.
 *
 * @param[in] pMutex reference to the mutex object
 * @return  One of the possible status codes:
 * - STATUS_SUCCESS: mutex created
 * - STATUS_ERROR: mutex could not be created
 */
status_t OSIF_MutexCreate(mutex_t * const pMutex);

/*!
 * @brief Destroys a previously created mutex.
 *
 * @param[in] pMutex reference to the mutex object
 * @return  One of the possible status codes:
 * - STATUS_SUCCESS: mutex destroyed
 */
status_t OSIF_MutexDestroy(const mutex_t * const pMutex);


/*!
 * @brief Decrement a semaphore with timeout.
 *
 * @param[in] pSem reference to the semaphore object
 * @param[in] timeout time-out value in milliseconds
 * @return  One of the possible status codes:
 * - STATUS_SUCCESS: semaphore wait operation success
 * - STATUS_TIMEOUT: semaphore wait timed out
 */
status_t OSIF_SemaWait(semaphore_t * const pSem,
                       const uint32_t timeout);


/*!
 * @brief Increment a semaphore
 *
 * @param[in] pSem reference to the semaphore object
 * @return  One of the possible status codes:
 * - STATUS_SUCCESS: semaphore post operation success
 * - STATUS_ERROR: semaphore could not be incremented
 */
status_t OSIF_SemaPost(semaphore_t * const pSem);


/*!
 * @brief Creates a semaphore with a given value.
 *
 * @param[in] pSem reference to the semaphore object
 * @param[in] initValue initial value of the semaphore
 * @return  One of the possible status codes:
 * - STATUS_SUCCESS: semaphore created
 * - STATUS_ERROR: semaphore could not be created
 */
status_t OSIF_SemaCreate(semaphore_t * const pSem,
                         const uint8_t initValue);


/*!
 * @brief Destroys a previously created semaphore.
 *
 * @param[in] pSem reference to the semaphore object
 * @return  One of the possible status codes:
 * - STATUS_SUCCESS: semaphore destroyed
 */
status_t OSIF_SemaDestroy(const semaphore_t * const pSem);

void SysTick_Init(void);

extern volatile uint64_t s_osif_tick_cnt;

extern uint32_t tag_fifo[16];
extern uint32_t tag_in;
extern uint32_t tag_out;

static inline void SysTick_clr_tag(void)
{
    tag_out = tag_in;
}

static inline int SysTick_get_tag(uint32_t *tick)
{
    if (tag_in != tag_out)
    {
        *tick = tag_fifo[tag_out & 0xf];
        tag_out++;
        return 0;
    }

    return -1;
}

static inline void SysTick_put_tag(uint32_t tick)
{
    if (tag_in != tag_out + 15)
    {
        tag_fifo[tag_in & 0xf] = tick;
        tag_in++;
    }
}

static inline uint64_t SysTick_GetTickCount(void)
{
    return s_osif_tick_cnt;
}

static inline uint32_t SysTick_GetCurCounter(void)
{
    /* Change the countdown to a positive count. */
    return (S32_SysTick->RVR - S32_SysTick->CVR);
}

static inline uint32_t SysTick_GetCountFlag(void)
{
    return S32_SysTick->CSR & S32_SysTick_CSR_COUNTFLAG_MASK;
}

bool osif_IsIsrContext(void);

static inline uint64_t driverGetUptime(void)
{
    return s_osif_tick_cnt;
}

/*! @}*/
#if defined (__cplusplus)
}
#endif

/*! @}*/

#endif /* OSIF_H */
/*******************************************************************************
 * EOF
 ******************************************************************************/
