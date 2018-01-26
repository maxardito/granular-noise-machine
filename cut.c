#include "granular.h"

void cutAndDistort(SNDFILE *sfile, Buf *ibuf, int channels, Buf *obuf, int startFrame, int endFrame){

    int frames, bitSize;
    int i, j;
    float *ip;

    frames = endFrame - startFrame;

    /*Bit crushing factor is randomly selected between 1 bits and 8 bit*/
    bitSize = rand() % (8 + 1);

    sf_seek(sfile, startFrame, SEEK_SET);

    for (i=0; i<channels; i++) {
        ip = ibuf->buf[i];
        for (j=0; j<frames; j++) {
            obuf->buf[i][j] = roundf(ip[j + startFrame] * bitSize) / bitSize;
        }
    }

}
