#include <table.h>

void Database::addTable(const Table& table){
    tables[table.name] = table;
}

Table* Database::getTable(const std::string& name){
    Tables::iterator it = tables.find(name);
    if (it == tables.end()) return nullptr;

    return &it->second;
}

bool Database::addRow(const std::string& tableName, const Row& row){
    Table* table = getTable(tableName);
    if (!table) return false;

    if(row.values.size() != table->columns.size()) return false;

    table->rows.push_back(row);
    return true;
}

bool Database::deleteTable(const std::string& name){
    return tables.erase(name) > 0;
}

bool Database::deleteTable(const Table& table){
    return tables.erase(table.name) > 0;
}

bool Database::deleteRow(const std::string& tableName, const Row& row){
    Table* table = getTable(tableName);
    if(!table) return false;
    return deleteRow(*table, row);
}

bool Database::deleteRow(Table& table, const Row& row) {
    std::vector<Row>::iterator it = std::find_if(table.rows.begin(), table.rows.end(),
        [&](const Row& r) { return r.values == row.values; });

    if (it != table.rows.end()) {
        table.rows.erase(it);
        return true;
    }

    return false;
}