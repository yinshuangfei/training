/**
 * @file test_main.c
 * @author Alan Yin (yinshuangfei007@163.com)
 * @brief
 * @version 0.0.1
 * @date 2025-01-15
 *
 * @copyright Copyright (c) 2025
 *
 * @node:
 */

#include "00_load_data/load_random.h"
#include "01_sort/sort_alg.h"

#define FILENAME	"data.txt"

const sort_fn sort_array[] = {
	insert_sort,
	shell_sort,
	bubble_sort,
};

/**
 * @brief
 *
 * @param [in] data
 * @param [in] total
 * @param [in] func
 * @param [in] debug
 */
void measure_alg(long *data, long total, sort_fn func, int debug)
{
	int rc = 0;
	struct timespec start_tp, end_tp, diff_tp;
	char name[NAME_MAX] = {0};

	rc = clock_gettime(0, &start_tp);
	if (-1 == rc) {
		pr_stderr("get start_tp error");
		return;
	}

	if (debug) {
		pr_dbg("start time [ seconds:%ld, nanoseconds:%ld ]",
			start_tp.tv_sec,
			start_tp.tv_nsec);

		for (long i = 0; i < total && total < 20; i++) {
			pr_dbg("-- [%8d] %ld", i + 1, data[i]);
		}
	}

	/** 执行函数 */
	func(name, data, total);

	rc = clock_gettime(0, &end_tp);
	if (-1 == rc) {
		pr_stderr("get end_tp error");
		return;
	}

	if (debug) {
		pr_dbg("end time [ seconds:%ld, nanoseconds:%ld ]",
			end_tp.tv_sec,
			end_tp.tv_nsec);

		for (long i = 0; i < total && total < 20; i++) {
			pr_dbg("=> [%8d] %ld", i + 1, data[i]);
		}

		check_sort(data, total);
	}

	if (start_tp.tv_nsec > end_tp.tv_nsec) {
		diff_tp.tv_sec = end_tp.tv_sec - start_tp.tv_sec - 1;
		diff_tp.tv_nsec = 1000000000 +
				  end_tp.tv_nsec -
				  start_tp.tv_nsec;
	} else {
		diff_tp.tv_sec = end_tp.tv_sec - start_tp.tv_sec;
		diff_tp.tv_nsec = end_tp.tv_nsec - start_tp.tv_nsec;
	}

	pr_info("total time spend <%s> [ seconds:%ld, nanoseconds:%ld ]",
		name,
		diff_tp.tv_sec,
		diff_tp.tv_nsec);
}

int main(void)
{
	int rc = 0;
	long *data = NULL;
	long total = 0;
	int debug = 0;

	for (int i = 0; i < ARRAY_SIZE(sort_array); i++) {

		rc = load_data(FILENAME, &data, &total, 10000, 1000, debug);
		if (0 != rc) {
			SAFE_FREE(data);
			return rc;
		}

		measure_alg(data, total, sort_array[i], debug);

		SAFE_FREE(data);
	}

	return 0;
}
