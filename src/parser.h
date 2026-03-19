#ifndef PARSER_H
#define PARSER_H

#include <vector>
#include <string>
#include <unordered_map>
#include <optional>
#include <variant>
#include "tokenizer.h"
#include "table.h"

struct ParsedQuery {
    std::string action;
    std::string tableName;
    std::optional<std::string> columnName;
    std::optional<Value> value;
    std::optional<std::vector<Value>> values;
    std::optional<std::vector<Column>> columns;
};

// Advances past any symbol tokens, then checks the next token matches the expected type
// Throws std::runtime_error if the token is missing or the type doesn't match
// @param tokens The token vector
// @param i The current index, advanced in place
// @param type The expected token type
// @return The matched token
const Token& expect(const std::vector<Token>& tokens, size_t& i, const Token::Type type);

// Checks if the token is a keyword and it matches the keyword name
// @param t The token to check
// @param kw The keyword to compare
// @return true if the type is a keyword and the keywords match
bool matchKeyword(const Token& t, const std::string& kw);

// Maps a type string ("INT", "INTEGER", "TEXT") to a Column::Type
// Throws std::runtime_error for unrecognised type strings
// @param raw The raw type string
// @return The parsed Column::Type
Column::Type parseColumnType(const std::string& raw);

// Consumes a comma-separated name+type column list from tokens starting at i
// Stops at the first token that is not an identifier or symbol
// @param tokens The token list
// @param i The current index, advanced in place
// @return A vector of all the parsed columns
std::vector<Column> parseColumnList(const std::vector<Token>& tokens, size_t& i);

// Consumes a list of literal values from tokens starting at i
// Stops at the first token that is not a literal or symbol
// @param tokens The token list
// @param i The current index, advanced in place
// @return A vector of the parsed values
std::vector<Value> parseValueList(const std::vector<Token>& tokens, size_t& i);

// Parses a raw string as an int if possible, otherwise strips surrounding quotes and returns it as a string
// @param raw The string to parse
// @return The parsed Value
Value parseValue(const std::string& raw);

// Parses the token list into a ParsedQuery given the SQL action verb (e.g. "create", "insert", "select")
// Throws std::runtime_error on malformed input
// @param tokens The token list
// @param action The SQL action verb
// @return The parsed query
ParsedQuery parse(const std::vector<Token>& tokens, const std::string& action);

#endif