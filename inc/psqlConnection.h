#ifndef ANITAGS_PSQLCONNECTION_H
#define ANITAGS_PSQLCONNECTION_H

#include <pqxx/pqxx>
#include <string.h>
#include <vector>
#include "config.h"

pqxx::connection* db_connect(std::string user, std::string address, int port);
vector<string> db_search_for_tags(config *conf);
bool db_add_tags(config *conf);
bool db_remove_tags(config *conf);
bool db_print_tags(config *conf);
pqxx::prepare::invocation& prep_dynamic(vector<string> data, pqxx::prepare::invocation& inv);

#endif //ANITAGS_PSQLCONNECTION_HPP
