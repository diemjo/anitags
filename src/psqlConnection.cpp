#include <stdio.h>
#include <error_codes.h>
#include <vector>

#include "../inc/psqlConnection.h"

using namespace std;

pqxx::connection* db_con;

pqxx::connection* db_connect(string user, string address, int port) {
    try {
        char s[128];
        snprintf(s, 128, "dbname = anitags user = %s hostaddr = %s port = %i", user.c_str(), address.c_str(), port);
        db_con = new pqxx::connection(s);
        if (db_con->is_open()) {
            return db_con;
        }
        else {
            printf("Error: cannot connect to database anitags as %s@%s:%i\n", user.c_str(), address.c_str(), port);
            exit(ERROR_FILE_DOES_NOT_EXIST);
        }
    }
    catch (const std::exception &e) {
        printf("%s\n", e.what());
        return 0;
    }
}

bool db_add_tags(config *conf) {
    string insert_tag_string = "INSERT INTO tags(tag)\n"
                               "VALUES ($1)\n"
                               "ON CONFLICT (tag)\n"
                               "DO NOTHING\n";
    string insert_string = "INSERT INTO pictures(filename, tag)\n"
                           "VALUES ($1, $2)\n"
                           "ON CONFLICT (filename, tag)\n"
                           "DO NOTHING";
    db_con->prepare("insert_tag", insert_tag_string);
    db_con->prepare("insert", insert_string);
    try {
        for (string tag : conf->tags_to_add) {
            pqxx::work w(*db_con);
            pqxx::result r = w.exec_prepared("insert_tag", tag.c_str());
            w.commit();
        }
        for (string file : conf->filenames) {
            for (string tag : conf->tags_to_add) {
                pqxx::work w(*db_con);
                pqxx::result r = w.exec_prepared("insert", file, tag.c_str());
                w.commit();
            }
        }
    }
    catch (const std::exception &e) {
        printf("PSQLConnection::%s\n", e.what());
        return false;
    }
    return true;
}

bool db_remove_tags(config *conf) {
    string remove_string = "DELETE FROM pictures\n"
                           "WHERE filename=$1\n"
                           "AND tag=$2";
    db_con->prepare("remove", remove_string);
    try {
        for (string file : conf->filenames) {
            for (string tag : conf->tags_to_remove) {
                pqxx::work w(*db_con);
                pqxx::result r = w.exec_prepared("remove", file, tag.c_str());
                w.commit();
            }
        }
    }
    catch (const std::exception &e) {
        printf("PSQLConnection::%s\n", e.what());
        return false;
    }
    string remove_tag_string = "DELETE FROM tags\n"
                               "WHERE NOT EXISTS(\n"
                               "SELECT tag\n"
                               "FROM pictures\n"
                               "WHERE tags.tag=pictures.tag)";
    db_con->prepare("remove_tag", remove_tag_string);
    try {
        pqxx::work w(*db_con);
        pqxx::result r = w.exec_prepared("remove_tag");
        w.commit();
    }
    catch (const std::exception &e) {
        printf("PSQLConnection::%s\n", e.what());
        return false;
    }
    return true;
}

bool db_print_tags(config *conf) {
    for (string filename : conf->filenames) {
        string searchstring = "SELECT tag\n"
                              "FROM pictures\n"
                              "WHERE filename=$1";

        pqxx::work w(*db_con);

        db_con->prepare("print", searchstring);
        pqxx::result r = w.exec_prepared("print", filename);
        w.commit();

        if (conf->filenames.size()!=1)
            printf("%s:\n", filename.c_str());
        bool has_tag = false;
        for (pqxx::result::const_iterator c = r.begin(); c != r.end(); c++) {
            if (c == r.begin())
                printf("%s", c[0].as<string>().c_str());
            else
                printf(", %s", c[0].as<string>().c_str());
            has_tag=true;
        }
        if (has_tag)
            printf("\n");
    }
    return true;
}

vector<string> db_search_for_tags(config *conf) {
    vector<string> filenames;

    string searchstring = "SELECT filename, count(tag)\n"
                          "FROM pictures";
    if (conf->tags_to_search.size()>0)
        searchstring.append(" WHERE tag = $1\n");
    for (int i=1; i<conf->tags_to_search.size(); i++) {
        searchstring.append(" OR tag = $"+std::to_string(i+1));
    }
    searchstring.append("\nGROUP BY filename");
    searchstring.append("\nHAVING count(tag)=");
    searchstring.append(std::to_string(conf->tags_to_search.size()));

    pqxx::work w(*db_con);

    db_con->prepare("search", searchstring);
    pqxx::prepare::invocation w_invocation = w.prepared("search");
    prep_dynamic(conf->tags_to_search, w_invocation);
    pqxx::result r = w_invocation.exec();
    w.commit();

    for (pqxx::result::const_iterator c = r.begin(); c != r.end(); c++) {
        filenames.push_back(c[0].as<string>());
    }
    return filenames;
}

pqxx::prepare::invocation& prep_dynamic(vector<string> data, pqxx::prepare::invocation& inv) {
    for(auto data_val : data)
        inv(data_val);
    return inv;
}