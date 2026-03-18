#ifndef ENGINE_H
#define ENGINE_H

#include <vector>
#include <string>
#include "parser.h"
#include "table.h"

void create(ParsedQuery& query);
bool insert(ParsedQuery& query);
std::vector<Row> select(ParsedQuery& query);
bool deleteFrom(ParsedQuery& query);
bool deleteTable(ParsedQuery& query);

void execute(ParsedQuery& query);

#endif