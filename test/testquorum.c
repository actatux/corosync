#include <sys/types.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <corosync/corotypes.h>
#include <corosync/quorum.h>

static quorum_handle_t handle;

static void quorum_notification_fn(
	quorum_handle_t handle,
	uint32_t quorate,
	uint64_t ring_id,
	uint32_t view_list_entries,
	uint32_t *view_list)
{
	int i;

	printf("quorum notification called \n");
	printf("  quorate   = %d\n", quorate);
	printf("  ring id   = %lld\n", ring_id);
	printf("  num nodes = %d ", view_list_entries);

	for (i=0; i<view_list_entries; i++) {
		printf(" %d ", view_list[i]);
	}
	printf("\n");
}


int main(int argc, char *argv[])
{
	int quorate;
	quorum_callbacks_t callbacks;
	int err;

	callbacks.quorum_notify_fn = quorum_notification_fn;
	if ( (err=quorum_initialize(&handle, &callbacks)) != CS_OK)
		fprintf(stderr, "quorum_initialize FAILED: %d\n", err);

	if ( (err=quorum_trackstart(handle, CS_TRACK_CHANGES)) != CS_OK)
		fprintf(stderr, "quorum_trackstart FAILED: %d\n", err);

	if ( (err=quorum_getquorate(handle, &quorate)) != CS_OK)
		fprintf(stderr, "quorum_getquorate FAILED: %d\n", err);
	else {
		printf("quorate   %d\n", quorate);
	}

	printf("Waiting for quorum events, press ^C to finish\n");
	printf("-------------------\n");

	while (1)
		quorum_dispatch(handle, CS_DISPATCH_ALL);

	return 0;
}
