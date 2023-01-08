
#include <stdio.h>
#include <glib.h>
#include <string.h>

static GHashTable * hash_disk;

char *disk[8] = { "sda", "sdb", "sdc", "sdd", "dm-0", "dm-1", "dm-2", "loop0" };

static void print_hash_str_str(gpointer key, gpointer value, gpointer user_data) {
    printf("key:%s, val:%s\n", key, value);
}

static int hash_disk_remove(gpointer key, gpointer value, gpointer user_data) {
    char *dev;
    dev = (char *)value;

    if (strlen(dev) > 2 && dev[0] == 's' && dev[1] == 'd')
        return 0;
    return 1;
}


int main() {
    hash_disk = g_hash_table_new(g_str_hash, g_str_equal);

    int i = 0;
    for (i = 0; i < 8 ; i++) {
        g_hash_table_insert(hash_disk, disk[i], disk[i]);
    }

    g_hash_table_foreach_remove(hash_disk, hash_disk_remove, NULL);
    g_hash_table_foreach(hash_disk, print_hash_str_str, NULL);

    g_hash_table_destroy(hash_disk);
    return 0;
}
