#ifndef YAACRL_DATABASE_H
#define YAACRL_DATABASE_H

#include "mysql.h"

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
    //void delete_unfingerprinted_songs();
    //void get_num_songs();
    //void get_num_fingerprints();
    //void set_song_fingerprinted(int sid);
    //void get_song_by_id(int sid);
    //void insert(hash, sid, offset);
    //void insert_song(char * song_name);
    //void query(hash);
    //void get_iterable_kv_pairs();
    //void insert_hashes(sid, hash);
    //void return_matches(hashes);


    ~Database();
};


#endif //YAACRL_DATABASE_H
