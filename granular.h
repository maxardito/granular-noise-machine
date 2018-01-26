/***************************************************
 GRANULAR.H

 Main program

 ***************************************************/

#ifndef __GRANULAR_H
#define __GRANULAR_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <sndfile.h>
#include <portaudio.h>
#include <time.h>
#include <ncurses.h>
#include <dirent.h>

#define FRAMES_PER_BUFFER   512 /* must be divisible by 2 -- see fade-out window code */
#define MAX_PARAM_LENGTH    20
#define MAX_CHN             2
#define MAX_OFILES          9

/*libsndfile data structure*/
struct BUF_tag {
    float *buf[MAX_CHN];
    SNDFILE *sndfile[MAX_OFILES], *isfile, *osfile[MAX_OFILES];
    SF_INFO sfinfo[MAX_OFILES], isfinfo, osfinfo[MAX_OFILES];
    unsigned int num_chan;
    int selection;
};

typedef struct BUF_tag Buf;

#endif /* __GRANULAR_H */
