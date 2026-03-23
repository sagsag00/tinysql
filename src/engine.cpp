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
            Table* table = Database::getInstance()->getTable(query.tableName);
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
    std::vector<Row> rows;

    if(!query.value.has_value()){
        if(!query.columns.has_value()){
            rows = Database::getInstance()->select(query.tableName);
        }
        else
            rows = Database::getInstance()->select(query.tableName, *query.columns);
    } else {
        if(!query.columnName.has_value()) return {};
        rows = Database::getInstance()->select(query.tableName, *query.columnName, *query.value);
    }

    if(!query.orderByColumn.has_value()) return rows;
    
    const std::vector<Column>* cols = Database::getInstance()->getColumns(query.tableName);
    if(!cols) return rows;

    int idx = -1;
    for(size_t i = 0; i < cols->size(); i++){
        if((*cols)[i].name == *query.orderByColumn) { idx = i; break; }
    }
    if(idx != -1){
        std::sort(rows.begin(), rows.end(), [&](const Row& a, const Row& b){
            return compareVariants(a.values[idx], b.values[idx], query.orderByDesc);
        });
    }

    return rows;
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