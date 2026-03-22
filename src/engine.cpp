#include "engine.h"
#include <iostream>

Result execute(const ParsedQuery& query){
    if(query.action == "create"){
        return Result{query.tableName, query.action, create(query)};
    }
    if(query.action == "insert"){
        return Result{query.tableName, query.action, insert(query)};
    }
    if(query.action == "select"){
        return Result{query.tableName, query.action, select(query)};
    }
    if(query.action == "delete"){
        return Result{query.tableName, query.action, deleteFrom(query)};
    }
    if(query.action == "drop"){
        return Result{query.tableName, query.action, deleteTable(query)};
    }
    return Result{};
}

bool create(const ParsedQuery& query){
    if (!query.columns.has_value())
        throw std::runtime_error("CREATE TABLE missing column definitions");
    return Database::getInstance()->addTable({query.tableName, query.columns.value(), {}});
}

bool insert(const ParsedQuery& query){
    if(!query.values.has_value())
        throw std::runtime_error("INSERT INTO missing values definitions");
    
    return Database::getInstance()->addRow(query.tableName, Row{*query.values});
}

std::vector<Row> select(const ParsedQuery& query){
    if(!query.value.has_value()){
        if(!query.columns.has_value()){
            // SELECT * FROM
            return Database::getInstance()->select(query.tableName);
        }
        // SELECT COLS FROM NAME
        return Database::getInstance()->select(query.tableName, *query.columns);
    }
    if(!query.columnName.has_value()) return {};
    return Database::getInstance()->select(query.tableName, *query.columnName, *query.value);
}

bool deleteFrom(const ParsedQuery& query){
    if(!query.value.has_value()){
        throw std::runtime_error("DELETE FROM missing value definitions");
    }
    if(!query.columnName.has_value()){
        throw std::runtime_error("DELETE FROM missing column name definitions");
    }
    return Database::getInstance()->deleteFrom(query.tableName, *query.columnName, *query.value);
}

bool deleteTable(const ParsedQuery& query){
    return Database::getInstance()->deleteTable(query.tableName);
}