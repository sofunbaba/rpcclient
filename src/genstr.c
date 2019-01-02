#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "urlopen.h"
#include "base64.h"
#include "main.h"

DEBUG_LEVEL_E debug_level = E_DEBUG;

#define decode(in, in_len, out, salt) encode(in, in_len, out, salt)

static char *encode(char *in, uint32_t in_len, char *out, char *salt)
{
    if(in==NULL || out==NULL || salt==NULL || in_len==0)
        return NULL;

    uint32_t i=0, j=0;
    uint32_t salt_len = strlen(salt);

    for(i=0,j=0; i<in_len; i++,j++)
    {
       if(j%salt_len == 0)
           j = 0;
       out[i] = in[i] ^ salt[j]*_SL;
    }

    return out;
}

static void print_help(char *name)
{
    printf("usage:%s <OPTION> [STR/FILE]\r\n", name);
    printf("options:\r\n");
    printf("    -f : read from a file.\r\n");
    printf("    -s : encode a str\r\n");

    exit(0);
}

static uint32_t read_from_file(char *filename, char **str)
{
    if(filename==NULL || str==NULL)
        return 0;

    FILE *fp = fopen(filename, "r");
    if(fp == NULL)
        return 0;

    fseek(fp, 0, SEEK_END);
    uint32_t len = ftell(fp);

    *str = malloc(len+1);
    memset(*str, 0, len+1);

    rewind(fp);
    len = fread(*str, 1, len, fp);
    fclose(fp);

    return len;
}

int main(int argc, char *argv[])
{
    char *str = NULL;
    uint32_t len = 0;

    if(argc < 3)
        print_help(argv[0]);

    int i = 0;
    for(i=1; i<argc; i++)
    {
        if(strcmp(argv[i], "-f") == 0)
        {
            char *filename = argv[++i];
            len = read_from_file(filename, &str);
            continue;
        }
        else if(strcmp(argv[i], "-s") == 0)
        {
            str = strdup(argv[++i]);
            len = strlen(str);
            continue;
        }
    }

    if(str == NULL)
        print_help(argv[0]);

    // printf("str:%s\r\n", str);

    char in[len*2];
    char out[len*2];
    memset(in, 0, sizeof(in));
    memset(out, 0, sizeof(out));

    char *salt = SALTSTR;
    // printf("salt:%s\r\n", salt);

    encode(str, len, in, salt);
    base64_encode(in, out, len);

    printf("%s\r\n", out);
    // printf("encode buff:%s\r\n", out);

    len = base64_decode(out, in);

    memset(out, 0, sizeof(out));
    decode(in, len, out, salt);
    // printf("decode buff:%s\r\n", out);

    free(str);

    return 0;
}

