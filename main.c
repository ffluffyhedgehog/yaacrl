// File used for testing
// Trying to get fingerprint on snaar_cut.wav

#include <stdlib.h>
#include "./fingerprint.h"

#define DR_WAV_IMPLEMENTATION  // needed to compile drwav
#include "./dr_wav/dr_wav.h"

int main(int argc, char** argv) {
	if (argc == 1)
		return -1;
    drwav* pWav = drwav_open_file(argv[1]);
	if (pWav == NULL) {
		return -1;
	}
	float* pSampleData = (float*) malloc((size_t)pWav->totalSampleCount * sizeof(float));
	drwav_read_f32(pWav, pWav->totalSampleCount, pSampleData);
	PeakHashCollection * hashes = fingerprint(pSampleData,
                                              pWav->totalSampleCount);
	//GETTING HASHES
    FILE * f = fopen("res.test.txt", "w");
    for (int i = 0; i < hashes->count; i++)
        fprintf(f, "%s -> %d\n", hashes->peak_hashes[i].hash,
                                 hashes->peak_hashes[i].time);
    fclose(f);
	drwav_close(pWav);
    free((*hashes).peak_hashes);
    free(hashes);
    return 0;
}
