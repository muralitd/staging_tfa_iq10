/*
 * Copyright (c) 2024, Arm Limited and Contributors. All rights reserved.
 * Copyright (c) 2026, Qualcomm Technologies, Inc. and/or its subsidiaries.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <assert.h>

#include <arch_helpers.h>
#include <common/debug.h>
#include <drivers/arm/css/scmi.h>

#include "scmi_private.h"

/*
 * SCMI Reset Domain Management protocol message and response lengths.
 * Calculated as sum of header (4 bytes) + payload bytes.
 *
 * RESET_REQUEST payload:  domain_id (4) + flags (4) + reset_state (4) = 12
 * RESET_REQUEST response: status (4)
 */
#define SCMI_RESET_DOMAIN_RESET_MSG_LEN		16U	/* 4 hdr + 12 payload */
#define SCMI_RESET_DOMAIN_RESET_RESP_LEN	8U	/* 4 hdr + 4 status  */

/*
 * API to send an SCMI Reset Domain Management RESET_REQUEST command
 * (SCMI spec §4.7.2.3).
 *
 * @p:           Opaque pointer to the initialized SCMI channel (scmi_channel_t *)
 * @domain_id:   Identifier for the reset domain.
 *               For QTI CPUCP: bits[15:8] = cluster (AFF1), bits[7:0] = core (AFF0)
 * @flags:       Reset flags (SCMI_RESET_FLAG_SYNC / SCMI_RESET_FLAG_AUTONOMOUS etc.)
 * @reset_state: Reset state (SCMI_RESET_STATE_ARCH / SCMI_RESET_STATE_IMPL | value)
 *
 * Returns SCMI_E_SUCCESS (0) on success, or a negative SCMI error code.
 */
int scmi_reset_domain_request(void *p, uint32_t domain_id,
			      uint32_t flags, uint32_t reset_state)
{
	mailbox_mem_t *mbx_mem;
	unsigned int token = 0;
	int ret;
	scmi_channel_t *ch = (scmi_channel_t *)p;

	validate_scmi_channel(ch);

	scmi_get_channel(ch);

	mbx_mem = (mailbox_mem_t *)(ch->info->scmi_mbx_mem);
	mbx_mem->msg_header = SCMI_MSG_CREATE(SCMI_RESET_DOMAIN_PROTO_ID,
					      SCMI_RESET_DOMAIN_RESET_MSG, token);
	mbx_mem->len = SCMI_RESET_DOMAIN_RESET_MSG_LEN;
	mbx_mem->flags = SCMI_FLAG_RESP_POLL;
	SCMI_PAYLOAD_ARG3(mbx_mem->payload, domain_id, flags, reset_state);

	scmi_send_sync_command(ch);

	/* Get the return values */
	SCMI_PAYLOAD_RET_VAL1(mbx_mem->payload, ret);
	assert(mbx_mem->len == SCMI_RESET_DOMAIN_RESET_RESP_LEN);
	assert(token == SCMI_MSG_GET_TOKEN(mbx_mem->msg_header));

	scmi_put_channel(ch);

	return ret;
}