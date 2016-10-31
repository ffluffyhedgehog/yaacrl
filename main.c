// File used for testing
// Trying to get fingerprint on snaar_cut.wav

#include <stdlib.h>
#include "./fingerprint.h"
#include "./decoder.h"

int main() {
    char * wavname = "snaar_cut.wav";
    wave * audio = decode(wavname);
    PeakHashCollection * hashes = fingerprint(audio->samples,
                                              audio->num_samples);
    // GETTING HASHES
    FILE * f = fopen("res.test.txt", "w");
    for (int i = 0; i < hashes->count; i++)
        fprintf(f, "%s -> %d\n", hashes->peak_hashes[i].hash,
                                 hashes->peak_hashes[i].time);
    fclose(f);
    free((*hashes).peak_hashes);
    free(hashes);
    return 0;
}
