#include <gtest/gtest.h>
#include "parser.h"
#include "tokenizer.h"

static ParsedQuery tokenizeAndParse(const std::string& input, const std::string& action){
    return parse(tokenize(input), action);
}

TEST(ParserTest, SelectAllFromTable){
    auto q = tokenizeAndParse("SELECT * FROM users", "select");
    EXPECT_EQ(q.action, "select");
    EXPECT_EQ(q.tableName, "users");
}

TEST(ParserTest, SelectWithWhere){
    auto q = tokenizeAndParse("SELECT * FROM users WHERE name = 'alice'", "select");
    EXPECT_EQ(q.tableName, "users");
    ASSERT_TRUE(q.columnName.has_value());
    EXPECT_EQ(q.columnName.value(), "name");
    ASSERT_TRUE(q.value.has_value());
    EXPECT_EQ(std::get<std::string>(q.value.value()), "alice");
}

TEST(ParserTest, SelectWithWhereInt){
    auto q = tokenizeAndParse("SELECT * FROM users WHERE id = '42'", "select");
    ASSERT_TRUE(q.value.has_value());
    EXPECT_EQ(std::get<int>(q.value.value()), 42);
}

TEST(ParserTest, OrderBy){
    auto q = tokenizeAndParse("SELECT * FROM users ORDER BY name DESC", "select");
    EXPECT_EQ(q.tableName, "users");
    EXPECT_TRUE(q.orderByColumn.has_value());
    EXPECT_EQ(q.orderByColumn, "name");
    EXPECT_TRUE(q.orderByDesc);
}

TEST(ParserTest, OrderByWhere){
    auto q = tokenizeAndParse("SELECT * FROM users WHERE id = '2' ORDER BY name DESC", "select");
    EXPECT_EQ(q.tableName, "users");
    ASSERT_TRUE(q.columnName.has_value());
    EXPECT_EQ(q.columnName.value(), "id");
    ASSERT_TRUE(q.value.has_value());
    EXPECT_EQ(std::get<int>(q.value.value()), 2);
    EXPECT_TRUE(q.orderByColumn.has_value());
    EXPECT_EQ(q.orderByColumn, "name");
    EXPECT_TRUE(q.orderByDesc);
}

TEST(ParserTest, InsertIntoTable){
    auto q = tokenizeAndParse("INSERT INTO users VALUES('alice', '30')", "insert");
    EXPECT_EQ(q.action, "insert");
    EXPECT_EQ(q.tableName, "users");
    ASSERT_TRUE(q.values.has_value());
    ASSERT_EQ(q.values->size(), 2u);
    EXPECT_EQ(std::get<std::string>(q.values->at(0)), "alice");
    EXPECT_EQ(std::get<int>(q.values->at(1)), 30);
}

TEST(ParserTest, InsertSingleValue){
    auto q = tokenizeAndParse("INSERT INTO items VALUES('sword')", "insert");
    ASSERT_TRUE(q.values.has_value());
    ASSERT_EQ(q.values->size(), 1u);
    EXPECT_EQ(std::get<std::string>(q.values->at(0)), "sword");
}

TEST(ParserTest, CreateTable){
    auto q = tokenizeAndParse("CREATE TABLE users (id INTEGER, name TEXT)", "create");
    EXPECT_EQ(q.action, "create");
    EXPECT_EQ(q.tableName, "users");
    ASSERT_TRUE(q.columns.has_value());
    ASSERT_EQ(q.columns->size(), 2u);
    EXPECT_EQ(q.columns->at(0).name, "id");
    EXPECT_EQ(q.columns->at(0).type, Column::Type::INTEGER);
    EXPECT_EQ(q.columns->at(1).name, "name");
    EXPECT_EQ(q.columns->at(1).type, Column::Type::TEXT);
}

TEST(ParserTest, CreateTableSingleColumn){
    auto q = tokenizeAndParse("CREATE TABLE logs (message TEXT)", "create");
    ASSERT_TRUE(q.columns.has_value());
    ASSERT_EQ(q.columns->size(), 1u);
    EXPECT_EQ(q.columns->at(0).name, "message");
    EXPECT_EQ(q.columns->at(0).type, Column::Type::TEXT);
}

TEST(ParserTest, DeleteFromTable){
    auto q = tokenizeAndParse("DELETE FROM users WHERE name = 'alice'", "delete");
    EXPECT_EQ(q.action, "delete");
    EXPECT_EQ(q.tableName, "users");
    ASSERT_TRUE(q.columnName.has_value());
    EXPECT_EQ(q.columnName.value(), "name");
    ASSERT_TRUE(q.value.has_value());
    EXPECT_EQ(std::get<std::string>(q.value.value()), "alice");
}

TEST(ParserTest, DropTable){
    auto q = tokenizeAndParse("DROP TABLE users", "drop");
    EXPECT_EQ(q.action, "drop");
    EXPECT_EQ(q.tableName, "users");
    EXPECT_FALSE(q.columns.has_value());
    EXPECT_FALSE(q.values.has_value());
}

TEST(ParserTest, DropTableNoColumnsOrValues){
    auto q = tokenizeAndParse("DROP TABLE orders", "drop");
    EXPECT_EQ(q.tableName, "orders");
    EXPECT_FALSE(q.columns.has_value());
    EXPECT_FALSE(q.values.has_value());
}

TEST(ParserTest, MissingtableNameThrows){
    EXPECT_THROW(tokenizeAndParse("SELECT * FROM", "select"), std::runtime_error);
}

TEST(ParserTest, UnknownColumnTypeThrows){
    EXPECT_THROW(
        tokenizeAndParse("CREATE TABLE users (id BLOB)", "create"),
        std::runtime_error
    );
}

