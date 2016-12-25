#include "yaacrl.h"
#include "fingerprint.h"
#include "decoder.h"
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
    char * wavname = "snaar_cut.wav";
    printf("ALL GOOD!");
    //wave * audio = decode(wavname);
    //free(bouree);
    return;
    PeakHashCollection * hashes = fingerprint(audio->samples,
                                              audio->num_samples);
    for (int i = 0; i < hashes->count; i++)
        std::cout <<"Hash:" <<  hashes->peak_hashes[i].hash << " Offset:" << hashes->peak_hashes[i].time << std::endl;
    free((*hashes).peak_hashes);
    free(hashes);
}
