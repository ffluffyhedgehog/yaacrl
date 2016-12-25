// File used for testing
// Trying to get fingerprint on snaar_cut.wav

#include <stdlib.h>
#include "./fingerprint.h"
#include "./decoder.h"

int main() {
    char * wavname = "audio/snaar_cut.wav";
    wave * audio = decode(wavname);
    PeakHashCollection * hashes = fingerprint(audio->samples,
                                              audio->num_samples);
    free((*hashes).peak_hashes);
    free(hashes);
    return 0;
}
