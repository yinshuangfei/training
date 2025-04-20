/**
 * @file test_main_03.c
 * @author Alan Yin (yinshuangfei007@163.com)
 * @brief
 * @version 0.0.1
 * @date 2025-04-20
 *
 * @copyright Copyright (c) 2025
 *
 * @node: 解析 ini 文件，dump 至指定的流中
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "iniparser.h"

int main(int argc, char *argv[])
{
	dictionary *ini;
	char *ini_name;

	if (argc < 2) {
		ini_name = "example/good_ini/twisted.ini";
	} else {
		ini_name = argv[1];
	}

	ini = iniparser_load(ini_name);
	iniparser_dump(ini, stdout);
	iniparser_freedict(ini);

	return 0;
}