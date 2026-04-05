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

static bool matchesWhere(const Row& row, int idx, const ParsedQuery& query){
    const Value& cell = row.values[idx];
    bool result = false;

    // BETWEEN
    if(query.whereBetween.has_value()){
        auto [low, high] = *query.whereBetween;
        result = !compareVariants(cell, low, false) &&
                !compareVariants(high, cell, false);
    }

    // IN
    else if(query.whereInValues.has_value()){
        for(const Value& v: *query.whereInValues)
            if(cell == v) { result = true; break; }
    }

    // LIKE
    else if(query.whereLike.has_value() && std::holds_alternative<std::string>(cell)){
        const std::string& s = std::get<std::string>(cell);
        const std::string& pattern = *query.whereLike;
        if(pattern.front() == '%' && pattern.back() == '%')
            result = s.find(pattern.substr(1, pattern.size()-2)) != std::string::npos;
        else if(pattern.front() == '%')
            result = s.size() >= pattern.size()-1 && s.substr(s.size()-(pattern.size()-1)) == pattern.substr(1);
        else if(pattern.back() == '%')
            result = s.substr(0, pattern.size()-1) == pattern.substr(0, pattern.size()-1);
        else 
            result =  s == pattern;
    } 
    
    // Comparison operators
    else {
        if(!query.whereValue.has_value()) return false;
        const Value& target = *query.whereValue;
        std::string op = query.whereOperator.value_or("=");

        if(op == "=") result = cell == target;
        else if(op == "!=" || op == "<>") result = cell != target;
        else if(op == ">")  result = compareVariants(target, cell, false); // cell > target
        else if(op == "<")  result = compareVariants(cell, target, false); // cell < target
        else if(op == ">=") result = !compareVariants(cell, target, false);
        else if(op == "<=") result = !compareVariants(target, cell, false);
    }

    return query.whereNot ? !result : result;
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

static void applyWhere(std::vector<Row>& rows, const ParsedQuery& query, const std::vector<Column>& cols){
    if(!query.whereColumn.has_value()) return;

    int idx = -1;
    for(size_t i = 0; i < cols.size(); i++)
        if(cols[i].name == *query.whereColumn) { idx = i; break; }
    if(idx == -1) return;

    std::vector<Row> filtered;
    for(const Row& row : rows)
        if(matchesWhere(row, idx, query))
            filtered.push_back(row);
    rows = filtered;
}

static void applyOrderBy(std::vector<Row>& rows, const ParsedQuery& query, const std::vector<Column>& cols){
    if(!query.column.has_value()) return;

    int idx = -1;
    for(size_t i = 0; i < cols.size(); i++)
        if(cols[i].name == *query.column) { idx = i; break; }
    if(idx == -1) return;

    std::sort(rows.begin(), rows.end(), [&](const Row& a, const Row& b){
        return compareVariants(a.values[idx], b.values[idx], query.orderByDesc);
    });
}

static void applyColumnTrim(std::vector<Row>& rows, const ParsedQuery& query, const std::vector<Column>& cols){
    if(!query.columns.has_value()) return;

    std::vector<int> colIndices;
    for(const Column& col : *query.columns)
        for(size_t i = 0; i < cols.size(); i++)
            if(cols[i].name == col.name) { colIndices.push_back(i); break; }

    for(Row& row : rows){
        std::vector<Value> trimmed;
        for(int idx : colIndices)
            trimmed.push_back(row.values[idx]);
        row.values = trimmed;
    }
}

std::vector<Row> select(const ParsedQuery& query){
    std::vector<Row> rows = Database::getInstance().select(query.tableName);

    const std::vector<Column>* cols = Database::getInstance().getColumns(query.tableName);
    if(!cols) return rows;

    applyWhere(rows, query, *cols);
    applyOrderBy(rows, query, *cols);
    applyColumnTrim(rows, query, *cols);

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