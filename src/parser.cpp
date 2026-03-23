#include "parser.h"
#include <iostream>
#include <stdexcept>

// Advances past any symbol tokens, then checks the next token matches the expected type
// Throws std::runtime_error if the token is missing or the type doesn't match
// @param tokens The token vector
// @param i The current index, advanced in place
// @param type The expected token type
// @return The matched token
static const Token& expect(const std::vector<Token>& tokens, size_t& i, const Token::Type type){
    while(i + 1 < tokens.size() && tokens[i + 1].type == Token::SYMBOL) i++;

    if (i + 1 >= tokens.size() || tokens[i + 1].type != type)
        throw std::runtime_error("Unexpected token at position " + std::to_string(i + 1));
    
    return tokens[++i];
}

// Checks if the token is a keyword and it matches the keyword name
// @param t The token to check
// @param kw The keyword to compare
// @return true if the type is a keyword and the keywords match
static bool matchKeyword(const Token& t, const std::string& kw){
    return t.type == Token::KEYWORD && t.value == kw;
}

// Maps a type string ("INT", "INTEGER", "TEXT") to a Column::Type
// Throws std::runtime_error for unrecognised type strings
// @param raw The raw type string
// @return The parsed Column::Type
static Column::Type parseColumnType(const std::string& raw){
    if(raw == "INT" || raw == "INTEGER") return Column::Type::INTEGER;
    if(raw == "TEXT") return Column::Type::TEXT;
    throw std::runtime_error("Unknown column type: " + raw);
}

// Parses a raw string as an int if possible, otherwise strips surrounding quotes and returns it as a string
// @param raw The string to parse
// @return The parsed Value
static Value parseValue(const std::string& raw){
    std::string s = raw;

    s.erase(std::remove(s.begin(), s.end(), '\''), s.end());
    s.erase(std::remove(s.begin(), s.end(), '"'), s.end());

    try{
        return std::stoi(s);
    } catch (...) {
        return s;
    }
}

// Consumes a comma-separated name+type column list from tokens starting at i
// Stops at the first token that is not an identifier or symbol
// @param tokens The token list
// @param i The current index, advanced in place
// @return A vector of all the parsed columns
static std::vector<Column> parseColumnList(const std::vector<Token>& tokens, size_t& i, bool hasType){
    std::vector<Column> columns;

    while(i < tokens.size()){
        const Token& t = tokens[i];
        if(t.type == Token::SYMBOL) {i++; continue;}

        if(t.type != Token::IDENTIFIER) break;

        Column col;
        col.name = t.value;

        if(hasType)
            col.type = parseColumnType(expect(tokens, i, Token::IDENTIFIER).value);

        columns.push_back(col);
        i++;
    }

    return columns;
}

// Consumes a list of literal values from tokens starting at i
// Stops at the first token that is not a literal or symbol
// @param tokens The token list
// @param i The current index, advanced in place
// @return A vector of the parsed values
static std::vector<Value> parseValueList(const std::vector<Token>& tokens, size_t& i){
    std::vector<Value> values;

    while(i < tokens.size()){
        const Token& t = tokens[i];

        if(t.type == Token::SYMBOL) {i++; continue;}
        
        if(t.type != Token::LITERAL) break;

        values.push_back(parseValue(t.value));
        i++;
    }

    return values;
}

ParsedQuery parse(const std::vector<Token>& tokens, const std::string& action){
    ParsedQuery result;
    result.action = action;

    size_t i = 0;
    while(i < tokens.size()){
        const Token& t = tokens[i];
        if(t.type != Token::KEYWORD){
            i++;
            continue;
        }

        if((result.action == "create" || result.action == "drop") && matchKeyword(t, "TABLE")){
            result.tableName = expect(tokens, i, Token::IDENTIFIER).value;
            i++;
            if(result.action == "create")
                result.columns = parseColumnList(tokens, i, true);
        } 
        else if(result.action == "select" && matchKeyword(t, "SELECT")){
            i++; 

            // handle SELECT *
            if(tokens[i].type == Token::IDENTIFIER && tokens[i].value == "*"){
                result.columns = std::nullopt;
                i++;
                continue;
            } else {
                result.columns = parseColumnList(tokens, i, false);
                continue;
            }
        }
        else if(matchKeyword(t, "FROM") || matchKeyword(t, "INTO") || matchKeyword(t, "UPDATE")){
            result.tableName = expect(tokens, i, Token::IDENTIFIER).value;
        }
        else if (matchKeyword(t, "WHERE")){
            result.whereColumn = expect(tokens, i, Token::IDENTIFIER).value;
            while(i + 1 < tokens.size() && tokens[i + 1].type == Token::SYMBOL) i++;
            result.whereValue = parseValue(expect(tokens, i, Token::LITERAL).value);
        } 
        else if (matchKeyword(t, "VALUES")) {
            i++;
            result.values = parseValueList(tokens, i);
        } 
        else if(matchKeyword(t, "ORDER")){
            if(i + 1 < tokens.size() && matchKeyword(tokens[i + 1], "BY")) i++;
            result.column = expect(tokens, i, Token::IDENTIFIER).value;

            if(i + 1 < tokens.size() && tokens[i + 1].type == Token::IDENTIFIER){
                if(tokens[i + 1].value == "DESC") { result.orderByDesc = true;  i++; }
                else if(tokens[i + 1].value == "ASC")  { result.orderByDesc = false; i++; }
            }
        } 
        else if(matchKeyword(t, "SET")){
            result.column = expect(tokens, i, Token::IDENTIFIER).value;
            while(i + 1 < tokens.size() && tokens[i + 1].type == Token::SYMBOL) i++;
            result.value = parseValue(expect(tokens, i, Token::LITERAL).value);
        }

        i++;
    }
    return result;
}
