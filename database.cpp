#include "mysql.h"
#include "database.h"
#include <iostream>


Database::Database() {
    connection = mysql_init(NULL);
    mysql_real_connect(connection,
    "localhost", "root", "sportbuhlo", "yaacrl", 3308, NULL, 0);
    int ha = mysql_query(connection,
    "CREATE TABLE IF NOT EXISTS songs ("
    "song_id mediumint unsigned not null auto_increment,"
    "song_name varchar(250) not null,"
    "fingerprinted tinyint default 0,"
    "file_sha1 binary(20) not null,"
            "PRIMARY KEY (song_id),"
    "UNIQUE KEY song_id (song_id)"
    ") ENGINE=INNODB;");
    ha |= mysql_query(connection,
    "CREATE TABLE IF NOT EXISTS fingerprints ("
    "hash binary(10) not null,"
    "song_id mediumint unsigned not null,"
    "offset int unsigned not null,"
            "INDEX (hash),"
    "UNIQUE KEY `unique_constraint` (song_id, offset, hash),"
    "FOREIGN KEY (song_id) REFERENCES songs(song_id) ON DELETE CASCADE"
    ") ENGINE=INNODB;");
    if (ha)
    {
        std::cout << "Something is wrong:\n\n";
        std::cout << mysql_error(connection);
    }

}
Database::~Database() {
    mysql_close(connection);
    std::cout << "Closed database";
}
