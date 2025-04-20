/**
 * @file test_main_01.c
 * @author Alan Yin (yinshuangfei007@163.com)
 * @brief
 * @version 0.0.1
 * @date 2025-04-20
 *
 * @copyright Copyright (c) 2025
 *
 * @node: 创建 ini 文件，解析 ini 文件获取指定字段
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "iniparser.h"

void create_example_ini_file(void);
int parse_ini_file(char *ini_name);

int main(int argc, char *argv[])
{
	int status;

	if (argc < 2) {
		create_example_ini_file();
		status = parse_ini_file("example.ini");
	} else {
		status = parse_ini_file(argv[1]);
	}

	return status;
}

void create_example_ini_file(void)
{
	FILE *ini;

	if ((ini = fopen("example.ini", "w")) == NULL) {
		fprintf(stderr, "iniparser: cannot create example.ini\n");
		return;
	}

	fprintf(ini,
		"#\n"
		"# This is an example of ini file\n"
		"#\n"
		"\n"
		"[Pizza]\n"
		"\n"
		"Ham       = yes ;\n"
		"Mushrooms = TRUE ;\n"
		"Capres    = 0 ;\n"
		"Cheese    = Non ;\n"
		"\n"
		"\n"
		"[Wine]\n"
		"\n"
		"Grape     = Cabernet Sauvignon ;\n"
		"Year      = 1989 ;\n"
		"Country   = Spain ;\n"
		"Alcohol   = 12.5  ;\n"
		"\n");
	fclose(ini);
}

int parse_ini_file(char *ini_name)
{
	dictionary *ini;

	/* Some temporary variables to hold query results */
	int b;
	int i;
	double d;
	const char *s;

	ini = iniparser_load(ini_name);
	if (ini == NULL) {
		fprintf(stderr, "cannot parse file: %s\n", ini_name);
		return -1;
	}
	iniparser_dump(ini, stderr);

	/* Get pizza attributes */
	printf("Pizza:\n");

	b = iniparser_getboolean(ini, "pizza:ham", -1);
	printf("Ham:       [%d]\n", b);
	b = iniparser_getboolean(ini, "pizza:mushrooms", -1);
	printf("Mushrooms: [%d]\n", b);
	b = iniparser_getboolean(ini, "pizza:capres", -1);
	printf("Capres:    [%d]\n", b);
	b = iniparser_getboolean(ini, "pizza:cheese", -1);
	printf("Cheese:    [%d]\n", b);
	b = iniparser_getboolean(ini, "pizza:notfind", -1);
	printf("Notfind:   [%d]\n", b);

	/* Get wine attributes */
	printf("Wine:\n");
	s = iniparser_getstring(ini, "wine:grape", NULL);
	printf("Grape:     [%s]\n", s ? s : "UNDEF");

	i = iniparser_getint(ini, "wine:year", -1);
	printf("Year:      [%d]\n", i);

	s = iniparser_getstring(ini, "wine:country", NULL);
	printf("Country:   [%s]\n", s ? s : "UNDEF");

	d = iniparser_getdouble(ini, "wine:alcohol", -1.0);
	printf("Alcohol:   [%g]\n", d);

	iniparser_freedict(ini);
	return 0;
}