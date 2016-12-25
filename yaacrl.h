#ifndef YAACRL_YAACRL_H
#define YAACRL_YAACRL_H

#include "database.h"


class Yaacrl {
    Database * db;
public:
    Yaacrl();
    void init();
    void fingerprint_file();
    ~Yaacrl();


};


#endif //YAACRL_YAACRL_H