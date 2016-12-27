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
    db->setup();
}

Yaacrl::~Yaacrl() {
    delete(db);
}

int Yaacrl::clear_database() {
    int status = db->drop_tables();
    return status;
}

int Yaacrl::add_file(std::string filename_string) {
    const char * filename = filename_string.c_str();
    drwav* pWav = drwav_open_file(filename);
    if (pWav == NULL) {
        return -1;
    }
    float* pSampleData = (float*) malloc((size_t)pWav->totalSampleCount * sizeof(float));
    drwav_read_f32(pWav, pWav->totalSampleCount, pSampleData);
    PeakHashCollection * hashes = fingerprint(pSampleData,
                                              pWav->totalSampleCount);
    drwav_close(pWav);
    free(pSampleData);

    // Inserting song
    SHA1Context sha1;
    SHA1Reset(&sha1);
    SHA1Input(&sha1, (const unsigned char *) filename, strlen(filename));
    char hash[41];
    if (SHA1Result(&sha1)) {
        SHA1Hex(&sha1, hash);
    }

    PeakHashCollection matches;
    int * matches_ids;
    db->return_matches(hashes, &matches, &matches_ids);

    int new_song_hashes_count = hashes->count;
    int matched_hashes_count = matches.count;
    if (new_song_hashes_count == matched_hashes_count) {
        free((*hashes).peak_hashes);
        free(hashes);
        free(matches.peak_hashes);
        free(matches_ids);
        return -2;
    }
    int sid = db->insert_song((char*)filename, hash);

    // Inserting hashes
    db->insert_hashes(sid, hashes);

    db->set_song_fingerprinted(sid);
    free((*hashes).peak_hashes);
    free(hashes);
    free(matches.peak_hashes);
    free(matches_ids);
    return sid;
}

int Yaacrl::recognize_file(std::string filename_string) {
    const char * filename = filename_string.c_str();
    drwav* pWav = drwav_open_file(filename);
    if (pWav == NULL) {
        return -1;
    }
    float* pSampleData = (float*) malloc((size_t)pWav->totalSampleCount * sizeof(float));
    drwav_read_f32(pWav, pWav->totalSampleCount, pSampleData);
    PeakHashCollection * hashes = fingerprint(pSampleData,
                                              pWav->totalSampleCount);

    drwav_close(pWav);
    free(pSampleData);

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
    int max_id = -1;
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
    free(matches.peak_hashes);
    free(matches_ids);
    free((*hashes).peak_hashes);
    free(hashes);
    return max_id;
}

std::string Yaacrl::get_song_by_id(int sid) {
    return db->get_song_by_id(sid);
}