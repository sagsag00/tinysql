#ifndef ENGINE_H
#define ENGINE_H

#include <vector>
#include <string>
#include "parser.h"
#include "table.h"

using Result = std::variant<std::vector<Row>, bool, std::monostate>;

// Creates a new table using the column definitions in the query
// Throws std::runtime_error if column definitions are missing
// @param query The parsed query containing the table name and columns
std::monostate create(ParsedQuery& query);

// Inserts a new row into the table specified in the query
// Throws std::runtime_error if values are missing
// @param query The parsed query containing the table name and values
// @return true if the row was inserted successfully
bool insert(ParsedQuery& query);

// Selects rows from the table specified in the query
// Returns all rows if no WHERE clause is present, otherwise filters by the column and value
// @param query The parsed query containing the table name and optional filter
// @return A vector of rows matching the query
std::vector<Row> select(ParsedQuery& query);

// Deletes rows from the table where the column matches the value in the query
// Throws std::runtime_error if the column name or value are missing
// @param query The parsed query containing the table name, column name and value
// @return true if at least one row was deleted
bool deleteFrom(ParsedQuery& query);

// Drops the table specified in the query
// @param query The parsed query containing the table name
// @return true if the table was dropped successfully
bool deleteTable(ParsedQuery& query);

// Dispatches the parsed query to the appropriate execute function based on the action
// Throws std::runtime_error for unknown actions or malformed queries
// @param query The parsed query to execute
Result execute(ParsedQuery& query);

#endif