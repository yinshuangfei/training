/*
 * @Author: Alan Yin
 * @Date: 2024-07-20 09:54:18
 * @LastEditTime: 2024-07-20 10:01:13
 * @LastEditors: Alan Yin
 * @FilePath: /02_nas_mgmt/name_to_handle_at/t_name_to_handle_at.c
 * @Description:
 * // -*- mode:C; tab-width:8; c-basic-offset:2; indent-tabs-mode:t -*-
 * // vim: ts=8 sw=2 smarttab
 * Copyright (c) 2024 by HT706, All Rights Reserved.
 */
#define _GNU_SOURCE

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#define errExit(msg) do { perror(msg); exit(EXIT_FAILURE); } while (0)

int main(int argc, char *argv[])
{
	struct file_handle *fhp;
	int mount_id, fhsize, flags, dirfd, j;
	char *pathname;

	if (argc != 2) {
		fprintf(stderr, "Usage: %s pathname\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	pathname = argv[1];

	/* Allocate file_handle structure */
	fhsize = sizeof(*fhp);
	fhp = malloc(fhsize);
	if (fhp == NULL)
		errExit("malloc");

	/* Make an initial call to name_to_handle_at() to discover
	   the size required for file handle */

	dirfd = AT_FDCWD;           /* For name_to_handle_at() calls */
	flags = 0;                  /* For name_to_handle_at() calls */
	fhp->handle_bytes = 0;
	if (name_to_handle_at(dirfd, pathname, fhp,
				&mount_id, flags) != -1 || errno != EOVERFLOW) {
		fprintf(stderr, "Unexpected result from name_to_handle_at()\n");
		exit(EXIT_FAILURE);
	}

	/* Reallocate file_handle structure with correct size */
	fhsize = sizeof(struct file_handle) + fhp->handle_bytes;
	fhp = realloc(fhp, fhsize);         /* Copies fhp->handle_bytes */
	if (fhp == NULL)
		errExit("realloc");

	/* Get file handle from pathname supplied on command line */

	if (name_to_handle_at(dirfd, pathname, fhp, &mount_id, flags) == -1)
		errExit("name_to_handle_at");

	/* Write mount ID, file handle size, and file handle to stdout,
		for later reuse by t_open_by_handle_at.c */

	printf("%d\n", mount_id);
	printf("%d %d   ", fhp->handle_bytes, fhp->handle_type);
	for (j = 0; j < fhp->handle_bytes; j++)
		printf(" %02x", fhp->f_handle[j]);
	printf("\n");

	exit(EXIT_SUCCESS);
}
