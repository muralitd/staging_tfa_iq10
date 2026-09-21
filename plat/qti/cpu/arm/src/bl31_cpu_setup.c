/*
 * Copyright (c) 2026, Qualcomm Technologies, Inc. and/or its subsidiaries.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * ARM CPU-specific BL31 setup for QTI platforms.
 *
 * This file contains CPU-specific initialisation for standard ARM CPUs
 * (e.g., Cortex-A series). It is selected when QTI_NCC_CPU is not set
 * in the platform.mk file (i.e., QTI_NCC_CPU := 0 or unset).
 *
 * Responsibilities:
 *   - EL3 GIC system-register interface initialisation (qti_el3_sys_regs_init)
 *   - plat_qti_cpu_boot_setup entry point called by wildcat_bl31_setup.c
 *
 * Note: The NCC-specific CL4 sleep-state reset workaround is NOT performed
 * for standard ARM CPUs.
 */

#include <arch_helpers.h>
#include <common/debug.h>

/*
 * qti_el3_sys_regs_init - Initialise EL3 system registers for ARM CPUs.
 *
 * Configures the GIC system-register interface and interrupt-control
 * registers at EL3 for standard ARM CPU architectures (Cortex-A series).
 */
void qti_el3_sys_regs_init(void)
{
	/* EL3 SRE: enable system-register interface for EL3 */
	write_icc_sre_el3(0x9U | read_icc_sre_el3());

	/* Set PMHE & IDbits to 24 bits */
	write_icc_ctlr_el3(0xCC40U);

	/* EL1 SRE: enable system-register interface for EL1 */
	write_icc_sre_el1(0x1U | read_icc_sre_el1());

	/* Enable Group 0 interrupts at EL1 */
	write_icc_igrpen0_el1(1U);
}

/*
 * plat_qti_cpu_boot_setup - ARM CPU boot-time setup.
 *
 * Called from bl31_early_platform_setup() in wildcat_bl31_setup.c for
 * each CPU boot.  Performs EL3 GIC system-register initialisation for
 * standard ARM CPUs.
 */
void plat_qti_cpu_boot_setup(void)
{
	qti_el3_sys_regs_init();
}