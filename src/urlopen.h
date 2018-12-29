#ifndef _URLOPEN_H_
#define _URLOPEN_H_

#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <curl/curl.h>

typedef enum {
    M_GET_RESPONSE,
    M_GET_FILE,
}M_URLOPEN_TYPE;


/*
 * url:     const char *, target url
 * out:     char **,      out response, must be freed
 * out_len: uint32_t *,   out len
 */
#define url_get(url, out, out_len) m_urlget(url, out, out_len, M_GET_RESPONSE)

/*
 * url: const char *, target url
 * out: char *,       file name
 */
#define url_download(url, outfile) m_urlget(url, outfile, NULL, M_GET_FILE)












bool m_urlget(const char *url, void *out, uint32_t *out_len, M_URLOPEN_TYPE type);



#endif

