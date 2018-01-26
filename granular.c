#include "granular.h"
#include "cut.h"


/* local function prototypes */
bool read_input(SNDFILE *sfile, SF_INFO *sfinfo, Buf *buf);
bool write_output(SNDFILE *sfile, SF_INFO *sfinfo, Buf *buf, long frames);
void removeGrains();

/* PortAudio callback function protoype */
static int paCallback( const void *inputBuffer, void *outputBuffer,
                      unsigned long framesPerBuffer,
                      const PaStreamCallbackTimeInfo* timeInfo,
                      PaStreamCallbackFlags statusFlags,
                      void *userData );


int main (int argc, char *argv[]){

    char *ifilename, ofilename[12], lfilename[MAX_OFILES][10], ch;
    char outFileName = 49;
    int n = 1, oframes = 0, selection = 0;
    unsigned int startFrame, endFrame, num_chan;
    float seconds;
    srand(time(0));

    /* my data structures */
    Buf ibuf, obuf, *p = &ibuf;

    /* PortAudio data structures */
    PaStream *stream;
    PaError err;
    PaStreamParameters outputParams;
    PaStreamParameters inputParams;


    /* libsndfile data structures */
    SNDFILE *isfile, *osfile[MAX_OFILES];
    SF_INFO isfinfo, osfinfo[MAX_OFILES];

    /* zero libsndfile structures */
    memset(&isfinfo, 0, sizeof(isfinfo));
    memset(&p->isfinfo, 0, sizeof(p->isfinfo));
    memset(&osfinfo, 0, sizeof(osfinfo));

    for (int i=0; i<MAX_OFILES; i++) {
        memset(&p->sfinfo[i], 0, sizeof(p->sfinfo[i]));
    }

    /* zero buffer pointers in Buf structures */
    for (int i=0; i<MAX_CHN; i++) {
        ibuf.buf[i] = 0;
        obuf.buf[i] = 0;
    }


    /*initialize*/
    ifilename = NULL;

    /*command line parsing*/
    if (argc != 2) {
        printf("Program arguments: ./main ifile.wav\n");
        return -1;
    }

    ifilename = argv[1];
    strcpy(ofilename, "grain1.wav");


    /*open auxiliary input file*/
    if ( (isfile = sf_open (ifilename, SFM_READ, &isfinfo)) == NULL ) {
        fprintf (stderr, "Error: could not open wav file: %s\n", ifilename);
        return -1;
    }


    /*open playback input file*/
    if ( (p->isfile = sf_open (ifilename, SFM_READ, &p->isfinfo)) == NULL ) {
        fprintf (stderr, "Error: could not open wav file: %s\n", ifilename);
        return -1;
    }



    /*Too many channels*/
    if (isfinfo.channels > MAX_CHN){
        fprintf (stderr, "Error: number of channels must be less than 2\n");
        return -1;
    }

    /*send number of channels to callback*/
    num_chan = isfinfo.channels;
    p->num_chan = num_chan;

    /*allocate memory*/
    for (int i=0; i<isfinfo.channels; i++) {

        if  ((ibuf.buf[i] = (float *)malloc(sizeof(float)*isfinfo.frames)) == NULL) {
            printf("Error allocating memory for input buffer\n");
            return -1;
        }

    }

    /*Read input/write output*/
    if ( !read_input(isfile, &isfinfo, &ibuf) ) {
        fprintf(stderr, "ERROR: Cannot read input file %s", ifilename);
        return -1;
    }

    /*Calculate file length in seconds*/
    seconds = (isfinfo.frames / isfinfo.samplerate);

    /*Clear window*/
    system("clear");

    /* Print input file information */
    printf("Input audio file %s:\n\n\nFrames: %d\n Channels: %d\n Samplerate: %d\n Length: %dm:%ds\n",
           ifilename, (int)isfinfo.frames, isfinfo.channels, isfinfo.samplerate, (int)seconds / 60, (int)seconds % 60);
           sleep(2);
           printf("Extracting grains...\n");


    /*Extract grains*/
    for(int i = 0; i < 9; i++){

        /*Calculate grain lengths */
        oframes = ((rand() % 10000 + 1);

        /*Partition frames in audio file*/
        endFrame = rand() % isfinfo.frames + 1;
        startFrame = endFrame - oframes;

        /*outfile parameters*/
        osfinfo[n].samplerate = isfinfo.samplerate;
        osfinfo[n].channels = isfinfo.channels;
        osfinfo[n].format = isfinfo.format;

        /*output file names*/
        ofilename[5] = outFileName;

        /*open output file*/
        if ( (osfile[n] = sf_open (ofilename, SFM_WRITE, &osfinfo[n])) == NULL ) {
            printf ("Error : could not open wav file : %s\n", ofilename);
            return -1;
        }

        /*Allocate output memory*/
        for (int i=0; i<isfinfo.channels; i++){
            if ((obuf.buf[i] = (float *)malloc(sizeof(float)*oframes)) == NULL) {
                printf("Error allocating memory for output buffer\n");
                return -1;
            }
        }

        /*Cut to specified length*/
        cutAndDistort(isfile, &ibuf, isfinfo.channels, &obuf, startFrame, endFrame);

        /*Write output file*/
        if ( !write_output(osfile[n], &osfinfo[n], &obuf, oframes) ) {
            fprintf(stderr, "ERROR: Cannot write output file %s", ofilename);
            return -1;
        }

        /*Close output files and free storage*/
        sf_close(osfile[n]);

        for (int i=0; i<MAX_CHN; i++){
            if (obuf.buf[i] != NULL)
                free(obuf.buf[i]);
        }

        /*increment variables*/
        n++;
        outFileName++;
    }


    /*Reset grain number variable*/
    outFileName = 49;

    /* open list of files */
    printf("Input files:\n");
        for (int i=0; i<9; i++) {

            strcpy(lfilename[i], "grain1.wav");
            lfilename[i][5] = outFileName;

            /* open audio file */
            if ((p->sndfile[i + 1] = sf_open(lfilename[i], SFM_READ, &p->sfinfo[i])) == NULL) {
              fprintf (stderr, "Error: could not open wav file: %s\n", lfilename[i]);
            }

            /* Print file information */
            printf("%d %s Frames: %8d, Channels: %d, Samplerate: %d\n",
                i+1, lfilename[i], (int)p->sfinfo[i].frames, p->sfinfo[i].channels, p->sfinfo[i].samplerate);

            outFileName++;

        }
        sleep(3);



    /* Reinitializing PortAudio */
    err = Pa_Initialize();
    if (err != paNoError) {
        printf("PortAudio error: %s\n", Pa_GetErrorText(err));
        printf("\nExiting.\n");
        return -1;
    }

    /* Input stream parameters */
    inputParams.device = Pa_GetDefaultInputDevice();
    inputParams.channelCount = 1;
    inputParams.sampleFormat = paFloat32;
    inputParams.suggestedLatency =
    Pa_GetDeviceInfo(inputParams.device)->defaultLowInputLatency;
    inputParams.hostApiSpecificStreamInfo = NULL;

    /* Ouput stream parameters */
    outputParams.device = Pa_GetDefaultOutputDevice();
    outputParams.channelCount = 2;
    outputParams.sampleFormat = paFloat32;
    outputParams.suggestedLatency =
    Pa_GetDeviceInfo(outputParams.device)->defaultLowOutputLatency;
    outputParams.hostApiSpecificStreamInfo = NULL;

    /* Open audio stream */
    err = Pa_OpenStream(&stream,
                        &inputParams, /* no input */
                        &outputParams,
                        isfinfo.samplerate, FRAMES_PER_BUFFER,
                        paNoFlag, /* flags */
                        paCallback,
                        &ibuf);

    if (err != paNoError) {
        printf("PortAudio error: open stream: %s\n", Pa_GetErrorText(err));
        printf("\nExiting.\n");
        return -1;
    }

    /* Start audio stream */
    err = Pa_StartStream(stream);
    if (err != paNoError) {
        printf(  "PortAudio error: start stream: %s\n", Pa_GetErrorText(err));
        printf("\nExiting.\n");
        return -1;
    }




    /**Sampler Interface**/
    initscr(); /* Start curses mode */
    cbreak();  /* Line buffering disabled */
    noecho(); /* Uncomment this if you don't want to echo characters when typing */


    printw("SAMPLER: Use numbers 1-9 to toggle through grains\n");
    printw("********************************************\n\n");
    printw("Select input file by number:\n");
    printw("0: Original File\n");
    for (int i=0; i<n; i++) {
        printw("%d: Grain File %d\n", i, i);
    }
    printw("Enter q to quit\n");
    mvprintw(6, 0, "               Selection: ");
    refresh();
    selection = 0;
    ch = '\0'; /* Init ch to null character */
    while (ch != 'q') {
        ch = tolower(getch());
        if (ch >= '1' && ch < '1' + n - 1) {
            selection = ch-'1' + 1;
            p->selection = selection;
        }
        mvprintw(6, 0, "Playing %d, New selection: ", selection);
        refresh();
    }

    /* End curses mode  */
    endwin();


    /* Stop stream */
    err = Pa_StopStream(stream);
    if (err != paNoError) {
        printf(  "PortAudio error: stop stream: %s\n", Pa_GetErrorText(err));
        printf("\nExiting.\n");
        exit(1);
    }

    /* Close stream */
    err = Pa_CloseStream(stream);
    if (err != paNoError) {
        printf(  "PortAudio error: close stream: %s\n", Pa_GetErrorText(err));
        printf("\nExiting.\n");
        exit(1);
    }

    /* Terminate PortAudio */
    err = Pa_Terminate();
    if (err != paNoError) {
        printf("PortAudio error: terminate: %s\n", Pa_GetErrorText(err));
        printf("\nExiting.\n");
        exit(1);
    }


    /* Close files */
    for (int i=0; i<n; i++) {
        sf_close (p->sndfile[i]);
    }

    /* Remove grain files */
    removeGrains();

    return 0;
}






/***************************************************************************
***************************************************************************
***************************************************************************/





/* This routine will be called by the PortAudio engine when audio is needed.
 * It may called at interrupt level on some machines so don't do anything
 * in the routine that requires significant resources.
 */
static int paCallback(const void *inputBuffer, void *outputBuffer,
                      unsigned long framesPerBuffer,
                      const PaStreamCallbackTimeInfo* timeInfo,
                      PaStreamCallbackFlags statusFlags,
                      void *userData) {
    Buf *p = (Buf *)userData; /* Cast data passed through stream to our structure. */
    float *output = (float *)outputBuffer;
    unsigned int num_samples = framesPerBuffer * p->num_chan; /* number of samples in buffer */
    unsigned int selection, count;

    selection = p->selection;

        count = sf_read_float (p->sndfile[selection], output, num_samples);
        if (count < num_samples) {

            sf_seek(p->sndfile[selection], framesPerBuffer, SEEK_SET);

            sf_read_float (p->sndfile[selection], output, num_samples);

        }

    return 0;
}







/*READ INPUT*/
bool read_input(SNDFILE *sfile, SF_INFO *sfinfo, Buf *buf)
{
    int i, j = 0, count;
    float frame_buf[MAX_CHN]; /* to hold one sample frame of audio data */
    for (i=0; i<sfinfo->frames; i++) {
        /* for each frame */
        if ( (count = sf_read_float (sfile, frame_buf, sfinfo->channels)) != sfinfo->channels) {
            fprintf(stderr, "Error: on sample frame %d\n", i);
            return false;
        }
        //ToDo: de-interleave the frame[j] into separate channel buffers buf->buf[j][i]
        for (j = 0; j < sfinfo->channels; j++){
            buf->buf[j][i] = frame_buf[j];
        }

    }
    return true;
}








/*WRITE OUTPUT*/
bool write_output(SNDFILE *sfile, SF_INFO *sfinfo, Buf *buf, long frames)
{
    int i, j, count;
    float frame_buf[MAX_CHN]; /* to hold one sample frame of audio data */
    for (i = 0; i < frames; i++) {
        /* for each frame */
        //ToDo: interleave separate channel buffers buf->buf[j][i] into a frame_buf[j]
        for (j = 0; j < sfinfo->channels; j++){
            frame_buf[j] = buf->buf[j][i];
        }

        if ( (count = sf_write_float (sfile, frame_buf, sfinfo->channels)) != sfinfo->channels) {
            fprintf(stderr, "Error: on sample frame %d\n", i);
            return false;
        }
    }
    return true;
}








/*REMOVE PREVIOUSLY EXTRACTED GRAINS*/
void removeGrains(){
  DIR *d = opendir(".");
  struct dirent *dir;
  if (d) {
     while ((dir = readdir(d)) != NULL) {
         if(strcmp(dir->d_name, "grain1.wav") == 0){
           remove(dir->d_name);
         }
         if(strcmp(dir->d_name, "grain2.wav") == 0){
           remove(dir->d_name);
         }
         if(strcmp(dir->d_name, "grain3.wav") == 0){
           remove(dir->d_name);
         }
         if(strcmp(dir->d_name, "grain4.wav") == 0){
           remove(dir->d_name);
         }
         if(strcmp(dir->d_name, "grain5.wav") == 0){
           remove(dir->d_name);
         }
         if(strcmp(dir->d_name, "grain6.wav") == 0){
           remove(dir->d_name);
         }
         if(strcmp(dir->d_name, "grain7.wav") == 0){
           remove(dir->d_name);
         }
         if(strcmp(dir->d_name, "grain8.wav") == 0){
           remove(dir->d_name);
         }
         if(strcmp(dir->d_name, "grain9.wav") == 0){
           remove(dir->d_name);
         }
     }
     closedir(d);
  }
}
