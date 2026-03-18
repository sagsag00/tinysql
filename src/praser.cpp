#include "parser.h"
#include <stdexcept>

static const Token& expect(const std::vector<Token>& tokens, size_t& i, Token::Type type){
    while(++i < tokens.size() && tokens[i].type == Token::SYMBOL);

    if (i>= tokens.size() || tokens[i].type != type)
        throw std::runtime_error("Unexpected token at position " + std::to_string(i));

    return tokens[i++];
}

static bool matchKeyword(const Token& t, const std::string& kw){
    return t.type == Token::KEYWORD && t.value == kw;
}

static Column::Type parseColumnType(const std::string& raw){
    if(raw == "INT" || raw == "INTEGER") return Column::Type::INTEGER;
    if(raw == "TEXT") return Column::Type::TEXT;
    throw std::runtime_error("Unknown column type: " + raw);
}

static std::vector<Column> parseColumnList(const std::vector<Token>& tokens, size_t& i){
    std::vector<Column> columns;

    while(i < tokens.size()){
        const Token& t = tokens[i];

        if(t.type == Token::SYMBOL) {i++; continue;}

        if(t.type != Token::IDENTIFIER) break;

        Column col;
        col.name = t.value;
        col.type = parseColumnType(expect(tokens, ++i, Token::IDENTIFIER).value);
        columns.push_back(col);
        i++;
    }

    return columns;
}

ParsedQuery parse(const std::vector<Token>& tokens, const std::string& action){
    ParsedQuery result;
    result.action = action;

    for(size_t i = 0; i < tokens.size(); i++){
        const Token& t = tokens[i];

        if(t.type != Token::KEYWORD) continue;

        if(result.action == "create" && matchKeyword(t, "TABLE")){
            result.tableName = expect(tokens, i, Token::IDENTIFIER).value;
            i++;  // move past table name into column list
            result.columns = parseColumnList(tokens, i);
        }

        if(matchKeyword(t, "FROM") || matchKeyword(t, "INTO")){
            result.tableName = expect(tokens, i, Token::IDENTIFIER).value;
        }
        else if (matchKeyword(t, "WHERE")){
            result.columnName = expect(tokens, i, Token::IDENTIFIER).value;
            while(i + 1 < tokens.size() && tokens[i + 1].type == Token::SYMBOL) i++;
            result.value = parseValue(expect(tokens, i, Token::LITERAL).value);
        }
    }
    return result;
}

Value parseValue(const std::string& raw){
    try{
        return std::stoi(raw);
    } catch (...) {
        if(raw.size() >= 2 && (raw.front() == '\'' || raw.front() == '"'))
            return raw.substr(1, raw.size() - 2);
        return raw;
    }
}

