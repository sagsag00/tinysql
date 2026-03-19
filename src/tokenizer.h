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

// Checks if a word is a part of the keywords
// @param word The word to check
bool isKeyword(const std::string word);

// Pushes current token to tokens and clears current token after
// @param tokens 
// @param currentToken
void pushCurrentToken(std::vector<Token>& tokens, std::string& currentToken);

// Pushes symbol to tokens
// @param tokens
// @param ch The symbol
void pushSymbol(std::vector<Token>& tokens, const char ch);

// Reads literal from input and updates i to be after literals
// @param input
// @param i
std::string readLiteral(const std::string& input, size_t& i);

// Tokenizes an input
// @param input
std::vector<Token> tokenize(const std::string& input);

#endif