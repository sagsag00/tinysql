#include <iostream>
#include <string>
#include <algorithm>
#include "engine.h"
#include "parser.h"
#include "tokenizer.h"
#include "printer.h"
#include "storage.h"

static std::string extractAction(const std::string& input){
    std::string first;
    for(char c : input){
        if(c == ' ' || c == '\t') break;
        first += std::tolower(c);
    }
    return first;  
}

static void printResult(const Result& r){
    std::visit([&](auto&& val){
        using T = std::decay_t<decltype(val)>;
        if constexpr (std::is_same_v<T, bool>)
            formatResult(val, r.tableName, r.action); // val is bool
        else
            formatResult(val, *r.columns); // val is std::vector<Row>
    }, r.result);
}

static void printAllTables(){
    for(Table table : Database::getInstance().getTables()){
        formatResult(table.rows, table.columns);
    }
}

static void save(std::string& input){
    auto [path, oneFile] = parseStorageArgs(input.size() > 6 ? input.substr(6) : "");

    if (!oneFile) {
        std::error_code ec;
        std::filesystem::create_directories(path, ec);
        if (ec) throw std::runtime_error("Cannot create directory: " + path);
    }

    saveTables(Database::getInstance().getTables(), path, oneFile);
    std::cout << "Saved to " << path << (oneFile ? "" : "/") << "\n";
}

static void load(std::string& input){
    auto [path, oneFile] = parseStorageArgs(input.size() > 6 ? input.substr(6) : "");
    loadTables(path, oneFile);
    std::cout << "Loaded from " << path << (oneFile ? "" : "/") << "\n";
}

int main(){
    std::cout << "TinySQL REPL - type .quit to quit" << std::endl;
    std::cout << "------------------------------------\n";

    std::string input;
    while(true){
        try{
            std::cout << "> ";
            if(!std::getline(std::cin, input)) break;

            size_t start = input.find_first_not_of(" \t");
            if(start == std::string::npos) continue;
            input = input.substr(start);

            if(input == ".quit") break;
            if(input == ".tables") {
                printAllTables();
                continue;
            }
            if (input.substr(0, 5) == ".save") {
                save(input);
                continue;
            }
            if (input.substr(0, 5) == ".load") {
                load(input);
                continue;
            }
            if(input.empty()) continue;


            std::string action = extractAction(input);

            static const std::unordered_set<std::string> validActions = {
                "select", "insert", "update", "delete", "create", "drop"
            };

            if (validActions.find(action) == validActions.end()) {
                std::cout << "Error: Unknown command '" << action << "'\n";
                continue;
            }

            std::vector<Token> tokens = tokenize(input);
            ParsedQuery query = parse(tokens, action);
            Result result = execute(query);
            printResult(result);
        } catch(const std::exception& e){
            std::cout << "Error: " << e.what() << "\n";
        }
    }

    std::cout << "Bye\n";
    return 0;
}