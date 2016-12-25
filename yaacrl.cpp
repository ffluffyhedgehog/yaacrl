#include "yaacrl.h"

#define DR_WAV_IMPLEMENTATION  // needed to compile drwav
#include "./dr_wav/dr_wav.h"
#include "fingerprint.h"
#include <string.h>
#include "iostream"
#include "sha1/sha1.h"
#include <map>

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

void Yaacrl::fingerprint_file(char * filename) {
    drwav* pWav = drwav_open_file(filename);
    if (pWav == NULL) {
        return;
    }
    float* pSampleData = (float*) malloc((size_t)pWav->totalSampleCount * sizeof(float));
    drwav_read_f32(pWav, pWav->totalSampleCount, pSampleData);
    PeakHashCollection * hashes = fingerprint(pSampleData,
                                              pWav->totalSampleCount);
    //printf("LEN IS %d", hashes->count);
    //for (int i = 0; i < hashes->count; i++) {
    //    printf("%s -> %d\n", hashes->peak_hashes[i].hash, hashes->peak_hashes[i].time);
    //}
    drwav_close(pWav);
    // Inserting song
    SHA1Context sha1;
    SHA1Reset(&sha1);
    SHA1Input(&sha1, (const unsigned char *) filename, strlen(filename));
    char hash[41];
    if (SHA1Result(&sha1)) {
        SHA1Hex(&sha1, hash);
    }
    int sid = db->insert_song(filename, hash);
    // Inserting hashes
    db->insert_hashes(sid, hashes);
    db->set_song_fingerprinted(sid);

    free(pSampleData);
    free((*hashes).peak_hashes);
    free(hashes);
}

void Yaacrl::recognize_file(char * filename) {
    drwav* pWav = drwav_open_file(filename);
    if (pWav == NULL) {
        return;
    }
    float* pSampleData = (float*) malloc((size_t)pWav->totalSampleCount * sizeof(float));
    drwav_read_f32(pWav, pWav->totalSampleCount, pSampleData);
    PeakHashCollection * hashes = fingerprint(pSampleData,
                                              pWav->totalSampleCount);
    PeakHashCollection matches;
    int * matches_ids;
    db->return_matches(hashes, &matches, &matches_ids);
    std::map <char *, int> hashes_map;
    for (int i = 0; i < hashes->count; i++) {
        hashes_map[hashes->peak_hashes[i].hash] =  hashes->peak_hashes[i].time;
    }
    int * matches_diffs = (int *) malloc(sizeof(int) * matches.count);
    for (int i = 0; i < matches.count; i++) {
        matches_diffs[i] = matches.peak_hashes[i].time - hashes_map[matches.peak_hashes[i].hash];
    }
    hashes_map.clear();
    std::map< int,std::map<int,int> > diff_counter;
    int max_count = 0;
    int max_diff = 0;
    int max_id = 0;
    for (int i = 0; i < matches.count; i++) {
        if (diff_counter.find(matches_diffs[i]) == diff_counter.end()) {
            diff_counter[matches_diffs[i]] = {};
        }
        else if (diff_counter[matches_diffs[i]].find(matches_ids[i]) == diff_counter[matches_diffs[i]].end()) {
            diff_counter[matches_diffs[i]][matches_ids[i]] = 0;
        }
        diff_counter[matches_diffs[i]][matches_ids[i]] += 1;
        if (diff_counter[matches_diffs[i]][matches_ids[i]] > max_count) {
            max_count = diff_counter[matches_diffs[i]][matches_ids[i]] = 0;
            max_diff = matches_diffs[i];
            max_id= matches_ids[i];
        }
    }
    std::cout << "-----\nMax id is" << max_id << " \n------\n";
    drwav_close(pWav);
    free(pSampleData);
    free((*hashes).peak_hashes);
    free(hashes);

}
