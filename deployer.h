#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h> 
#include <getopt.h>
#include <bzlib.h>
#include <unistd.h>
#include <syslog.h>
#include <curl/curl.h>
#include <curl/easy.h>
#define _ERR 1
#define DP_OK  0
#define DP_ERR 1
#define MAX_BUFF_LENGTH 1024
#define CHUNK 4096


struct globalArgs {

    unsigned char *url; //URL param
    unsigned char *dest; // write destenation
    unsigned char *md5; // md5summ
    unsigned char *image; //name of deployment image
    size_t errno;
  };


bz_stream strm;

size_t decompressData(char *ptr, size_t size, size_t nmemb, FILE *);
size_t downloadData(struct globalArgs *);
size_t preConfigure(int argc, char *argv[], struct globalArgs *);
size_t getMAC();
size_t getConfig(char *ptr, size_t size, size_t nmemb, struct globalArgs *);





