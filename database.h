#ifndef YAACRL_DATABASE_H
#define YAACRL_DATABASE_H

#include "mysql.h"
#include "fingerprint.h"

#define FIELD_FILE_SHA1 "file_sha1"
#define FIELD_SONG_ID "song_id"
#define FIELD_SONGNAME "song_name"
#define FIELD_OFFSET "offset"
#define FIELD_HASH  "hash"

class Database {
    MYSQL *connection;
public:
    Database();
    void get_songs();
    int insert_song(char *, char hash[41]);
    void insert_hashes(int, PeakHashCollection *);
    void set_song_fingerprinted(int);
    void return_matches(PeakHashCollection * to_recognize, PeakHashCollection * matches, int ** );
    void get_song_by_id(int sid);
    //void delete_unfingerprinted_songs();
    //void get_num_songs();
    //void get_num_fingerprints();
    //void insert(hash, sid, offset);
    //void query(hash);
    //void get_iterable_kv_pairs();



    ~Database();
};


#endif //YAACRL_DATABASE_H
