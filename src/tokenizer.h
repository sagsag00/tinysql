#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <vector>
#include <string>
#include <unordered_set>
#include <algorithm>

struct Token
{
    enum Type {KEYWORD, IDENTIFIER, LITERAL, SYMBOL} type;
    std::string value;
};

// Tokenizes an input
// @param input
std::vector<Token> tokenize(const std::string& input);

#endif