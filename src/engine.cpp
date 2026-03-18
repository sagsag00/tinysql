#include "engine.h"

void execute(ParsedQuery& query){
    if(query.action == "create"){
        create(query);
    }
    if(query.action == "insert"){
        insert(query);
    }
    if(query.action == "select"){
        select(query);
    }
    if(query.action == "delete"){
        deleteFrom(query);
    }
    if(query.action == "drop"){
        deleteFrom(query);
    }
}
void create(ParsedQuery& query){
    if (!query.columns.has_value())
        throw std::runtime_error("CREATE TABLE missing column definitions");
    Database::getInstance()->addTable({query.tableName, query.columns.value(), {}});
}

bool insert(ParsedQuery& query){
    if(!query.values.has_value())
        throw std::runtime_error("INSERT INTO missing values definitions");
    
    return Database::getInstance()->addRow(query.tableName, Row{*query.values});
}

std::vector<Row> select(ParsedQuery& query){
    if(!query.value.has_value()){
        if(!query.columns.has_value()){
            // SELECT *
            Database::getInstance()->select(query.tableName);
            return;
        }
        //TODO SELECT COLS
    }
    //TODO SELECT ... WHERE
    return;
}

bool deleteFrom(ParsedQuery& query){
    if(!query.value.has_value()){
        throw std::runtime_error("DELETE FROM missing value definitions");
    }
    if(!query.columnName.has_value()){
        throw std::runtime_error("DELETE FROM missing column name definitions");
    }
    return Database::getInstance()->deleteFrom(query.tableName, *query.columnName, *query.value);
}

bool deleteTable(ParsedQuery& query){
    return Database::getInstance()->deleteTable(query.tableName);
}