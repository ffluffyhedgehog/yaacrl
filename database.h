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
    ~Database();
};


#endif //YAACRL_DATABASE_H
