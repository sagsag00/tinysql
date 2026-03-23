#include "printer.h"

static std::string toString(const std::variant<int, std::string>& v) {
    return std::visit([](auto&& value) -> std::string {
        using T = std::decay_t<decltype(value)>;

        if constexpr (std::is_same_v<T, std::string>)
            return value;
        else
            return std::to_string(value);
    }, v);
}

void formatResult(const bool& success, const std::string& tableName, const std::string& action){
    std::cout << "The action: '" << action
          << " in table " << tableName 
          << (success ? "' was " : "' wasn't ")
          << "successful"
          << std::endl;
}

void formatResult(const std::vector<Row>& rows, const std::vector<Column>& columns){
    printTable(columns, rows);
}

void printTable(const std::vector<Column>& columns, const std::vector<Row>& rows){
    std::vector<size_t> widths(columns.size());

    for(size_t i = 0; i < columns.size(); i++){
        widths[i] = columns[i].name.length();
    }

    for(const Row& row : rows){
        for(size_t i = 0; i < row.values.size(); i++){
            widths[i] = std::max(widths[i], toString(row.values[i]).length());
        }
    }    

    for(size_t i = 0; i < columns.size(); i++){
        std::cout << std::left << std::setw(widths[i] + 2) << columns[i].name;
    }
    std::cout << "\n";

    for(size_t i = 0; i < columns.size(); i++){
        std::cout << std::string(widths[i], '-') << " ";
    }
    std::cout << "\n";

    for(const Row& row: rows){
        for(size_t i = 0; i < row.values.size(); i++){
            std::cout << std::left << std::setw(widths[i] + 2) << toString(row.values[i]);
        }
        std::cout << "\n";
    }
}

void printTable(const Table& table){
    printTable(table.columns, table.rows);
}

void printTable(const std::string& tableName){
    Table* table = Database::getInstance()->getTable(tableName);
    if(!table) {
        std::cout << "Table " << tableName << " not found" << std::endl;
        return;
    }
    
    printTable(table->columns, table->rows);
}
