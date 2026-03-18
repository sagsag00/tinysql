#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <vector>
#include <string>
#include <unordered_set>
#include <algorithm>

const std::unordered_set<std::string> keywords = {
    "CREATE", "TABLE", "INSERT", "INTO", "SELECT", "FROM", "WHERE", "DELETE", "DROP"
};

struct Token
{
    enum Type {KEYWORD, IDENTIFIER, LITERAL, SYMBOL} type;
    std::string value;
};

bool isKeyword(std::string word);
void pushCurrentToken(std::vector<Token>& tokens, std::string& currentToken);
void pushSymbol(std::vector<Token>& tokens, char ch);
std::string readLiteral(const std::string& input, size_t& i);

std::vector<Token> tokenize(const std::string& input);

#endif