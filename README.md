# TinySQL

A lightweight SQL REPL built in C++17. Run SQL queries interactively, persist your data to CSV, and load it back — all from the terminal.
 
---
 
## Features
 
- **SQL queries** — `SELECT`, `INSERT`, `UPDATE`, `DELETE`, `CREATE TABLE`, `DROP TABLE`, `ORDER BY`
- **WHERE clauses** — filter rows by column values
- **Persistent storage** — save and load tables as CSV files
- **Single-file or multi-file storage** — one combined CSV or one file per table
- **Pretty-printed results** — tables formatted in the terminal
- **Lightweight** — no dependencies beyond the C++ standard library (GoogleTest for tests only)
 
---
 
## Installation & Build
 
**Requirements:**
- CMake 3.14+
- GCC/MinGW with C++17 support (or any C++17-compliant compiler)
 
```bash
git clone https://github.com/you/tinysql.git
cd tinysql
cmake --build build
```
 
**Run tests:**
```bash
cd build
ctest 
```
 
---
 
## Usage
 
### SQL Commands
 
**Create a table:**
```sql
CREATE TABLE {table} ({col} INTEGER, {col} TEXT, ...)
```
 
**Insert rows:**
```sql
INSERT INTO {table} VALUES('{value}', '{value}', ...)
```
 
**Query rows:**
```sql
SELECT * FROM {table}
SELECT * FROM {table} WHERE {col} = '{value}'
SELECT {cols} FROM {table} 
SELECT {cols|*} FROM {table} ORDER BY {col} ASC|DESC
SELECT {cols|*} FROM {table} WHERE {col} = '{value}' ORDER BY {col} ASC|DESC
```
 
**Update rows:**
```sql
UPDATE {table} SET {col} = '{value}' WHERE {col} = '{value}'
```
 
**Delete rows:**
```sql
DELETE FROM {table} WHERE {col} = '{value}'
```
 
**Drop a table:**
```sql
DROP TABLE {table}
```
 
### REPL Commands
 
| Command | Description |
|---|---|
| `.tables` | List all tables with their rows |
| `.save [args]` | Save tables to disk |
| `.load [args]` | Load tables from disk |
| `.quit` | Exit the REPL |
 
---
 
## File Format & Storage
 
TinySQL saves tables as plain CSV files that you can open in any spreadsheet app.
 
### `.save` and `.load` arguments
 
Both commands accept the same optional arguments:
 
| Argument | Example | Description |
|---|---|---|
| `path=<value>` | `path=data/` | Where to save/load |
| `onefile=<bool>` | `onefile=false` | Single file or one per table |
| *(bare value)* | `mydb.csv` | Shorthand for `path=` |
 
**Smart defaults — you rarely need to specify anything:**
 
```
> .save                          → saves to db.csv       (onefile=true)
> .save data/                    → saves to data/*.csv   (onefile=false, inferred from directory)
> .save mydb.csv                 → saves to mydb.csv     (onefile=true, inferred from .csv)
> .save path=data/ onefile=true  → saves to data/db.csv
> .load                          → loads from db.csv
> .load data/                    → loads all *.csv from data/
```
 
### File format
 
**Single file (`onefile=true`):**
```
users,id:INT,name:TEXT
1,alice
2,bob
 
orders,id:INT,user_id:INT,item:TEXT
1,1,keyboard
2,2,mouse
```
 
**Multi-file (`onefile=false`):** one file per table, e.g. `data/users.csv`, `data/orders.csv`, each with the same format.
