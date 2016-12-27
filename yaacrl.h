#ifndef YAACRL_YAACRL_H
#define YAACRL_YAACRL_H

#include "database.h"
#include <string>


class Yaacrl {
    Database * db;
public:
    Yaacrl();
    int add_file(std::string);
    //int add_fingerprints(char *, char *) {};
    int recognize_file(std::string);
    //int recognize_fingerprints(char *, char *) {};
    std::string get_song_by_id(int);
    int clear_database();
    ~Yaacrl();
};

#endif //YAACRL_YAACRL_H