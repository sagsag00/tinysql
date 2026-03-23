#ifndef STORAGE_H
#define STORAGE_H

#include <fstream>
#include <unordered_map>
#include <string>
#include <iostream>
#include <vector>
#include <sstream>
#include <filesystem>
#include <optional>
#include "table.h"

struct StorageArgs {
    std::string path;
    bool oneFile;
};

/**
 * Parses the argument string following a .save or .load command and resolves
 * defaults and inferences so callers never need to handle missing args.
 *
 * Inference rules (applied in order):
 *   - No args                          → path="db.csv",  oneFile=true
 *   - Path ends in .csv                → oneFile=true  (unless explicitly false)
 *   - Path has no .csv extension       → oneFile=false (unless explicitly true)
 *   - onefile=true, path has no .csv   → ".csv" appended to path
 *   - onefile=false, path ends in .csv → ".csv" stripped, treated as directory
 *   - onefile=true,  no path           → path="db.csv"
 *   - onefile=false, no path           → path="db"
 *
 * Accepted token formats:
 *   path=<value>      explicit path
 *   onefile=<bool>    "true" or "false"
 *   <bare token>      treated as positional path
 *
 * @param args  Everything after ".save " / ".load " (may be empty).
 * @return      Fully resolved StorageArgs.
 */
StorageArgs parseStorageArgs(const std::string& args);

/**
 * Persists all tables to disk.
 * One-file mode:   Writes every table sequentially into a single CSV at @p path.
 *                  Tables are separated by a blank line.
 *                  Format per table:
 *                    <name>,<col>:<type>,...\n
 *                    <val>,<val>,...\n          (one line per row)
 * 
 * @param tables   Tables to persist.
 * @param path     Destination file (oneFile=true) or directory (oneFile=false).
 * @param oneFile  Storage mode selector.
 * @throws std::runtime_error  If any file cannot be opened for writing.
 */
void saveTables(const std::vector<Table>& tables, const std::string& path, bool oneFile);

/**
 * Loads tables from disk and registers them with the global Database singleton.
 * 
 *  One-file mode:   Reads a single CSV at @p path, splitting on blank lines to
 *                   recover individual tables.
 *
 * @param path     Source file (oneFile=true) or directory (oneFile=false).
 * @param oneFile  Storage mode selector.
 * @throws std::runtime_error  If the file/directory cannot be opened or iterated.
 */
void loadTables(const std::string& path, bool oneFile);

#endif