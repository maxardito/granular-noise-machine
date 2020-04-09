# granular-noise-sampler

Granular Noise Sampler
by Max Ardito (2016)

Takes an audio file, splices it into grains, distorts the waveforms with
bit crushing, and takes you to an interface where you can play the grains
synchronously using the number keys as a sampler. Have fun making insane
noise music!

Bit downsampling is random within the range of 1-8bits for each grain
Grain length is random between 1 and 1000 frames for each grain


==============================

How to compile: `make`

==============================

Must have following libraries installed:
PortAudio
libsndfile
ncurses

How to install libraries (Mac OSX)
---------------------------
PortAudio:

`cd PortAudio`
`cp portaudio.h /usr/local/include`
`cp libportaudio.dylib /usr/local/lib`

---------------------------
libsndfile:

`brew install libsndfile`

---------------------------

ncurses already installed on Mac OSX

==============================

Command Line Syntax

`./granular violino.wav`

(or any other wav file of your choice!)
