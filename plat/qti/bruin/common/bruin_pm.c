/*
 * Copyright (c) 2026, Qualcomm Technologies, Inc. and/or its subsidiaries.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdint.h>

#include <lib/psci/psci.h>

#include <platform_def.h>
#include <qti_plat.h>

/* QTI_CORE_PWRDN_EN_MASK happens to be same across all CPUs */
#define QTI_CORE_PWRDN_EN_MASK		1

/* cpu power control happens to be same across all CPUs */
DEFINE_RENAME_SYSREG_RW_FUNCS(cpu_pwrctrl_val, S3_0_C15_C2_7)

/*
 * plat_qti_pwr_psci_init - PSCI backend init hook.
 *
 * The secondary-core reset vector (RVBAR) is programmed by
 * qti_sec_core_remap(), invoked from plat_setup_psci_ops() ahead of this hook,
 * so there is nothing to do here.
 */
int plat_qti_pwr_psci_init(uintptr_t warmboot_entry)
{
	(void)warmboot_entry;

	return PSCI_E_SUCCESS;
}

/*
 * plat_qti_pwr_domain_on_finish - per-core setup once it has come online.
 */
void plat_qti_pwr_domain_on_finish(int core_pos, const uint8_t *states)
{
	(void)core_pos;
	(void)states;

	plat_qti_gic_pcpu_init();
}

void plat_qti_set_cpupwrctlr_val(void)
{
	unsigned long val;

	val = read_cpu_pwrctrl_val();
	val |= QTI_CORE_PWRDN_EN_MASK;
	write_cpu_pwrctrl_val(val);

	isb();
}

/*
 * The native port only supports CPU-level standby (WFI retention). The
 * firmware power-collapse / LPM node handling (PCU/RSC/PDC/RPM) is not ported.
 */
void plat_qti_pwr_domain_off(const uint8_t *states)
{
	(void)states;
}

void plat_qti_pwr_domain_suspend(const uint8_t *states)
{
	(void)states;
}

void plat_qti_pwr_domain_suspend_finish(const uint8_t *states)
{
	(void)states;
}

void plat_qti_invoke_unhandled_isr(uint32_t id, void *handle)
{
	(void)id;
	(void)handle;
}

const unsigned int *plat_qti_pm_idle_states(void)
{
	static const unsigned int idle_states[] = {
		qti_make_pwrstate_lvl0(QTI_LOCAL_STATE_STB,
				       PSTATE_TYPE_STANDBY),
		0,
	};

	return idle_states;
}
