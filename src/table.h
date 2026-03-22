#ifndef TABLE_H
#define TABLE_H

#include <string>
#include <variant>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <stdexcept>

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
    static Database* instance;

    Database() = default;
    Database(const Database&) = delete;
    Database& operator=(const Database&) = delete;

    std::vector<Row> iterateRows(const Table* table, const std::vector<int>& colIndices);
    int getColumnIndex(const Table* table, const std::string& columnName);
    bool compareValue(const Value& a, const Value& b);

public:
    static Database* getInstance();

    // Adds a table to the database
    // @param table The table object
    bool addTable(const Table& table);

    // Returns a table by name
    // @param name The table name
    Table* getTable(const std::string& name);

    // Returns all columns in a table
    // @param tableName The name of the table
    const std::vector<Column>* getColumns(const std::string& tableName);
    
    // Adds a row to the table
    // @param tableName The name of the table
    // @param row The row object
    // @return true if the row was added successfully, false otherwise
    bool addRow(const std::string& tableName, const Row& row);

    // Implementation of SELECT * FROM <name>
    // @param tableName The name of the table you want to select from
    // @return All the rows in the table
    std::vector<Row> select(const std::string& tableName);

    // Implementation of SELECT <cols> FROM <name>
    // @param tableName The name of the table you want to select from
    // @param columns The columns you want to select from
    // @return All the values in the provided columns 
    std::vector<Row> select(const std::string& tableName, const std::vector<Column>& columns);

    // Implementation of SELECT ... WHERE <col> = <value>
    // @param tableName The name of the table you want to select from
    // @param columns The column you want to select from
    // @param value The value you want to compare
    // @return All the values in the provided columns 
    std::vector<Row> select(const std::string& tableName, const std::string& columnName, const Value& value);

    // Implementation of DELETE FROM <name> WHERE <col> = <value>
    // @param tableName The name of the table you want to delete from
    // @param columns The column you want to delete from
    // @param value The value you want to compare
    // @return true if any rows deleted, false otherwise
    bool deleteFrom(const std::string& tableName, const std::string& columnName, const Value& value);

    // Deletes a table by name
    // @param name The table name
    // @return true if the table was deleted successfully, false otherwise
    bool deleteTable(const std::string& name);

    // Deletes a table by reference
    //@overload
    //@param table The Table object to delete
    //@return true if the table was deleted successfully, false otherwise
    bool deleteTable(const Table& table);

    // Deletes a row from table.
    // @overload
    // @param table A reference to the table object containing the row
    // @param row The row to delete
    // @return true if the row was deleted successfully, false otherwise
    bool deleteRow(Table& table, const Row& row);

    void clear() { tables.clear(); };
};

#endif