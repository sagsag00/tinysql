#include "table.h"

Database* Database::getInstance(){
    return instance;
}

bool Database::addTable(const Table& table){
    tables[table.name] = table;
    return true;
}

Table* Database::getTable(const std::string& name){
    Tables::iterator it = tables.find(name);
    if (it == tables.end()) return nullptr;

    return &it->second;
}


const std::vector<Column>* Database::getColumns(const std::string& tableName){
    const Table* table = Database::getTable(tableName);
    if(table == nullptr) return nullptr;
    return &table->columns;
}

bool Database::addRow(const std::string& tableName, const Row& row){
    Table* table = getTable(tableName);
    if (!table) return false;

    if(row.values.size() != table->columns.size()) return false;

    table->rows.push_back(row);
    return true;
}

std::vector<Row> Database::select(const std::string& tableName){
    Table* table = getTable(tableName);
    if (!table) return {};

    std::vector<int> colIndices;
    for(int i = 0; i < table->columns.size(); i++){
        colIndices.push_back(i);
    }

    return iterateRows(table, colIndices);
}

std::vector<Row> Database::select(const std::string& tableName, const std::vector<Column>& columns){
    Table* table = getTable(tableName);
    if (!table) return {};

    std::vector<int> colIndices;
    for (const Column& col : columns){
        bool found = false;
        for(int i = 0; i < table->columns.size(); i++){
            if(table->columns[i].name == col.name){
                colIndices.push_back(i);
                found = true;
                break;
            }
        }
        if (!found){
            throw std::runtime_error("Column " + col.name + " does not exist in table " + tableName);
        }
    }

    return iterateRows(table, colIndices);
}

std::vector<Row> Database::select(const std::string& tableName, const std::string& columnName, const Value& value){
    Table* table = getTable(tableName);
    if(!table) return {};

    int index = getColumnIndex(table, columnName);

    std::vector<Row> result;
    for (const Row& row : table->rows){
        if(compareValue(row.values[index], value)){
            result.push_back(row);
        } 
    }

    return result;
}

bool Database::deleteFrom(const std::string& tableName, const std::string& columnName, const Value& value){
    Table* table = getTable(tableName);
    if(!table) return false;

    int index = getColumnIndex(table, columnName);

    std::vector<Row> rows = table->rows;
    size_t originalSize = rows.size();

    rows.erase(
        std::remove_if(rows.begin(), rows.end(),
            [&](const Row& r){ return compareValue(r.values[index], value); }),
        rows.end()
    );

    return rows.size() < originalSize;
}

bool Database::deleteTable(const std::string& name){
    return tables.erase(name) > 0;
}

bool Database::deleteTable(const Table& table){
    return tables.erase(table.name) > 0; 
}

bool Database::compareValue(const Value& a, const Value& b){
    if(a.index() != b.index()) return false;
    return a == b;
}

std::vector<Row> Database::iterateRows(const Table* table, const std::vector<int>& colIndices){
    std::vector<Row> result;

    for (const Row& row : table->rows){
        Row newRow;
        for(int idx : colIndices){
            if(idx >= 0 && idx < row.values.size()){
                newRow.values.push_back(row.values[idx]);
            }
        }
        result.push_back(newRow);
    }

    return result;
}

int Database::getColumnIndex(const Table* table, const std::string& columnName){
    int index = -1;
    for(int i = 0; i < table->columns.size(); i++){
        if(table->columns[i].name == columnName){
            index = i;
            break;
        }
    }

    if(index == -1){
        throw std::runtime_error("Column " + columnName + " does not exist");
    }
    return index;
}