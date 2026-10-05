/*
 * Copyright (c) 2018, ARM Limited and Contributors. All rights reserved.
 * Copyright (c) 2018-2020, The Linux Foundation. All rights reserved.
 * Copyright (c) 2026 Qualcomm Technologies, Inc. and/or its subsidiaries.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Wildcat-specific extension of the common QTI platform interface.
 *
 * This header includes the common plat/qti/common/inc/qti_plat.h first
 * (which declares the PSCI power-domain back-end interface, GIC helpers,
 * QTI_LOCAL_PSTATE_WIDTH, etc.) and then adds Wildcat-specific declarations.
 *
 * Include-path note: -Iplat/qti/wildcat/common/inc comes before
 * -Iplat/qti/common/inc in the build, so this file is found first.
 * We pull in the common header via a relative path to avoid the shadowing.
 */

/*
 * Pull in the common QTI platform interface first.
 * This provides: PSCI power-domain declarations, GIC helpers,
 * QTI_LOCAL_PSTATE_WIDTH, qti_make_pwrstate_lvl* macros,
 * plat_qti_pm_idle_states(), and other shared APIs.
 *
 * Must be included BEFORE the wildcat guard so that the common header's
 * own #ifndef QTI_PLAT_H guard is not already set when it is processed.
 */

/* Wildcat-specific additions — protected by a separate guard. */
#ifndef WILDCAT_QTI_PLAT_H
#define WILDCAT_QTI_PLAT_H

#include "../../../common/inc/qti_plat.h"

#include <stdint.h>

#include <common/interrupt_props.h>

#define QTI_INVALID_CLUSTER_ID  ((uint32_t)-1)
#define QTI_INVALID_CPU_ID      ((uint32_t)-1)
#define QTI_INVALID_MPID_ID     ((uint32_t)-1)

/*
 * Utility functions common to QTI platforms
 */

/**
 * qti_ns_va_to_pa
 *
 * Translate a non-secure virtual address to a physical address.
 *
 * Temporarily asserts SCR_EL3.NS and issues an ATS instruction (ATS1E2R for
 * EL2 clients, ATS12E1R for EL1 clients) to perform the address translation,
 * then reads the result from PAR_EL1.
 *
 * @param[in]  va           NS virtual address to translate.
 * @param[in]  client_mode  Exception level of the NS caller; must be MODE_EL1
 *                          or MODE_EL2.
 * @param[out] pa_out       Receives the translated physical address on success.
 * @return 0 on success, -1 if the translation faulted (PAR_EL1.F set).
 */
int qti_ns_va_to_pa(uintptr_t va, unsigned int client_mode, uintptr_t *pa_out);

/* Optional functions required in ARM standard platforms */
unsigned int plat_qcom_core_pos_by_mpidr(u_register_t mpidr);
unsigned int find_cluster_id(void);
unsigned int find_cluster_id_by_mpidr(u_register_t mpidr);

/**
 *  @brief - Returns cluster id for logical cpu number
 *
 *  @param - cpu number
 *  @return - cluster id if found successful, otherwise return error
 */
uint32_t plat_qti_get_cluster_id_from_logical_cpu_num(uint32_t cpu_num);

/**
 *  @brief - Returns core id for logical cpu number
 *
 *  @param - cpu number
 *  @return - core id if found successful, otherwise return error
 */
uint32_t plat_qti_get_core_id_from_logical_cpu_num(uint32_t cpu_num);

/**
 *  @brief - Returns mpir val of logical cpu number
 *
 *  @param - cpu number
 *  @return - mpidr if found successful, otherwise return error
 */
uint32_t plat_qti_logical_cpu_num_to_mpidr(uint32_t cpu_num);

/**
 *  @brief - Configures CPUSS configs
 *
 *  @param - void
 *  @return - void
 */

void plat_cpuss_config(void);

/*
 * INTU interrupt-type helpers (wildcat_common.c).
 *
 * configure_irq_type() programs a CLR_EDGE / SET_LEVEL register pair:
 *   clr_edge_base[i]  = cfg_arr[i]
 *   set_level_base[i] = ~cfg_arr[i]
 * with a readback after each write for ordering.
 *
 * configure_irq_array() writes cfg_arr[] verbatim to consecutive 32-bit
 * registers starting at base, again with a readback after each write.
 */
void configure_irq_type(uintptr_t clr_edge_base, uintptr_t set_level_base,
			const uint32_t *cfg_arr, unsigned int num_words);
void configure_irq_array(uintptr_t base, const uint32_t *cfg_arr,
			 unsigned int num_words);

#endif /* WILDCAT_QTI_PLAT_H */
