#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <curl/curl.h>
#include <jansson.h>

#include "config.h"
#include "base64.h"
#include "urlopen.h"

struct response{
    char *buff;
    size_t size;
};

static size_t m_write_response(void *response, size_t size, size_t nmemb, void *userp)
{
    size_t realsize = size * nmemb;
    struct response *resp = (struct response *)userp;

    resp->buff = realloc(resp->buff, resp->size + realsize + 1);
    if(resp->buff == NULL) {
        log_msg(E_DEBUG, "not enough memory (realloc returned NULL)");
        return 0;
    }

    memcpy(&(resp->buff[resp->size]), response, realsize);
    resp->size += realsize;
    resp->buff[resp->size] = 0;

    return realsize;
}

static size_t m_write_file(void *ptr, size_t size, size_t nmemb, void *stream)
{
    size_t written = fwrite(ptr, size, nmemb, (FILE *)stream);
    return written;
}

bool m_urlget(const char *url, void *out, uint32_t *out_len, M_URLOPEN_TYPE type)
{
    bool ret = false;
    struct response resp;
    char rpc_url[1024];
    FILE *pagefile = NULL;
    char *pagefilename = NULL;

    memset(rpc_url, 0, sizeof(rpc_url));

    if(url==NULL || out==NULL)
        return false;

    if(curl_global_init(CURL_GLOBAL_ALL) != CURLE_OK)
        return false;

    CURL *curl = curl_easy_init();
    if(curl == NULL) {
        log_msg(E_DEBUG, "Failed to init curl");
        goto _RET_ERROR;
    }

    strncpy(rpc_url, url, sizeof(rpc_url));
    switch(type) {
        case M_GET_RESPONSE:
            resp.buff = malloc(1);
            resp.size = 0;
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, m_write_response);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&resp);
            break;
        case M_GET_FILE:
            pagefilename = out;
            pagefile = fopen(pagefilename, "wb");
            if(pagefile == NULL) {
                log_msg(E_DEBUG, "Failed to open file:%s", pagefilename);
                goto _RET_ERROR;
            }
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, m_write_file);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, pagefile);
            break;
    }

    log_msg(E_DEBUG, "rpc_url:%s", rpc_url);

    curl_easy_setopt(curl, CURLOPT_URL, rpc_url);
    curl_easy_setopt (curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt (curl, CURLOPT_NOSIGNAL, 1L);
    curl_easy_setopt(curl, CURLOPT_FAILONERROR, 1L);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "miner/1.0");

    CURLcode code = curl_easy_perform(curl);

    if(code != CURLE_OK) {
        log_msg(E_DEBUG, "Curl get failed:%s", curl_easy_strerror(code));
        if(type == M_GET_FILE)
        {
            fclose(pagefile);
            remove(pagefilename);
        }
    }
    else {
        switch(type) {
            case M_GET_RESPONSE:
                log_msg(E_DEBUG, "response %lu bytes:%s", (long)resp.size, resp.buff);
                *(char **)out = resp.buff;
                if(out_len)
                    *out_len = resp.size;
                break;
            case M_GET_FILE:
                fclose(pagefile);
                break;
        }

        ret = true;
    }

_RET_ERROR:
    curl_easy_cleanup(curl);
    curl_global_cleanup();

    return ret;
}

