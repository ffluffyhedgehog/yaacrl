#include "yaacrl.h"

#define DR_WAV_IMPLEMENTATION  // needed to compile drwav
#include "./dr_wav/dr_wav.h"
#include "fingerprint.h"
#include <string.h>
#include <algorithm>
#include "iostream"
#include "sha1/sha1.h"
#include <map>

Yaacrl::Yaacrl(std::string login, std::string pass) {
    db = new Database(login.c_str(), pass.c_str());
    db->setup();
}

Yaacrl::~Yaacrl() {
    delete(db);
}

int Yaacrl::clear_database() {
    int status = db->drop_tables();
    return status;
}

int Yaacrl::fingerprints_to_file(std::string filename) {
    const char * input = filename.c_str();
    drwav* pWav = drwav_open_file(input);
    if (pWav == NULL) {
        return -1;
    }
    float* pSampleData = (float*) malloc((size_t)pWav->totalSampleCount * sizeof(float));
    drwav_read_f32(pWav, pWav->totalSampleCount, pSampleData);
    PeakHashCollection * hashes = fingerprint(pSampleData,
                                              pWav->totalSampleCount);
    drwav_close(pWav);
    free(pSampleData);
    std::string output = filename + std::string(".fingerprints");
    FILE * f = fopen(output.c_str(), "w");
    if (f == NULL) {
        return -2;
    }
    for (int i = 0; i < hashes->count; i++)
        fprintf(f, "%s -> %d\n", hashes->peak_hashes[i].hash,
                hashes->peak_hashes[i].time);
    fclose(f);
    free((*hashes).peak_hashes);
    free(hashes);
    return 0;
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

std::map<std::string, int> Yaacrl::recognize_wav(std::string filename_string) {
    const char * filename = filename_string.c_str();
    drwav* pWav = drwav_open_file(filename);
    if (pWav == NULL) {
        std::map<std::string, int> res;
        res["success"] = 0;
        return res;
    }
    float* pSampleData = (float*) malloc((size_t)pWav->totalSampleCount * sizeof(float));
    drwav_read_f32(pWav, pWav->totalSampleCount, pSampleData);
    PeakHashCollection * hashes = fingerprint(pSampleData,
                                              pWav->totalSampleCount);

    drwav_close(pWav);
    free(pSampleData);


    return recognize_hashes(hashes);
}


std::map<std::string, int> Yaacrl::recognize_fingerprints(std::string filename) {
    const char * input = filename.c_str();
    FILE * f = fopen(input, "r");
    if (f == NULL) {
        std::map<std::string, int> res;
        res["success"] = 0;
        return res;
    }

    char print[21];
    int time;
    int START_ARRAY_SIZE = 200;
    int real_count = 0;
    PeakHashCollection * hashes = (PeakHashCollection *) malloc(sizeof(PeakHashCollection));
    hashes->count = START_ARRAY_SIZE;
    hashes->peak_hashes = (PeakHash *) malloc(hashes->count * sizeof(PeakHash));
    while(fscanf(f, "%20s -> %d", print, &time) == 2) {
        if (real_count + 10 > hashes->count) {
            hashes->count += 100;
            hashes->peak_hashes = (PeakHash *) realloc(hashes->peak_hashes, hashes->count * sizeof(PeakHash));
        }
        strncpy(hashes->peak_hashes[real_count].hash, print, 21);
        hashes->peak_hashes[real_count].time = time;
        real_count++;
    }
    hashes->count = real_count;
    hashes->peak_hashes = (PeakHash *) realloc(hashes->peak_hashes, hashes->count * sizeof(PeakHash));
    fclose(f);
    return  recognize_hashes(hashes);
}


std::map<std::string, int> Yaacrl::recognize_hashes(PeakHashCollection * hashes) {
    PeakHashCollection matches;
    int * matches_ids;
    db->return_matches(hashes, &matches, &matches_ids);
    std::cout << "total samples2: " << hashes->count;
    std::cout << "total samples1: " << matches.count;
    std::map <std::string, int> hashes_map;
    std::map <std::string, int> matches_map;
    std::string hash;
    for (int i = 0; i < hashes->count; i++) {
        hash = std::string(hashes->peak_hashes[i].hash);
        hashes_map[hash] =  hashes->peak_hashes[i].time;
        hash = std::string(matches.peak_hashes[i].hash);
        matches_map[hash] =  matches.peak_hashes[i].time;
    }
    int * matches_diffs = (int *) malloc(sizeof(int) * matches.count);
    for (int i = 0; i < matches.count; i++) {
        hash = std::string(hashes->peak_hashes[i].hash);
        matches_diffs[i] = matches_map[hash] - hashes_map[hash];
    }
    hashes_map.clear();
    matches_map.clear();


    std::map< int,std::map<int,int> > diff_counter;
    int max_count = 0;
    int max_diff = 0;
    int max_id = -1;
    int current_diff;
    for (int i = 0; i < matches.count; i++) {
        if (diff_counter.find(matches_diffs[i]) == diff_counter.end()) {
            diff_counter[matches_diffs[i]] = {};
        }
        else if (diff_counter[matches_diffs[i]].find(matches_ids[i]) == diff_counter[matches_diffs[i]].end()) {
            diff_counter[matches_diffs[i]][matches_ids[i]] = 0;
        }
        diff_counter[matches_diffs[i]][matches_ids[i]] += 1;
        if (diff_counter[matches_diffs[i]][matches_ids[i]] > max_count) {
            max_count = diff_counter[matches_diffs[i]][matches_ids[i]];
            max_diff = matches_diffs[i];
            max_id= matches_ids[i];
        }
    }
    for(std::map<int, std::map<int,int> >::const_iterator it = diff_counter.begin(); it != diff_counter.end(); ++it)
    {
        std::cout << it->first << "\n";
        for (std::map<int,int>::const_iterator iter = it->second.begin(); iter != it->second.end(); ++iter)
            std::cout << "\t" << iter->first << ":" << iter->first << std::endl;
    }
    free(matches.peak_hashes);
    free(matches_ids);
    free((*hashes).peak_hashes);
    free(hashes);
    std::map <std::string, int> result;
    result["success"] = 1;
    result["id"] = max_id;
    result["offset"] = max_diff * DEFAULT_WINDOW_SIZE * DEFAULT_OVERLAP_RATIO / DEFAULT_FS ;
    result["percentage"] = 100 * max_count / matches.count;
    return result;
}

std::string Yaacrl::get_song_by_id(int sid) {
    return db->get_song_by_id(sid);
}
