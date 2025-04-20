/**
 * @file test_main_02.c
 * @author Alan Yin (yinshuangfei007@163.com)
 * @brief
 * @version 0.0.1
 * @date 2025-04-20
 *
 * @copyright Copyright (c) 2025
 *
 * @node: 解析 ini 文件，内存态更新键值对，并更新字典值至文件中
 */

#include "iniparser.h"

void create_empty_ini_file(void)
{
	FILE *ini;

	if ((ini = fopen("example.ini", "w")) == NULL) {
		fprintf(stderr, "iniparser: cannot create example.ini\n");
		return;
	}

	fclose(ini);
}

int write_to_ini(const char *ini_name)
{
	void *dictionary;
	FILE *ini_file;
	int ret = 0;

	if (!ini_name) {
		fprintf(stderr, "Invalid argurment\n");
		return -1;
	}

	dictionary = iniparser_load(ini_name);

	if (!dictionary) {
		fprintf(stderr, "cannot parse file: %s\n", ini_name);
		return -1;
	}

	/* set section */
	ret = iniparser_set(dictionary, "Pizza", NULL);

	if (ret < 0) {
		fprintf(stderr, "cannot set section in: %s\n", ini_name);
		ret = -1;
		goto free_dict;
	}

	/* set key/value pair */
	ret = iniparser_set(dictionary, "Pizza:Cheese", "TRUE");
	if (ret < 0) {
		fprintf(stderr, "cannot set key/value in: %s\n", ini_name);
		ret = -1;
		goto free_dict;
	}

	ret = iniparser_set(dictionary, "Pizza1:Cheese", "dddd");
	if (ret < 0) {
		fprintf(stderr, "cannot set key/value in: %s\n", ini_name);
		ret = -1;
		goto free_dict;
	}

	ini_file = fopen(ini_name, "w+");

	if (!ini_file) {
		fprintf(stderr, "iniparser: cannot create example.ini\n");
		ret = -1;
		goto free_dict;
	}

	/**
	 * （1）没有 section 的配置不写入
	 * （2）一个 section 也没有，则写入所有扫描到的配置
	 **/
	iniparser_dump_ini(dictionary, ini_file);
	fclose(ini_file);

free_dict:
	iniparser_freedict(dictionary);
	return ret;
}

int main(int argc, char *argv[])
{
	int ret;

	if (argc < 2) {
		create_empty_ini_file();
		ret = write_to_ini("example.ini");
	} else
		ret = write_to_ini(argv[1]);

	return ret ;
}