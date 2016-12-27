#ifndef YAACRL_YAACRL_H
#define YAACRL_YAACRL_H

#include "database.h"


class Yaacrl {
    Database * db;
public:
    Yaacrl();
    int add_file(char *);
    int add_fingerprints(char *, char *) {};
    int recognize_file(char *);
    int recognize_fingerprints(char *, char *) {};
    ~Yaacrl();
};


#endif //YAACRL_YAACRL_H