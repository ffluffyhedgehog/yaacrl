#include "mysql.h"
#include <stdlib.h>
#include "database.h"
#include "fingerprint.h"
#include <iostream>
#include <string.h>
#include <string>


Database::Database() {
    connection = mysql_init(NULL);
    mysql_real_connect(connection, "localhost", "root", "tassadar", "yaacrl", 3308, NULL, 0);
    if (strcmp(mysql_error(connection), "") != 0) {
        std::cout << "MySQL improperly configured. Please create user 'yaacrl' with no password;";
        exit(1);
    }
}

Database::~Database() {
    mysql_close(connection);
}

int Database::setup() {
    int res;
    res = mysql_query(connection,
    "CREATE TABLE IF NOT EXISTS songs ("
    "song_id mediumint unsigned not null auto_increment,"
    "song_name varchar(250) not null,"
    "fingerprinted tinyint default 0,"
    "file_sha1 binary(20) not null,"
            "PRIMARY KEY (song_id),"
    "UNIQUE KEY song_id (song_id)"
    ") ENGINE=INNODB;");
    res |= mysql_query(connection,
    "CREATE TABLE IF NOT EXISTS fingerprints ("
    "hash binary(10) not null,"
    "song_id mediumint unsigned not null,"
    "offset int unsigned not null,"
            "INDEX (hash),"
    "UNIQUE KEY `unique_constraint` (song_id, offset, hash),"
    "FOREIGN KEY (song_id) REFERENCES songs(song_id) ON DELETE CASCADE"
    ") ENGINE=INNODB;");
    if (res)
    {
        std::cout << "MySQL error on setup():";
        std::cout << mysql_error(connection);
        return -1;
    }
    return 0;
}

int Database::drop_tables() {
    mysql_query(connection, "DROP TABLE IF EXISTS songs");
    mysql_query(connection, "DROP TABLE IF EXISTS fingerprints");
    if (strcmp(mysql_error(connection), "") != 0) {
        std::cout << "MySQL error on drop_tables(): ";
        std::cout << "mysql_error(connection)";
        return -1;
    }
    return 0;
}

int Database::insert_song(char *song_name, char *hash) {
    std::string query = "INSERT INTO songs (song_name, file_sha1) values ";
    query += "(\"" + std::string(song_name) + "\", UNHEX(\"" + std::string(hash) + "\"));";
    mysql_query(connection, query.c_str());
    int status = mysql_query(connection, "select last_insert_id() as id;");
    MYSQL_RES * res = mysql_store_result(connection);
    MYSQL_ROW row;
    if (status) {
        std::cout << mysql_error(connection) << std::endl;
        if(res != NULL)
            mysql_free_result(res);
        return -1;
    }
    else {
        int d = atoi(mysql_fetch_row(res)[0]);
        if(res != NULL)
            mysql_free_result(res);
        return d;
    }
}

int Database::insert_hashes(int sid, PeakHashCollection * hashes) {
    std::string RES = "INSERT IGNORE INTO fingerprints (hash, song_id, offset) values ";
    for (int i = 0; i < hashes->count; i++) {
        RES += "(UNHEX(\"" + std::string(hashes->peak_hashes[i].hash) + "\"), " +std::to_string(sid) + ", " +std::to_string(hashes->peak_hashes[i].time) + ")";
        if (i + 1 == hashes->count)
            RES += ";";
        else
            RES += ", ";
    }
    int status = mysql_query(connection, RES.c_str());
    if (status) {
        std::cout << "MySQL error on insert_hashes():" <<mysql_error(connection);
        return -1;
    }
    return 0;
}

int Database::set_song_fingerprinted(int sid) {
    std::string query = "UPDATE songs SET fingerprinted = 1 WHERE song_id = " + std::to_string(sid) + ";";
    int status = mysql_query(connection, query.c_str());
    if (status) {
        std::cout << "MySQL error on insert_hashes():" <<mysql_error(connection);
        return -1;
    }

    return 0;
}


int Database::return_matches(PeakHashCollection * to_recognize, PeakHashCollection * matches, int ** matched_ids) {
    std::string query = "SELECT HEX(hash), song_id, offset FROM fingerprints WHERE hash IN (";
    for (int i = 0; i < to_recognize->count; i++) {
        query += "UNHEX(\"" + std::string(to_recognize->peak_hashes[i].hash) + "\")";
        if (i + 1 == to_recognize->count)
            query += " );";
        else
            query += ", ";
    }
    int status = mysql_query(connection, query.c_str());
    if (status) {
        std::cout << "MySQL error in return_matches();" <<mysql_error(connection);
        return -1;
    }

    int START_MATCH_SIZE = 200;
    int real_count = 0;
    matches->count = START_MATCH_SIZE;
    matches->peak_hashes = (PeakHash *) malloc(sizeof(PeakHash) * START_MATCH_SIZE);
    (*matched_ids) = (int *) malloc(sizeof(int) * START_MATCH_SIZE);


    MYSQL_RES * res = mysql_store_result(connection);
    int num_fields = mysql_num_fields(res);
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res)))
    {
        for(int i = 0; i < num_fields; i++) {
            switch (i%3) {
                case 0:
                    real_count++;
                    if (real_count + 10 > matches->count) {
                        matches->count += 200;
                        matches->peak_hashes = (PeakHash *) realloc(matches->peak_hashes,
                                                                    sizeof(PeakHash) * matches->count);
                        (*matched_ids) = (int *) realloc((*matched_ids), sizeof(int) * matches->count);
                    }
                    strncpy(matches->peak_hashes[real_count - 1].hash, row[i], 20);
                    matches->peak_hashes[real_count - 1].hash[20] = '\0';
                    break;
                case 1:
                    (*matched_ids)[real_count - 1] = atoi(row[i]);
                    break;
                case 2:
                    matches->peak_hashes[real_count - 1].time = atoi(row[i]);
                    break;
            }
        }
    }
    if(res != NULL)
        mysql_free_result(res);

    matches->count = real_count;
    matches->peak_hashes = (PeakHash *) realloc(matches->peak_hashes, sizeof(PeakHash) * matches->count);
    (*matched_ids) = (int *) realloc((*matched_ids), sizeof(int) * matches->count);
    return 0;
}


std::string Database::get_song_by_id(int sid) {
    std::string query = "SELECT song_name FROM songs WHERE song_id = ";
    query += std::to_string(sid) + ";";
    int status = mysql_query(connection, query.c_str());
    if (status) {
        std::cout << mysql_error(connection) << std::endl;
        return "";
    } else {
        MYSQL_RES * res = mysql_store_result(connection);
        if (res == NULL)
            return "";
        MYSQL_ROW row;
        int num_fields = mysql_num_fields(res);
        std::string s = mysql_fetch_row(res)[0] ;
        if(res != NULL)
            mysql_free_result(res);
        return s;
    }
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




    ******************************
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
