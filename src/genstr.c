#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "urlopen.h"
#include "base64.h"
#include "main.h"

DEBUG_LEVEL_E debug_level = E_DEBUG;

#define decode(in, in_len, out, salt) encode(in, in_len, out, salt)

#define L 0x19
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
       out[i] = in[i] ^ salt[j]*L;
    }

    return out;
}

static void print_help(char *name)
{
    printf("usage:%s <STR> <SALT>\r\n", name);
    exit(0);
}

int main(int argc, char *argv[])
{
    if(argc < 3)
        print_help(argv[0]);

    char *url = argv[1];
    char in[2048];
    char out[2048];
    memset(in, 0, sizeof(in));
    memset(out, 0, sizeof(out));


    char *salt = argv[2];
    printf("salt:%s\r\n", salt);

    uint32_t len = strlen(url);
    encode(url, len, in, salt);
    base64_encode(in, out, len);

    printf("encode buff:%s\r\n", out);

    len = base64_decode(out, in);

    memset(out, 0, sizeof(out));
    decode(in, len, out, salt);
    printf("decode buff:%s\r\n", out);

    return 0;
}

