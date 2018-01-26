/***************************************************
 CUT.H

 Cuts a speciifed grain of audio from the input file
 and distorts its waveform with a bit crushing algorithm

 ***************************************************/
#ifndef __CUT_H
#define __CUT_H

#include "granular.h"

void cutAndDistort(SNDFILE *sfile, Buf *ibuf, int channels, Buf *obuf, int startFrame, int endFrame);

#endif /* __CUT_H */
