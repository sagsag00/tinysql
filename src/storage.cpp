#include "storage.h"

static void saveTable(std::ofstream& f, const std::string& name, const Table& table) {
    f << name;
    for (const auto& col : table.columns)
        f << "," << col.name << ":" << (col.type == Column::Type::INTEGER ? "INT" : "TEXT");
    f << "\n";

    for (const auto& row : table.rows) {
        for (size_t i = 0; i < row.values.size(); i++) {
            if (i > 0) f << ",";
            std::visit([&](const auto& v) { f << v; }, row.values[i]);
        }
        f << "\n";
    }
}

void saveTables(const std::vector<Table>& tables, const std::string& path, bool oneFile) {
    if (oneFile) {
        std::ofstream f(path);
        if (!f) throw std::runtime_error("Cannot open file: " + path);
        for (const auto& table : tables) {
            saveTable(f, table.name, table);
            f << "\n";
        }
        return;
    } 
    for (const auto& table : tables) {
        std::ofstream f(path + "/" + table.name + ".csv");
        if (!f) throw std::runtime_error("Cannot open file: " + path + "/" + table.name + ".csv");
        saveTable(f, table.name, table);
    }
}

static Table loadTable(std::ifstream& f, const std::string& firstLine) {
    Table table;

    std::istringstream ss(firstLine);
    std::string cell;
    bool first = true;
    while (std::getline(ss, cell, ',')) {
        if (first) { table.name = cell; first = false; continue; }
        auto colon = cell.find(':');
        Column col;
        col.name = cell.substr(0, colon);
        col.type = cell.substr(colon + 1) == "INT"
            ? Column::Type::INTEGER : Column::Type::TEXT;
        table.columns.push_back(col);
    }

    std::string line;
    while (std::getline(f, line) && !line.empty()) {
        std::istringstream ss(line);
        std::string cell;
        Row row;
        size_t i = 0;
        while (std::getline(ss, cell, ',')) {
            if (table.columns[i].type == Column::Type::INTEGER)
                row.values.push_back(std::stoi(cell));
            else
                row.values.push_back(cell);
            i++;
        }
        table.rows.push_back(row);
    }

    return table;
}

void loadTables(const std::string& path, bool oneFile) {
    if (oneFile) {
        std::ifstream f(path);
        if (!f) throw std::runtime_error("Cannot open file: " + path);

        std::string line;
        while (std::getline(f, line)) {
            if (line.empty()) continue;
            Database::getInstance().addTable(loadTable(f, line));
        }
        return;
    } 
    std::error_code ec;
    for (const auto& entry : std::filesystem::directory_iterator(path, ec)) {
        if (ec) throw std::runtime_error("Cannot iterate directory: " + path);
        if (entry.path().extension() != ".csv") continue;
        std::ifstream f(entry.path());
        if (!f) throw std::runtime_error("Cannot open file: " + entry.path().string());

        std::string line;
        if (std::getline(f, line))
            Database::getInstance().addTable(loadTable(f, line));
    }
    
}

StorageArgs parseStorageArgs(const std::string& args){
    std::string path;
    std::optional<bool> oneFile;

    std::istringstream ss(args);
    std::string token;
    while(ss >> token){
        if(token.substr(0, 7) == "onefile"){
            oneFile = token.substr(8) != "false";
        } else if(token.substr(0, 4) == "path"){
            path = token.substr(5);
        } else {
            path = token;
        }
    }
    if(!path.empty() && (path.back() == '/' || path.back() == '\\'))
        path.pop_back();
    
    bool endsInCsv = path.size() >= 4 && path.substr(path.size() - 4) == ".csv";

    if(!oneFile.has_value())
        oneFile = path.empty() || endsInCsv;

    if(path.empty()){
        path = oneFile.value() ? "db.csv" : "db";
    } else if(oneFile.value() && !endsInCsv){
        path += ".csv";
    } else if (!oneFile.value() && endsInCsv){
        path = path.substr(0, path.size() - 4);
    }

    return { path, oneFile.value() };
}