/*
 * Copyright (c) 2006-2008 Red Hat, Inc.
 *
 * All rights reserved.
 *
 * Author: Christine Caulfield (ccaulfi@redhat.com)
 *
 * This software licensed under BSD license, the text of which follows:
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 * - Neither the name of the MontaVista Software, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef COROSYNC_CPG_H_DEFINED
#define COROSYNC_CPG_H_DEFINED

#include <netinet/in.h>
#include <corosync/corotypes.h>

/**
 * @addtogroup cpg_corosync
 *
 * @{
 */
typedef uint64_t cpg_handle_t;

typedef enum {
	CPG_TYPE_UNORDERED, /* not implemented */
	CPG_TYPE_FIFO,		/* same as agreed */
	CPG_TYPE_AGREED,
	CPG_TYPE_SAFE		/* not implemented */
} cpg_guarantee_t;

typedef enum {
	CPG_FLOW_CONTROL_DISABLED,	/* flow control is disabled - new messages may be sent */
	CPG_FLOW_CONTROL_ENABLED	/* flow control is enabled - new messages should not be sent */
} cpg_flow_control_state_t;


typedef enum {
	CPG_REASON_JOIN = 1,
	CPG_REASON_LEAVE = 2,
	CPG_REASON_NODEDOWN = 3,
	CPG_REASON_NODEUP = 4,
	CPG_REASON_PROCDOWN = 5
} cpg_reason_t;

struct cpg_address {
	uint32_t nodeid;
	uint32_t pid;
	uint32_t reason;
};

#define CPG_MAX_NAME_LENGTH 128
struct cpg_name {
	uint32_t length;
	char value[CPG_MAX_NAME_LENGTH];
};

#define CPG_MEMBERS_MAX 128

typedef void (*cpg_deliver_fn_t) (
	cpg_handle_t handle,
	struct cpg_name *group_name,
	uint32_t nodeid,
	uint32_t pid,
	void *msg,
	int msg_len);

typedef void (*cpg_confchg_fn_t) (
	cpg_handle_t handle,
	struct cpg_name *group_name,
	struct cpg_address *member_list, int member_list_entries,
	struct cpg_address *left_list, int left_list_entries,
	struct cpg_address *joined_list, int joined_list_entries);

typedef void (*cpg_groups_get_fn_t) (
	cpg_handle_t handle,
	uint32_t group_num,
	uint32_t group_total,
	struct cpg_name *group_name,
	struct cpg_address *member_list, int member_list_entries);

typedef struct {
	cpg_deliver_fn_t cpg_deliver_fn;
	cpg_confchg_fn_t cpg_confchg_fn;
	cpg_groups_get_fn_t cpg_groups_get_fn;
} cpg_callbacks_t;

/** @} */

/*
 * Create a new cpg connection
 */
cs_error_t cpg_initialize (
	cpg_handle_t *handle,
	cpg_callbacks_t *callbacks);

/*
 * Close the cpg handle
 */
cs_error_t cpg_finalize (
	cpg_handle_t handle);

/*
 * Get a file descriptor on which to poll.  cpg_handle_t is NOT a
 * file descriptor and may not be used directly.
 */
cs_error_t cpg_fd_get (
	cpg_handle_t handle,
	int *fd);

/* 
 * Get and set contexts for a CPG handle
 */
cs_error_t cpg_context_get (
	cpg_handle_t handle,
	void **context);

cs_error_t cpg_context_set (
	cpg_handle_t handle,
	void *context);


/*
 * Dispatch messages and configuration changes
 */
cs_error_t cpg_dispatch (
	cpg_handle_t handle,
	cs_dispatch_flags_t dispatch_types);

/*
 * Join one or more groups.
 * messages multicasted with cpg_mcast_joined will be sent to every
 * group that has been joined on handle handle.  Any message multicasted
 * to a group that has been previously joined will be delivered in cpg_dispatch
 */
cs_error_t cpg_join (
	cpg_handle_t handle,
	struct cpg_name *group);

/*
 * Leave one or more groups
 */
cs_error_t cpg_leave (
	cpg_handle_t handle,
	struct cpg_name *group);

/*
 * Multicast to groups joined with cpg_join.
 * The iovec described by iovec will be multicasted to all groups joined with
 * the cpg_join interface for handle.
 */
cs_error_t cpg_mcast_joined (
	cpg_handle_t handle,
	cpg_guarantee_t guarantee,
	struct iovec *iovec,
	int iov_len);

/*
 * Get membership information from cpg
 */
cs_error_t cpg_membership_get (
	cpg_handle_t handle,
	struct cpg_name *groupName,
	struct cpg_address *member_list,
	int *member_list_entries);

cs_error_t cpg_local_get (
	cpg_handle_t handle,
	unsigned int *local_nodeid);

cs_error_t cpg_groups_get (
	cpg_handle_t handle,
	unsigned int *num_groups);

cs_error_t cpg_flow_control_state_get (
	cpg_handle_t handle,
	cpg_flow_control_state_t *flow_control_enabled);

#endif /* COROSYNC_CPG_H_DEFINED */
