#include "mysql.h"
#include "database.h"
#include <iostream>


Database::Database() {
    connection = mysql_init(NULL);
    mysql_real_connect(connection,
    "localhost", "root", "tassadar", "yaacrl", 3308, NULL, 0);
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
    /*
     *
     CREATE_FINGERPRINTS_TABLE = """
        CREATE TABLE IF NOT EXISTS `%s` (
             `%s` binary(10) not null,
             `%s` mediumint unsigned not null,
             `%s` int unsigned not null,
         INDEX (%s),
         UNIQUE KEY `unique_constraint` (%s, %s, %s),
         FOREIGN KEY (%s) REFERENCES %s(%s) ON DELETE CASCADE
    ) ENGINE=INNODB;""" % (
        FINGERPRINTS_TABLENAME, Database.FIELD_HASH,
        Database.FIELD_SONG_ID, Database.FIELD_OFFSET, Database.FIELD_HASH,
        Database.FIELD_SONG_ID, Database.FIELD_OFFSET, Database.FIELD_HASH,
        Database.FIELD_SONG_ID, SONGS_TABLENAME, Database.FIELD_SONG_ID
    )
    --------
     Create songs table
     CREATE TABLE IF NOT EXISTS `songs` (
            `song_id` mediumint unsigned not null auto_increment,
            `song_name` varchar(250) not null,
            `fingerprinted` tinyint default 0,
            `file_sha1` binary(20) not null,
        PRIMARY KEY (`song_id`),
        UNIQUE KEY `song_id` (`song_id`)
    ) ENGINE=INNODB;
     --------
     insert fingerprint

     INSERT IGNORE INTO fingerprints (hash, song_id, offset) values
            (UNHEX(%s), %s, %s);

     -------------
     insert song
     INSERT INTO songs (song_name, file_sha1) values (%s, UNHEX(%s));

     ---------------
     select

     SELECT song_id, offset FROM fingerprints WHERE hash = UNHEX(%s);


     ------------
     select nultiple

      SELECT HEX(hash), song_id, offset FROM fingerprints WHERE hash IN (%s);

      --------
      select all
      SELECT song_id, offset FROM fingerprints;

     -----------
     select song

        SELECT song_name, HEX(file_sha1) as file_sha1 FROM songs WHERE song_id = %s;


    ------------
     select num fingerprints
         SELECT COUNT(*) as n FROM fingerprints

     -------
     select unique song ids

      SELECT COUNT(DISTINCT song_id) as n FROM songs WHERE fingerprinted = 1;

     ----------
     select songs

       SELECT song_id, song_name, HEX(file_sha1) as file_sha1 FROM songs WHERE fingerprinted = 1;

     --------
     drop songs

     'DROP TABLE IF EXISTS songs;'

     drop fingerprints

    'DROP TABLE IF EXISTS fingerprints;'

     --------------
     update song fingerprinted

       UPDATE songs SET fingerprinted = 1 WHERE song_id = %s

       ------------
       delete unfingerprinted
         DELETE FROM songs WHERE fingerprinted = 0;









     */

}

void Database::get_songs() {
    mysql_query(connection, "SELECT song_id, song_name, HEX(file_sha1) as file_sha1 FROM songs WHERE fingerprinted = 1;");
    MYSQL_ROW row;
    MYSQL_RES * res = mysql_store_result(connection);
    int num_fields = mysql_num_fields(res);
    std::cout << "There is " << num_fields << " song now!\n";
    // Fetch all rows from the result
    while ((row = mysql_fetch_row(res)))
    {
        // Print all columns
        for(int i = 0; i < num_fields; i++)
        {
            // Make sure row[i] is valid!
            if(row[i] != NULL)
                std::cout << row[i] << std::endl;
            else
                std::cout << "NULL" << std::endl;

            // Also, you can use ternary operator here instead of if-else
            // cout << row[i] ? row[i] : "NULL" << endl;
        }
    }
    if(res != NULL)
        mysql_free_result(res);
}


Database::~Database() {
    mysql_close(connection);
    std::cout << "Closed database\n";
}
