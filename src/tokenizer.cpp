#include "tokenizer.h"

static const std::unordered_set<std::string> keywords = {
    "CREATE", "TABLE", "INSERT", "INTO", "SELECT", "FROM", "WHERE",
    "DELETE", "DROP", "VALUES", "ORDER", "BY", "UPDATE", "SET",
    "IN", "BETWEEN", "AND", "LIKE", "NOT"
};

// Reads literal from input and updates i to be after literals
// @param input
// @param i
static std::string readLiteral(const std::string& input, size_t& i) {
    char quoteChar = input[i];
    std::string literal;
    literal += quoteChar;
    i++;
    while(i < input.size() && input[i] != quoteChar) {
        literal += input[i];
        i++;
    }
    if(i < input.size()) literal += input[i]; // add closing quote
    return literal;
}

// Pushes symbol to tokens
// @param tokens
// @param ch The symbol
static void pushSymbol(std::vector<Token>& tokens, const std::string sym) {
    tokens.push_back({Token::SYMBOL, sym});
}

// Checks if a word is a part of the keywords
// @param word The word to check
static bool isKeyword(const std::string word){
    std::string upperWord = word;
    std::transform(upperWord.begin(), upperWord.end(), upperWord.begin(), ::toupper);

    return keywords.find(upperWord) != keywords.end();
}

// Pushes current token to tokens and clears current token after
// @param tokens 
// @param currentToken
static void pushCurrentToken(std::vector<Token>& tokens, std::string& currentToken){
    if(currentToken.empty()) return;

    bool isNumber = !currentToken.empty() &&
                    std::all_of(currentToken.begin(), currentToken.end(), ::isdigit);

    Token::Type type = isNumber ? Token::LITERAL 
                     : isKeyword(currentToken) ? Token::KEYWORD 
                     : Token::IDENTIFIER;
    tokens.push_back({type, currentToken});
    currentToken.clear();
}

std::vector<Token> tokenize(const std::string& input){
    std::vector<Token> tokens;
    std::string currentToken;

    for(size_t i = 0; i < input.size(); i++) {
        char ch = input[i];

        if(ch == '\'' || ch == '"') {
            pushCurrentToken(tokens, currentToken);
            std::string literal = readLiteral(input, i);
            tokens.push_back({Token::LITERAL, literal});
        } 
        else if(ch == ' '){
            pushCurrentToken(tokens, currentToken);
        }
        else if(ch == ',' || ch == '(' || ch == ')' || ch == '=' ||
                ch == '>' || ch == '<' || ch == '!'){
            pushCurrentToken(tokens, currentToken);
            
            // Handle >=, <=, !=, <>
            if(i + 1 < input.size()){
                char next = input[i + 1];
                if((ch == '>' || ch == '<' || ch == '!') && next == '='){
                    pushSymbol(tokens, std::string(1, ch) + "=");
                    i++;
                    continue;
                }
                if(ch == '<' && next == '>'){
                    pushSymbol(tokens, "<>");
                    i++;
                    continue;
                }
            }

            pushSymbol(tokens, std::string(1, ch));
        }
        else {
            currentToken += ch;
        }
    }

    pushCurrentToken(tokens, currentToken);

    return tokens;
}