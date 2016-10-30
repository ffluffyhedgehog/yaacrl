// File used for testing 
// Trying to get fingerprint on snaar_cut.wav

#include "./fingerprint.h"
#include "./decoder.h"
#include <stdlib.h>

int main() {
	const char * wavname = "snaar_cut.wav";
	wave * audio = decode(wavname);
	PeakHashCollection * hashes = fingerprint(audio->samples,
											  audio->num_samples);
	free((*hashes).peak_hashes);
	free(hashes);
	return 0;
}