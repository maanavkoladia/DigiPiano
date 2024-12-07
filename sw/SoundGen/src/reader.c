/*
#include <stdio.h>

typedef struct waveform {
    unsigned startingOffset;
    unsigned sampleLength;
} waveform;
waveform waveforms[24];

int main() {
    FILE* file = fopen("waveforms.raw", "rb");
    unsigned num_waveforms = 0;
    fread(&num_waveforms, 4, 1, file);
    printf("There are %u waveforms in this file\n", num_waveforms);
    for (unsigned i = 0; i < num_waveforms; i++) {
        fread(&waveforms[i].startingOffset, 4, 1, file);
        fread(&waveforms[i].sampleLength, 4, 1, file);
        printf("Waveform of length %u begins at offset %u\n", waveforms[i].sampleLength, waveforms[i].startingOffset);
    }

    unsigned short samples[256];
    // Print out waveform for 9
    fseek(file, waveforms[23].startingOffset, SEEK_SET);
    fread(samples, 2, 256, file);
    for (int i = 0; i < 256; i++) {
        printf("Sample[%3i]: %hu\n", i, samples[i]);
    }

    fclose(file);
    return 0;
}
*/