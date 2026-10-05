/*
 * Copyright (c) 2026, Qualcomm Technologies, Inc. and/or its subsidiaries.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * NCC PSCI platform hooks — stub implementation for platforms that have not
 * yet enabled NCC LPM (e.g. Hamoa).
 *
 * Implements the plat_qti_pwr_* interface consumed by
 * plat/qti/common/src/qti_pm.c.  CPU power-on and power-on-finish delegate
 * to the bl31qtilib PSCI layer.  Suspend / off hooks are no-ops because LPM
 * has not been enabled on this platform yet.
 *
 * When LPM is ready, replace this file with the full NCC PSCI implementation
 * (plat/qti/cpu/ncc/src/qcom_ncc_psci.c).
 */

#include <arch_helpers.h>
#include <common/debug.h>
#include <drivers/arm/gicv3.h>
#include <lib/psci/psci.h>

#include <bl31qtilib_defs.h>
#include <bl31qtilib_interface.h>
#include <platform_def.h>
#include <qti_plat.h>

/* --------------------------------------------------------------------------
 * plat_qti_pwr_domain_on
 *
 * Power on the CPU identified by @mpidr.  Delegates to the bl31qtilib PSCI
 * layer which handles the actual core bring-up sequence.
 *
 * Returns PSCI_E_SUCCESS on success, else a PSCI error code.
 * -------------------------------------------------------------------------- */
int plat_qti_pwr_domain_on(u_register_t mpidr, int core_pos)
{
	(void)core_pos;
	return bl31qtilib_psci_power_domain_on(mpidr);
}

/* --------------------------------------------------------------------------
 * plat_qti_pwr_domain_on_finish
 *
 * Called on the waking core after a CPU_ON power-on.  Re-initialises the
 * GIC redistributor and CPU interface, then notifies bl31qtilib.
 * -------------------------------------------------------------------------- */
void plat_qti_pwr_domain_on_finish(int core_pos, const uint8_t *states)
{
	(void)core_pos;

	plat_qti_gic_pcpu_init();
	plat_qti_gic_cpuif_enable();
	bl31qtilib_psci_power_domain_on_finish(
		read_mpidr(), states);
}

/* --------------------------------------------------------------------------
 * plat_qti_pwr_domain_off
 *
 * Stub — LPM not yet enabled on this platform.
 * -------------------------------------------------------------------------- */
void plat_qti_pwr_domain_off(const uint8_t *states)
{
	(void)states;
	/* TODO: implement NCC LPM power-off when LPM is enabled. */
}

/* --------------------------------------------------------------------------
 * plat_qti_pwr_domain_suspend
 *
 * Stub — LPM not yet enabled on this platform.
 * -------------------------------------------------------------------------- */
void plat_qti_pwr_domain_suspend(const uint8_t *states)
{
	(void)states;
	/* TODO: implement NCC LPM suspend when LPM is enabled. */
}

/* --------------------------------------------------------------------------
 * plat_qti_pwr_domain_suspend_finish
 *
 * Stub — LPM not yet enabled on this platform.
 * -------------------------------------------------------------------------- */
void plat_qti_pwr_domain_suspend_finish(const uint8_t *states)
{
	(void)states;
	/* TODO: implement NCC LPM suspend-finish when LPM is enabled. */
}

/* --------------------------------------------------------------------------
 * plat_qti_pwr_psci_init
 *
 * Initialise the bl31qtilib PSCI layer.  Called from
 * plat/qti/common/src/qti_pm.c::plat_setup_psci_ops().
 * -------------------------------------------------------------------------- */
int plat_qti_pwr_psci_init(uintptr_t warmboot_entry)
{
	uint32_t mpidr = read_mpidr();

	return bl31qtilib_psci_init((uintptr_t)warmboot_entry, mpidr);
}

/* --------------------------------------------------------------------------
 * plat_qti_pm_idle_states
 *
 * Returns the platform idle-state table.  Empty (terminator only) because
 * LPM has not been enabled on this platform yet.
 * -------------------------------------------------------------------------- */
const unsigned int *plat_qti_pm_idle_states(void)
{
	static const unsigned int no_lpm_idle_states[] = {
		0U /* terminator — no LPM states advertised */
	};

	return no_lpm_idle_states;
}

/* --------------------------------------------------------------------------
 * plat_get_target_pwr_state
 *
 * PSCI platform hook — delegates to bl31qtilib.
 * -------------------------------------------------------------------------- */
plat_local_state_t plat_get_target_pwr_state(unsigned int lvl,
					     const plat_local_state_t *states,
					     unsigned int ncpu)
{
	uint32_t mpidr = read_mpidr();

	return (plat_local_state_t)bl31qtilib_psci_get_target_pwr_state(
		lvl, (uint8_t *)states, ncpu, mpidr);
}