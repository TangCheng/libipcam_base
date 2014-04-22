#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "utils.h"

void ipcam_get_rand_num(guint *num, guint n_len)
{
    FILE* random_data = fopen("/dev/urandom", "r");
    guint i = 0;
    while (i < n_len)
    {
        ssize_t result = fread((gchar *)(num + i), sizeof(guint), n_len -i, random_data);
        i += result;
    }
    fclose(random_data);
}

void ipcam_get_rand_str(gchar *str, guint n_len)
{
    static char table[64] = "00123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz\0";
    guint i = 0;
    char ss[2] = {'\0', '\0'};
    str[0] = '\0';
    guint *num = g_new(guint, n_len);
    ipcam_get_rand_num(num, n_len);
    for (i = 0; i < n_len; i++)
    {
        sprintf(ss, "%c", table[(num[i] % 62) + 1]);
        strcat(str, ss);
    }
    g_free(num);
}
