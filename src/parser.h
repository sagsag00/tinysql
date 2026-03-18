#ifndef PARSER.H
#define PARSER.H

#include <vector>
#include <string>
#include "tokenizer.h"

bool parseCreate(std::vector<Token>& tokens);
bool parseSelect(std::vector<Token>& tokens);
bool parseDelete(std::vector<Token>& tokens);
bool parseDrop(std::vector<Token>& tokens);

#endif