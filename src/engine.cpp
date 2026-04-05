#include "engine.h"
#include <iostream>

static bool compareVariants(const Value& a, const Value& b, bool desc){
    if(std::holds_alternative<int>(a) && std::holds_alternative<int>(b)){
        return desc ? std::get<int>(a) > std::get<int>(b)
                    : std::get<int>(a) < std::get<int>(b);
    }
    if(std::holds_alternative<std::string>(a) && std::holds_alternative<std::string>(b)){
        return desc ? std::get<std::string>(a) > std::get<std::string>(b)
                    : std::get<std::string>(a) < std::get<std::string>(b);
    }
    throw std::runtime_error("Cannot compare values of different types in ORDER BY");
}

Result execute(const ParsedQuery& query){
    if(query.action == "create"){
        return Result{query.tableName, query.action, create(query)};
    }
    if(query.action == "insert"){
        return Result{query.tableName, query.action, insert(query)};
    }
    if(query.action == "select"){
        std::vector<Column> columns;
        if (query.columns.has_value()) {
            columns = *query.columns;
        } else {
            Table* table = Database::getInstance().getTable(query.tableName);
            if (table != nullptr)
                columns = table->columns;
        }
        return Result{query.tableName, query.action, select(query), columns};
    }
    if(query.action == "delete"){
        return Result{query.tableName, query.action, deleteFrom(query)};
    }
    if(query.action == "drop"){
        return Result{query.tableName, query.action, deleteTable(query)};
    }
    if(query.action == "update"){
        return Result{query.tableName, query.action, update(query)};
    }
    return Result{};
}

bool create(const ParsedQuery& query){
    if (!query.columns.has_value())
        throw std::runtime_error("CREATE TABLE missing column definitions");
    return Database::getInstance().addTable({query.tableName, query.columns.value(), {}});
}

bool insert(const ParsedQuery& query){
    if(!query.values.has_value())
        throw std::runtime_error("INSERT INTO missing values definitions");
    
    return Database::getInstance().addRow(query.tableName, Row{*query.values});
}

bool update(const ParsedQuery& query){
    if(
        !query.column.has_value() ||
        !query.value.has_value() ||
        !query.whereValue.has_value() ||
        !query.whereColumn.has_value() 
    )
        throw std::runtime_error("UPDATE missing value or column");

    

    return Database::getInstance().update(
        query.tableName,
        *query.column,
        *query.value,
        *query.whereColumn,
        *query.whereValue
    );
}

std::vector<Row> select(const ParsedQuery& query){
    std::vector<Row> rows;

    if(!query.whereValue.has_value()){
        rows = Database::getInstance().select(query.tableName);
    } else {
        if(!query.whereColumn.has_value()) return {};
        rows = Database::getInstance().select(query.tableName, *query.whereColumn, *query.whereValue);
    }

    if(query.column.has_value()){
        const std::vector<Column>* cols = Database::getInstance().getColumns(query.tableName);
        if(cols){
            int idx = -1;
            for (size_t i = 0; i < cols->size(); i++){
                if((*cols)[i].name == *query.column) { idx = i; break; } 
            }
            if(idx != -1){
                std::sort(rows.begin(), rows.end(), [&](Row& a, Row&b){
                    return compareVariants(a.values[idx], b.values[idx], query.orderByDesc);
                });
            }
        }
    }
    
    // SELECT cols FROM
    if(query.columns.has_value()){
        const std::vector<Column>* cols = Database::getInstance().getColumns(query.tableName);
        if(!cols) return rows;

        std::vector<int> colIndices;
        for(const Column& col: * query.columns){
            for(size_t i = 0; i < cols->size(); i++){
                if((*cols)[i].name == col.name) { colIndices.push_back(i); break; }
            }
        }

        for(Row& row : rows){
            std::vector<Value> trimmed;
            for(int idx : colIndices){
                trimmed.push_back(row.values[idx]);
            }
            row.values = trimmed;
        }
    }
    return rows;
}

bool deleteFrom(const ParsedQuery& query){
    if(!query.whereValue.has_value()){
        throw std::runtime_error("DELETE FROM missing value definitions");
    }
    if(!query.whereColumn.has_value()){
        throw std::runtime_error("DELETE FROM missing column name definitions");
    }
    return Database::getInstance().deleteFrom(query.tableName, *query.whereColumn, *query.whereValue);
}

bool deleteTable(const ParsedQuery& query){
    return Database::getInstance().deleteTable(query.tableName);
}