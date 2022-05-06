#include <stdio.h>
#include <glib.h>
#include <string.h>


int h_remove(gpointer key, gpointer value, gpointer user_data) {
	char * dev = (char *)key;
	if (strlen(dev) > 2 && dev[0] == 's' && dev[1] == 'd') {
		printf("%s\n", (char *)key);
		return 1;
	}
	return 0;
}

void print_key_value(gpointer key, gpointer value, gpointer user_data) {
	printf("%s ---> %s\n", (char *)key, (char *)value);
}

int
main() {
	char *a[8] = {"sda", "sdb", "sdc", "loop-1", "sdd", "dm-1", "dm-2", "dm-3"};
	GHashTable *hash = NULL;

	hash = g_hash_table_new(g_str_hash, g_str_equal);

	for(int i = 0; i < 8; i++) {
		g_hash_table_insert(hash, a[i], a[i]);
	}

	g_hash_table_foreach_remove(hash, h_remove, NULL);
	g_hash_table_foreach(hash, print_key_value, NULL);

	printf("start \n");
}
