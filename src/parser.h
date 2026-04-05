#ifndef PARSER_H
#define PARSER_H

#include <vector>
#include <string>
#include <unordered_map>
#include <optional>
#include <variant>
#include <iostream>
#include <stdexcept>
#include "tokenizer.h"
#include "table.h"

struct ParsedQuery {
    std::string action;
    std::string tableName;
    std::optional<std::string> whereColumn;
    std::optional<std::string> whereOperator;
    std::optional<Value> whereValue;
    std::optional<std::vector<Value>> whereInValues;
    std::optional<std::pair<Value, Value>> whereBetween;
    std::optional<std::string> whereLike;
    std::optional<std::vector<Value>> values;
    std::optional<std::vector<Column>> columns;
    std::optional<std::string> column; 
    std::optional<Value> value;
    bool orderByDesc = false;
    bool whereNot = false;
};

// Parses the token list into a ParsedQuery given the SQL action verb (e.g. "create", "insert", "select")
// Throws std::runtime_error on malformed input
// @param tokens The token list
// @param action The SQL action verb
// @return The parsed query
ParsedQuery parse(const std::vector<Token>& tokens, const std::string& action);

#endif