/**
 * @file load_random.c
 * @author Alan Yin (yinshuangfei007@163.com)
 * @brief
 * @version 0.0.1
 * @date 2025-01-15
 *
 * @copyright Copyright (c) 2025
 *
 * @node:
 */

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <limits.h>

#include "../../common/utils.h"

#define MAX_SIZE	(1024 * 1024 * 1024)

static int init_random(void)
{
	struct timespec tp;
	int rc = 0;

	rc = clock_gettime(0, &tp);
	if (-1 == rc) {
		pr_stderr("get time error");
		return -errno;
	}

	/** use nsec to init random*/
	srandom((unsigned int)tp.tv_nsec);

	pr_info("init successful, usec:%u, first random:%ld",
		tp.tv_nsec, random());

	return 0;
}

static int gen_random_data(char *filename, int size, long max, int debug)
{
	int rc = 0;
	FILE *file = NULL;
	long int num = 0;
	char line[LINE_MAX] = {0};
	long this_max;

	if (size > MAX_SIZE || size < 0) {
		pr_err("size invalid: %d", size);
		return -EINVAL;
	}

	if (max + 1 > RAND_MAX || max + 1 <= 1) {
		this_max = RAND_MAX;
	} else {
		this_max = max + 1;
	}

	file = fopen(filename, "w");
	if (NULL == file) {
		pr_stderr("fopen error");
		return -errno;
	}

	for (int i = 0; i < size; i++) {
		sprintf(line, "%ld\n", random() % this_max);

		if (debug)
			printf("[%8d] %s", i + 1, line);

		if (0 > fwrite(line, strlen(line), 1, file)) {
			pr_stderr("fwrite error");
			rc = -errno;
			goto err;
		}
	}

	rc = fclose(file);
	if (0 != rc) {
		pr_stderr("fclose error");
		return -errno;
	}

	return 0;

err:
	fclose(file);
	return rc;
}

static int __load_data(char *filename, long **data, long *total, int debug)
{
	int rc = 0;
	FILE *file = NULL;
	char line[LINE_MAX] = {0};
	size_t lines = 0;
	size_t size = 0;

	file = fopen(filename, "r");
	if (NULL == file) {
		pr_stdwarn("fopen %s error", filename);
		return -errno;
	}

	/** get lines */
	while (fgets(line, sizeof(line), file) != NULL) {
		lines++;
	}

	/** move to head */
	rc = fseek(file, 0, SEEK_SET);
	if (0 != rc) {
		pr_stderr("fseek error");
		rc = -errno;
		goto err;
	}

	*data = (long *)calloc(sizeof(long), lines);
	if (NULL == *data) {
		pr_stderr("out of memory");
		rc = -errno;
		goto err;
	}

	lines = 0;
	while (fgets(line, sizeof(line), file) != NULL) {
		(*data)[lines] = atol(line);
		lines++;
	}

	rc = fclose(file);
	if (0 != rc) {
		SAFE_FREE(*data);

		pr_stderr("fclose error");
		return -errno;
	}

	*total = lines;

	if (debug) {
		pr_dbg("load success, total lines: %d", lines);
	}

	return 0;

err:
	fclose(file);
	return rc;
}

int load_data(char *filename, long **data, long *total, long creat_size,
	      long creat_max, int debug)
{
	int rc = 0;

	rc = __load_data(filename, data, total, debug);
	if (rc == -ENOENT) {
		pr_info("create %s, and start generate random data", filename);

		rc = init_random();
		if (0 != rc)
			return rc;

		rc = gen_random_data(filename, creat_size, creat_max, debug);
		if (0 != rc)
			return rc;

		rc = __load_data(filename, data, total, debug);
		if (0 != rc)
			return rc;
	}

	return rc;
}
