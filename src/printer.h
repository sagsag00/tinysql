#ifndef PRINTER_H
#define PRINTER_H

#include <vector>
#include <string>
#include <iostream>
#include <iomanip>
#include "table.h"

// Formats and prints the results for a boolean (success or not)
// @param success Was the execution successful
// @param tableName The name of the table that was executed on
// @param action The action taken (ie DROP, CREATE, ...)
void formatResult(const bool& success, const std::string& tableName, const std::string& action);

// Formats and prints a table of the result 
// @param rows The table of data to print
// @param tableName The name of the table that was executed on
// @param action The action taken (ie SELECT, ORDER BY, ...)
void formatResult(const std::vector<Row>& rows, const std::vector<Column>& columns);

// Prints a table of data
// @param columns The columns of the table
// @param rows The rows of the table
void printTable(const std::vector<Column>& columns, const std::vector<Row>& rows);

// Prints a table of data
// @overload
// @param table The table
void printTable(const Table& table);

// Prints a table of data
// @overload
// @param tableName The name of the table
void printTable(const std::string& tableName);

#endif