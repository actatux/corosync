/*
 * Copyright (c) 2008, 2009 Red Hat, Inc.
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
#ifndef COROSYNC_CONFDB_H_DEFINED
#define COROSYNC_CONFDB_H_DEFINED

#include <corosync/corotypes.h>
#include <corosync/hdb.h>

/**
 * @addtogroup confdb_corosync
 *
 * @{
 */
typedef uint64_t confdb_handle_t;

#define OBJECT_PARENT_HANDLE 0xFFFFFFFF00000000ULL

typedef enum {
	CONFDB_TRACK_DEPTH_ONE,
	CONFDB_TRACK_DEPTH_RECURSIVE
} confdb_track_depth_t;

typedef enum {
	OBJECT_KEY_CREATED,
	OBJECT_KEY_REPLACED,
	OBJECT_KEY_DELETED
} confdb_change_type_t;

typedef void (*confdb_key_change_notify_fn_t) (
	confdb_handle_t handle,
	confdb_change_type_t change_type,
	hdb_handle_t parent_object_handle,
	hdb_handle_t object_handle,
	const void *object_name,
	int  object_name_len,
	const void *key_name,
	int key_name_len,
	const void *key_value,
	int key_value_len);

typedef void (*confdb_object_create_notify_fn_t) (
	confdb_handle_t handle,
	hdb_handle_t parent_object_handle,
	hdb_handle_t object_handle,
	uint8_t *name_pt,
	int  name_len);

typedef void (*confdb_object_delete_notify_fn_t) (
	confdb_handle_t handle,
	hdb_handle_t parent_object_handle,
	uint8_t *name_pt,
	int  name_len);

typedef struct {
	confdb_object_create_notify_fn_t confdb_object_create_change_notify_fn;
	confdb_object_delete_notify_fn_t confdb_object_delete_change_notify_fn;
	confdb_key_change_notify_fn_t confdb_key_change_notify_fn;
} confdb_callbacks_t;

/** @} */

/*
 * Create a new confdb connection
 */
cs_error_t confdb_initialize (
	confdb_handle_t *handle,
	confdb_callbacks_t *callbacks);

/*
 * Close the confdb handle
 */
cs_error_t confdb_finalize (
	confdb_handle_t handle);


/*
 * Write back the configuration
 */
cs_error_t confdb_write (
	confdb_handle_t handle,
	char *error_text);

/*
 * Reload the configuration
 */
cs_error_t confdb_reload (
	confdb_handle_t handle,
	int flush,
	char *error_text);

/*
 * Get a file descriptor on which to poll.  confdb_handle_t is NOT a
 * file descriptor and may not be used directly.
 */
cs_error_t confdb_fd_get (
	confdb_handle_t handle,
	int *fd);

/*
 * Dispatch configuration changes
 */
cs_error_t confdb_dispatch (
	confdb_handle_t handle,
	cs_dispatch_flags_t dispatch_types);

/*
 * Change notification
 */
cs_error_t confdb_track_changes (
	confdb_handle_t handle,
	hdb_handle_t object_handle,
	unsigned int flags);

cs_error_t confdb_stop_track_changes (
	confdb_handle_t handle);

/*
 * Manipulate objects
 */
cs_error_t confdb_object_create (
	confdb_handle_t handle,
	hdb_handle_t parent_object_handle,
	const void *object_name,
	int object_name_len,
	hdb_handle_t *object_handle);

cs_error_t confdb_object_destroy (
	confdb_handle_t handle,
	hdb_handle_t object_handle);

cs_error_t confdb_object_parent_get (
	confdb_handle_t handle,
	hdb_handle_t object_handle,
	hdb_handle_t *parent_object_handle);

/*
 * Manipulate keys
 */
cs_error_t confdb_key_create (
	confdb_handle_t handle,
	hdb_handle_t parent_object_handle,
	const void *key_name,
	int key_name_len,
	const void *value,
	int value_len);

cs_error_t confdb_key_delete (
	confdb_handle_t handle,
	hdb_handle_t parent_object_handle,
	const void *key_name,
	int key_name_len,
	const void *value,
	int value_len);

/*
 * Key queries
 */
cs_error_t confdb_key_get (
	confdb_handle_t handle,
	hdb_handle_t parent_object_handle,
	const void *key_name,
	int key_name_len,
	void *value,
	int *value_len);

cs_error_t confdb_key_replace (
	confdb_handle_t handle,
	hdb_handle_t parent_object_handle,
	const void *key_name,
	int key_name_len,
	const void *old_value,
	int old_value_len,
	const void *new_value,
	int new_value_len);

cs_error_t confdb_key_increment (
	confdb_handle_t handle,
	hdb_handle_t parent_object_handle,
	const void *key_name,
	int key_name_len,
	unsigned int *value);

cs_error_t confdb_key_decrement (
	confdb_handle_t handle,
	hdb_handle_t parent_object_handle,
	const void *key_name,
	int key_name_len,
	unsigned int *value);

/*
 * Object queries
 * "find" loops through all objects of a given name and is also
 * a quick way of finding a specific object,
 * "iter" returns each object in sequence.
 */
cs_error_t confdb_object_find_start (
	confdb_handle_t handle,
	hdb_handle_t parent_object_handle);

cs_error_t confdb_object_find (
	confdb_handle_t handle,
	hdb_handle_t parent_object_handle,
	const void *object_name,
	int object_name_len,
	hdb_handle_t *object_handle);

cs_error_t confdb_object_find_destroy(
	confdb_handle_t handle,
	hdb_handle_t parent_object_handle);

cs_error_t confdb_object_iter_start (
	confdb_handle_t handle,
	hdb_handle_t parent_object_handle);

cs_error_t confdb_object_iter (
	confdb_handle_t handle,
	hdb_handle_t parent_object_handle,
	hdb_handle_t *object_handle,
	void *object_name,
	int *object_name_len);

cs_error_t confdb_object_iter_destroy(
	confdb_handle_t handle,
	hdb_handle_t parent_object_handle);

/*
 * Key iterator
 */
cs_error_t confdb_key_iter_start (
	confdb_handle_t handle,
	hdb_handle_t object_handle);

cs_error_t confdb_key_iter (
	confdb_handle_t handle,
	hdb_handle_t parent_object_handle,
	void *key_name,
	int *key_name_len,
	void *value,
	int *value_len);

/*
 * Get/set context variable
 */

cs_error_t confdb_context_get (
	confdb_handle_t handle,
	const void **context);

cs_error_t confdb_context_set (
	confdb_handle_t handle,
	const void *context);

#endif /* COROSYNC_CONFDB_H_DEFINED */
