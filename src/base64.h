#ifndef _BASE64_H_
#define _BASE64_H_





char * base64_encode( const char * bindata, char * base64, int binlength );
int base64_decode( const char * base64, char * bindata );


#endif

