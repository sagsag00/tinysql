#include <gtest/gtest.h>
#include "engine.h"
#include "parser.h"
#include "tokenizer.h"

class EngineTest: public ::testing::Test{
protected:
    void SetUp() override {
        Database::getInstance()->clear();
    }
};

static Result executeQuery(const std::string& input, const std::string& action){
    return execute(parse(tokenize(input), action));
}

TEST_F(EngineTest, CreateTable){
    auto r = executeQuery("CREATE TABLE users (id INTEGER, name TEXT)", "create");
    EXPECT_EQ(r.tableName, "users");
    EXPECT_EQ(r.action, "create");
    EXPECT_TRUE(std::get<bool>(r.result));
}

TEST_F(EngineTest, CreateTableSingleColumn){
    auto r = executeQuery("CREATE TABLE logs (message TEXT)", "create");
    EXPECT_EQ(r.tableName, "logs");
    EXPECT_TRUE(std::get<bool>(r.result));
}

TEST_F(EngineTest, CreateDuplicateTableReturnsFalse){
    executeQuery("CREATE TABLE products (id INTEGER)", "create");
    auto r = executeQuery("CREATE TABLE products (id INTEGER)", "create");
    EXPECT_FALSE(std::get<bool>(r.result));
}

TEST_F(EngineTest, CreateTableMissingColumnsThrows){
    ParsedQuery q;
    q.action = "create";
    q.tableName = "empty";
    EXPECT_THROW(execute(q), std::runtime_error);
}

TEST_F(EngineTest, InsertIntoTable){
    executeQuery("CREATE TABLE users (id INTEGER, name TEXT)", "create");
    auto r = executeQuery("INSERT INTO users VALUES('1', 'alice')", "insert");
    EXPECT_EQ(r.tableName, "users");
    EXPECT_EQ(r.action, "insert");
    EXPECT_TRUE(std::get<bool>(r.result));
}

TEST_F(EngineTest, InsertSingleValue){
    executeQuery("CREATE TABLE items (label TEXT)", "create");
    auto r = executeQuery("INSERT INTO items VALUES('sword')", "insert");
    EXPECT_TRUE(std::get<bool>(r.result));
}

TEST_F(EngineTest, InsertWrongTypeMismatch){
    executeQuery("CREATE TABLE items (id INTEGER)", "create");
    EXPECT_THROW(
        executeQuery("INSERT INTO items VALUES('hello')", "insert"),
        std::runtime_error
    );
}

TEST_F(EngineTest, InsertMissingValuesThrows){
    executeQuery("CREATE TABLE users (id INTEGER)", "create");
    ParsedQuery q;
    q.action = "insert";
    q.tableName = "users";
    EXPECT_THROW(execute(q), std::runtime_error);
}

TEST_F(EngineTest, SelectAllFromEmptyTable){
    executeQuery("CREATE TABLE orders (id INTEGER, total INTEGER)", "create");
    auto r = executeQuery("SELECT * FROM orders", "select");
    EXPECT_EQ(r.tableName, "orders");
    EXPECT_EQ(r.action, "select");
    EXPECT_TRUE(std::get<std::vector<Row>>(r.result).empty());
}

TEST_F(EngineTest, SelectAllReturnsInsertedRows){
    executeQuery("CREATE TABLE orders (id INTEGER, total INTEGER)", "create");
    executeQuery("INSERT INTO orders VALUES('1', '100')", "insert");
    executeQuery("INSERT INTO orders VALUES('2', '200')", "insert");
    auto rows = std::get<std::vector<Row>>(executeQuery("SELECT * FROM orders", "select").result);
    EXPECT_EQ(rows.size(), 2u);
}

TEST_F(EngineTest, SelectSpecificColumns){
    executeQuery("CREATE TABLE users (id INTEGER, name TEXT)", "create");
    executeQuery("INSERT INTO users VALUES('1', 'alice')", "insert");
    auto r = executeQuery("SELECT name FROM users", "select");
    EXPECT_FALSE(std::get<std::vector<Row>>(r.result).empty());
}
 
TEST_F(EngineTest, SelectWithWhereString){
    executeQuery("CREATE TABLE users (id INTEGER, name TEXT)", "create");
    executeQuery("INSERT INTO users VALUES('1', 'alice')", "insert");
    executeQuery("INSERT INTO users VALUES('2', 'bob')", "insert");
    auto rows = std::get<std::vector<Row>>(
        executeQuery("SELECT * FROM users WHERE name = 'alice'", "select").result);
    EXPECT_EQ(rows.size(), 1u);
}

TEST_F(EngineTest, SelectWithWhereInt){
    executeQuery("CREATE TABLE users (id INTEGER, name TEXT)", "create");
    executeQuery("INSERT INTO users VALUES('1', 'alice')", "insert");
    executeQuery("INSERT INTO users VALUES('2', 'bob')", "insert");
    auto rows = std::get<std::vector<Row>>(
        executeQuery("SELECT * FROM users WHERE id = '2'", "select").result);
    EXPECT_EQ(rows.size(), 1u);
}

TEST_F(EngineTest, SelectWhereNoMatchReturnsEmpty){
    executeQuery("CREATE TABLE users (id INTEGER, name TEXT)", "create");
    executeQuery("INSERT INTO users VALUES('1', 'alice')", "insert");
    auto rows = std::get<std::vector<Row>>(
        executeQuery("SELECT * FROM users WHERE name = 'charlie'", "select").result);
    EXPECT_TRUE(rows.empty());
}

TEST_F(EngineTest, DeleteFromTable){
    executeQuery("CREATE TABLE users (id INTEGER, name TEXT)", "create");
    executeQuery("INSERT INTO users VALUES('1', 'alice')", "insert");
    auto r = executeQuery("DELETE FROM users WHERE name = 'alice'", "delete");
    EXPECT_EQ(r.tableName, "users");
    EXPECT_EQ(r.action, "delete");
    EXPECT_TRUE(std::get<bool>(r.result));
}
 
TEST_F(EngineTest, DeleteRemovesRowFromSelect){
    executeQuery("CREATE TABLE users (id INTEGER, name TEXT)", "create");
    executeQuery("INSERT INTO users VALUES('1', 'alice')", "insert");
    executeQuery("DELETE FROM users WHERE name = 'alice'", "delete");
    auto rows = std::get<std::vector<Row>>(
        executeQuery("SELECT * FROM users", "select").result);
    EXPECT_TRUE(rows.empty());
}
 
TEST_F(EngineTest, DeleteOnlyRemovesMatchingRow){
    executeQuery("CREATE TABLE users (id INTEGER, name TEXT)", "create");
    executeQuery("INSERT INTO users VALUES('1', 'alice')", "insert");
    executeQuery("INSERT INTO users VALUES('2', 'bob')", "insert");
    executeQuery("DELETE FROM users WHERE name = 'alice'", "delete");
    auto rows = std::get<std::vector<Row>>(
        executeQuery("SELECT * FROM users", "select").result);
    EXPECT_EQ(rows.size(), 1u);
}
 
TEST_F(EngineTest, DeleteMissingValueThrows){
    executeQuery("CREATE TABLE users (id INTEGER)", "create");
    ParsedQuery q;
    q.action     = "delete";
    q.tableName  = "users";
    q.columnName = "id";

    EXPECT_THROW(execute(q), std::runtime_error);
}
 
TEST_F(EngineTest, DeleteMissingColumnNameThrows){
    executeQuery("CREATE TABLE users (id INTEGER)", "create");
    ParsedQuery q;
    q.action    = "delete";
    q.tableName = "users";
    q.value     = std::string{"1"};

    EXPECT_THROW(execute(q), std::runtime_error);
}

TEST_F(EngineTest, DropTable){
    executeQuery("CREATE TABLE tmp (id INTEGER)", "create");
    auto r = executeQuery("DROP TABLE tmp", "drop");
    EXPECT_EQ(r.tableName, "tmp");
    EXPECT_EQ(r.action, "drop");
    EXPECT_TRUE(std::get<bool>(r.result));
}
 
TEST_F(EngineTest, DropNonExistingTableReturnsFalse){
    auto r = executeQuery("DROP TABLE nonexistent", "drop");
    EXPECT_FALSE(std::get<bool>(r.result));
}

TEST_F(EngineTest, UnknownActionReturnsEmptyResult){
    ParsedQuery q;
    q.action    = "unknown";
    q.tableName = "users";
    Result r = execute(q);
    EXPECT_TRUE(r.tableName.empty());
    EXPECT_TRUE(r.action.empty());
}