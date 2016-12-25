#include "mysql.h"
#include <stdlib.h>
#include "database.h"
#include "fingerprint.h"
#include <iostream>
#include <string.h>


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
}

Database::~Database() {
    mysql_close(connection);
    std::cout << "Closed database\n";
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

int Database::insert_song(char *song_name, char *hash) {
    const char * query_template = "INSERT INTO songs (song_name, file_sha1) values ('%s', UNHEX('%s'));";
    char query[512];
    int status;
    snprintf(query, 512, query_template, song_name, hash);
    mysql_query(connection, query);
    status = mysql_query(connection, "select last_insert_id() as id;");
    MYSQL_RES * res = mysql_store_result(connection);
    MYSQL_ROW row;
    if (status != 0) {
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

void Database::insert_hashes(int sid, PeakHashCollection * hashes) {
    const char * query_template = "INSERT IGNORE INTO fingerprints (hash, song_id, offset) values (UNHEX(\"%s\"), \"%d\", \"%d\");";
    const char * query_template_10 = "INSERT IGNORE INTO fingerprints (hash, song_id, offset) values "
            "(UNHEX(\"%s\"), \"%d\", \"%d\"), "  "(UNHEX(\"%s\"), \"%d\", \"%d\"), "
            "(UNHEX(\"%s\"), \"%d\", \"%d\"), "  "(UNHEX(\"%s\"), \"%d\", \"%d\"), "
            "(UNHEX(\"%s\"), \"%d\", \"%d\"), "  "(UNHEX(\"%s\"), \"%d\", \"%d\"), "
            "(UNHEX(\"%s\"), \"%d\", \"%d\"), "  "(UNHEX(\"%s\"), \"%d\", \"%d\"), "
            "(UNHEX(\"%s\"), \"%d\", \"%d\"), "  "(UNHEX(\"%s\"), \"%d\", \"%d\")";
    char query[1024];
    int status;
    for (int i = 0; i < hashes->count; i++) {
        printf("%d - currenct index", i);
         if (i+20 < hashes->count)
            snprintf(query, 1024, query_template_10, hashes->peak_hashes[i].hash, sid, hashes->peak_hashes[i].time
                                                    , hashes->peak_hashes[++i].hash, sid, hashes->peak_hashes[i].time
                                                    , hashes->peak_hashes[++i].hash, sid, hashes->peak_hashes[i].time
                                                    , hashes->peak_hashes[++i].hash, sid, hashes->peak_hashes[i].time
                                                    , hashes->peak_hashes[++i].hash, sid, hashes->peak_hashes[i].time
                                                    , hashes->peak_hashes[++i].hash, sid, hashes->peak_hashes[i].time
                                                    , hashes->peak_hashes[++i].hash, sid, hashes->peak_hashes[i].time
                                                    , hashes->peak_hashes[++i].hash, sid, hashes->peak_hashes[i].time
                                                    , hashes->peak_hashes[++i].hash, sid, hashes->peak_hashes[i].time
                                                    , hashes->peak_hashes[++i].hash, sid, hashes->peak_hashes[i].time);
        else
             snprintf(query, 1024, query_template, hashes->peak_hashes[i].hash, sid, hashes->peak_hashes[i].time);
        mysql_query(connection, query);
    }
}

void Database::set_song_fingerprinted(int sid) {
    const char * query_template = "UPDATE songs SET fingerprinted = 1 WHERE song_id = %d";
    char query[512];
    snprintf(query, 512, query_template, sid);
    mysql_query(connection, query);
}


void Database::return_matches(PeakHashCollection * to_recognize, PeakHashCollection * matches, int ** matched_ids) {
    int START_MATCH_SIZE = 200;
    char * query_template = "SELECT HEX(hash), song_id, offset FROM fingerprints WHERE hash IN (%s);";
    char query[2048];
    MYSQL_RES * res;
    MYSQL_ROW row;
    int status;

    matches->count = START_MATCH_SIZE;
    matches->peak_hashes = (PeakHash *) malloc(sizeof(PeakHash) * START_MATCH_SIZE);
    (*matched_ids) = (int *) malloc(sizeof(int) * START_MATCH_SIZE);

    int real_count = 0;
    int num_fields;
    for (int i = 0; i < to_recognize->count; i++) {
        if (i + 21 < to_recognize->count) {
            char templ[1024];
            const char * template_20 = "UNHEX(\"%s\"), UNHEX(\"%s\"), UNHEX(\"%s\"), UNHEX(\"%s\"), UNHEX(\"%s\"), "
                                     "UNHEX(\"%s\"), UNHEX(\"%s\"), UNHEX(\"%s\"), UNHEX(\"%s\"), UNHEX(\"%s\"), "
                                     "UNHEX(\"%s\"), UNHEX(\"%s\"), UNHEX(\"%s\"), UNHEX(\"%s\"), UNHEX(\"%s\"), "
                                     "UNHEX(\"%s\"), UNHEX(\"%s\"), UNHEX(\"%s\"), UNHEX(\"%s\"), UNHEX(\"%s\")";

            snprintf(templ, 1024, template_20, to_recognize->peak_hashes[i].hash, to_recognize->peak_hashes[++i].hash,
                     to_recognize->peak_hashes[++i].hash, to_recognize->peak_hashes[++i].hash,
                     to_recognize->peak_hashes[++i].hash, to_recognize->peak_hashes[++i].hash,
                     to_recognize->peak_hashes[++i].hash, to_recognize->peak_hashes[++i].hash,
                     to_recognize->peak_hashes[++i].hash, to_recognize->peak_hashes[++i].hash,
                     to_recognize->peak_hashes[++i].hash, to_recognize->peak_hashes[++i].hash,
                     to_recognize->peak_hashes[++i].hash, to_recognize->peak_hashes[++i].hash,
                     to_recognize->peak_hashes[++i].hash, to_recognize->peak_hashes[++i].hash,
                     to_recognize->peak_hashes[++i].hash, to_recognize->peak_hashes[++i].hash,
                     to_recognize->peak_hashes[++i].hash, to_recognize->peak_hashes[++i].hash);
            snprintf(query, 2048, query_template, templ);
        }
        else {
            char template_1[100];
            snprintf(template_1, 100, "UNHEX(\"%s\")", to_recognize->peak_hashes[i].hash);
            snprintf(query, 2048, query_template, template_1);
        }
        status = mysql_query(connection, query);
        if (status != 0 ) {
            std::cout << mysql_error(connection);
            return;
        }
        res = mysql_store_result(connection);
        num_fields = mysql_num_fields(res);
        while ((row = mysql_fetch_row(res)))
        {
            for(int i = 0; i < num_fields; i++)
                switch (i%3) {
                    case 0:
                        //printf("Hash is %s ", row[i]);
                        real_count++;
                        if (real_count + 10 > matches->count) {
                            matches->count += 200;
                            matches->peak_hashes = (PeakHash *) realloc(matches->peak_hashes, sizeof(PeakHash) * matches->count);
                            (*matched_ids) = (int *) realloc((*matched_ids), sizeof(int) * matches->count);
                        }
                        strncpy(matches->peak_hashes[real_count-1].hash, row[i], 20);
                        matches->peak_hashes[real_count-1].hash[20] = '\0';
                        break;
                    case 1:
                        //printf("song_id is %d ", atoi(row[i]));
                        (*matched_ids)[real_count-1] = atoi(row[i]);
                        break;
                    case 2:
                        //printf("offset is %s\n", row[i]);
                        matches->peak_hashes[real_count-1].time = atoi(row[i]);
                        break;
                }
        }
        if(res != NULL)
            mysql_free_result(res);

    }
    matches->count = real_count;
    matches->peak_hashes = (PeakHash *) realloc(matches->peak_hashes, sizeof(PeakHash) * matches->count);
    (*matched_ids) = (int *) realloc((*matched_ids), sizeof(int) * matches->count);
}

void Database::get_song_by_id(int sid) {
    const char * query_template = "SELECT song_name, HEX(file_sha1) as file_sha1 FROM songs WHERE song_id = %d;";
    char query[256];
    int status;
    snprintf(query, 256, query_template, sid);
    status = mysql_query(connection, query);
    MYSQL_RES * res = mysql_store_result(connection);
    MYSQL_ROW row;
    if (status != 0) {
        std::cout << mysql_error(connection) << std::endl;
        if(res != NULL)
            mysql_free_result(res);
        return ;
    }
    else {
        int num_fields = mysql_num_fields(res);
        while ((row = mysql_fetch_row(res)))
        {
            // Print all columns
            for(int i = 0; i < num_fields; i++)
            {
                    std::cout << row[i] << std::endl;


            }
        }
        if(res != NULL)
            mysql_free_result(res);
        return;

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
