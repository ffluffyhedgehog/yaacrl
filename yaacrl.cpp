#include "yaacrl.h"

#define DR_WAV_IMPLEMENTATION  // needed to compile drwav
#include "./dr_wav/dr_wav.h"
#include "fingerprint.h"
#include "iostream"

Yaacrl::Yaacrl() {
    db = new Database();
    std::cout << "Yaacrl started, db connected\n";
}

Yaacrl::~Yaacrl() {
    delete(db);
    std::cout << "Yaacrl finished, db connection closed\n";
}

void Yaacrl::init() {
    db->get_songs();
}

void Yaacrl::fingerprint_file() {
    drwav* pWav = drwav_open_file("audio/snaar_cut.wav");
    if (pWav == NULL) {
        return;
    }
    float* pSampleData = (float*) malloc((size_t)pWav->totalSampleCount * sizeof(float));
    drwav_read_f32(pWav, pWav->totalSampleCount, pSampleData);
    PeakHashCollection * hashes = fingerprint(pSampleData,
                                              pWav->totalSampleCount);
    //GETTING HASHES
    for (int i = 0; i < hashes->count; i++)
        printf("%s -> %d\n", hashes->peak_hashes[i].hash,
               hashes->peak_hashes[i].time);
    drwav_close(pWav);
    free((*hashes).peak_hashes);
    free(hashes);
}
