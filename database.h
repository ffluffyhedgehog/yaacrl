#ifndef YAACRL_DATABASE_H
#define YAACRL_DATABASE_H

#include "mysql/mysql.h"
#include "string"
#include "fingerprint.h"

#define FIELD_FILE_SHA1 "file_sha1"
#define FIELD_SONG_ID "song_id"
#define FIELD_SONGNAME "song_name"
#define FIELD_OFFSET "offset"
#define FIELD_HASH  "hash"

class Database {
    MYSQL *connection;
public:
    Database(const char *, const char *);
    ~Database();
    int setup();
    int drop_tables();

    int insert_song(char *, char hash[41]);
    int insert_hashes(int, PeakHashCollection *);
    int set_song_fingerprinted(int);
    int return_matches(PeakHashCollection * to_recognize, PeakHashCollection * matches, int ** );
    std::string get_song_by_id(int sid);


    //void get_songs();
    //void delete_unfingerprinted_songs();
    //void get_num_songs();
    //void get_num_fingerprints();
    //void insert(hash, sid, offset);
    //void query(hash);
    //void get_iterable_kv_pairs();




};


#endif //YAACRL_DATABASE_H
