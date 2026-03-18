#ifndef TABLE.H
#define TABLE.H

#include <string>
#include <variant>
#include <vector>
#include <unordered_map>
#include <algorithm>

struct Table;

using Value = std::variant<int, std::string>;
using Tables = std::unordered_map<std::string, Table>;

struct Column{
    std::string name;
    enum Type {INTEGER, TEXT} type;
};

struct Row{
    std::vector<Value> values;
};

struct Table{
    std::string name;
    std::vector<Column> columns;
    std::vector<Row> rows;
};

class Database{
private:
    Tables tables;

public:
    // Adds a table to the database
    // @param table The table object
    void addTable(const Table& table);

    // Returns a table by name
    // @param name The table name
    Table* getTable(const std::string& name);

    // Adds a row to the table
    // @param tableName The name of the table
    // @param row The row object
    // @return true if the row was added successfully false otherwise
    bool addRow(const std::string& tableName, const Row& row);

    // Deletes a table by name
    // @param name The table name
    // @return true if the table was deleted successfully false otherwise
    bool deleteTable(const std::string& name);
    bool deleteTable(const Table& table);

    bool deleteRow(const std::string& tableName, const Row& row);
    bool deleteRow(Table& table, const Row& row);
};

#endif