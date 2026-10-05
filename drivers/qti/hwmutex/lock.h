/*------------------------------------------------------------------------------
 * Copyright (c) Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *----------------------------------------------------------------------------*/

#ifndef QTI_HWM_LOCK_H
#define QTI_HWM_LOCK_H

#include <stdint.h>

#define BLOCK		1	/* Use Bakery lock SW implementation */
#define HWMUTEX		2	/* Use HWMUTEX */
#define MONITORS	3	/* Use exclusive monitors LDXR/STXR */

enum core_lpm_status {
	CORE_NOT_IN_LPM,
	CORE_IN_LPM = 255
};

void acquire_lpm_lock(uint8_t core_id);
void release_lpm_lock(uint8_t core_id);

/* Bakery lock based atomicity */
#if (LOCK_METHOD == BLOCK)
#include "block.h"
typedef bakery_lock_t lock_t;

#define LOCK(x)		block_lock(&x)
#define UNLOCK(x)	block_unlock(&x)

/* HWMUTEX based atomicity */
#elif (LOCK_METHOD == HWMUTEX)
extern void hwm_lock(void *lock);
extern void hwm_unlock(void *lock);
typedef uint64_t lock_t;
#define LOCK(x)		hwm_lock(&x)
#define UNLOCK(x)	hwm_unlock(&x)

/* Exclusive Monitors with GCC built-in functions for atomicity */
#elif (LOCK_METHOD == MONITORS)
#include "armlib_lock.h"
typedef spinlock_t lock_t;
#define LOCK(x)		spinlock_obtain(&x)
#define UNLOCK(x)	spinlock_release(&x)

#else
#error "LOCK_METHOD not defined, available opts: BLOCK, HWMUTEX, MONITORS"
#endif

#endif /* QTI_HWM_LOCK_H */