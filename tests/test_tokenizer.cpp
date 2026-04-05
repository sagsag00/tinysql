#include <gtest/gtest.h>
#include "tokenizer.h"

Token makeToken(Token::Type type, const std::string& val){
    return {type, val};
}

static bool tokenEqual(const std::vector<Token>& a, const std::vector<Token>& b){
    if(a.size() != b.size()) return false;
    for(size_t i = 0; i < a.size(); i++){
        if(a[i].type != b[1].type || a[i].value != b[i].value) return false;
    }
    return true;
}

TEST(TokenizerTest, EmptyInput){
    auto tokens = tokenize("");
    EXPECT_TRUE(tokens.empty());
}

TEST(TokenizerTest, OnlySpaces){
    auto tokens = tokenize("    ");
    EXPECT_TRUE(tokens.empty());
}

TEST(TokenizerTest, SingleIdentifier){
    auto tokens = tokenize("foo");
    ASSERT_EQ(tokens.size(), 1u);
    EXPECT_EQ(tokens[0].type, Token::IDENTIFIER);
    EXPECT_EQ(tokens[0].value, "foo");
}

TEST(TokenizerTest, MultipleIdentifiers){
    auto tokens = tokenize("foo bar baz");
    ASSERT_EQ(tokens.size(), 3u);
    for(auto& t: tokens)
        EXPECT_EQ(t.type, Token::IDENTIFIER);
    EXPECT_EQ(tokens[0].value, "foo");
    EXPECT_EQ(tokens[1].value, "bar");
    EXPECT_EQ(tokens[2].value, "baz");
}

TEST(TokenizerTest, KeywordCaseInsensetive){
    for (const auto& kw: {"SELECT", "select", "Select", "sElEcT"}) {
        auto tokens = tokenize(kw);
        ASSERT_EQ(tokens.size(), 1u) << "Input: " << kw;
        EXPECT_EQ(tokens[0].type, Token::KEYWORD) << "Input: " << kw;
    }
}

TEST(TokenizerTest, CommonKeywords){
    for(const auto& kw: {"SELECT", "FROM", "WHERE", "INSERT", "DELETE"}){
        auto tokens = tokenize(kw);
        ASSERT_FALSE(tokens.empty()) << "Keyword: " << kw;
        EXPECT_EQ(tokens[0].type, Token::KEYWORD) << "Keyword: " << kw;
    }
}

TEST(TokenizerTest, KeywordMixedWithIdentifiers){
    auto tokens = tokenize("SELECT foo FROM bar");
    ASSERT_EQ(tokens.size(), 4u);
    EXPECT_EQ(tokens[0].type, Token::KEYWORD);
    EXPECT_EQ(tokens[1].type, Token::IDENTIFIER);
    EXPECT_EQ(tokens[2].type, Token::KEYWORD);
    EXPECT_EQ(tokens[3].type, Token::IDENTIFIER);
}

TEST(TokenizerTest, ParenthesesAndComma){
    auto tokens = tokenize("(a,b)");
    ASSERT_EQ(tokens.size(), 5u);
    EXPECT_EQ(tokens[0].type, Token::SYMBOL);
    EXPECT_EQ(tokens[1].type, Token::IDENTIFIER);
    EXPECT_EQ(tokens[2].type, Token::SYMBOL);
    EXPECT_EQ(tokens[3].type, Token::IDENTIFIER);
    EXPECT_EQ(tokens[4].type, Token::SYMBOL);
}

TEST(TokenizerTest, SymbolsWithSpaces) {
    auto tokens = tokenize("( a , b )");
    ASSERT_EQ(tokens.size(), 5u);
    EXPECT_EQ(tokens[0].value, "(");
    EXPECT_EQ(tokens[1].value, "a");
    EXPECT_EQ(tokens[2].value, ",");
    EXPECT_EQ(tokens[3].value, "b");
    EXPECT_EQ(tokens[4].value, ")");
}

TEST(TokenizerTest, SingleQuotedLiteral) {
    auto tokens = tokenize("'hello'");
    ASSERT_EQ(tokens.size(), 1u);
    EXPECT_EQ(tokens[0].type, Token::LITERAL);
    EXPECT_EQ(tokens[0].value, "'hello'");
}

TEST(TokenizerTest, DoubleQuotedLiteral) {
    auto tokens = tokenize("\"hello\"");
    ASSERT_EQ(tokens.size(), 1u);
    EXPECT_EQ(tokens[0].type, Token::LITERAL);
    EXPECT_EQ(tokens[0].value, "\"hello\"");
}

TEST(TokenizerTest, LiteralWithSpacesInside) {
    auto tokens = tokenize("'hello world'");
    ASSERT_EQ(tokens.size(), 1u);
    EXPECT_EQ(tokens[0].type, Token::LITERAL);
    EXPECT_EQ(tokens[0].value, "'hello world'");
}
 
TEST(TokenizerTest, LiteralWithKeywordInside) {
    // Content inside quotes should NOT be tokenized as a keyword
    auto tokens = tokenize("'SELECT'");
    ASSERT_EQ(tokens.size(), 1u);
    EXPECT_EQ(tokens[0].type, Token::LITERAL);
}

TEST(TokenizerTest, SimpleSelectQuery) {
    auto tokens = tokenize("SELECT name FROM users");
    ASSERT_EQ(tokens.size(), 4u);
    EXPECT_EQ(tokens[0].type, Token::KEYWORD);    EXPECT_EQ(tokens[0].value, "SELECT");
    EXPECT_EQ(tokens[1].type, Token::IDENTIFIER); EXPECT_EQ(tokens[1].value, "name");
    EXPECT_EQ(tokens[2].type, Token::KEYWORD);    EXPECT_EQ(tokens[2].value, "FROM");
    EXPECT_EQ(tokens[3].type, Token::IDENTIFIER); EXPECT_EQ(tokens[3].value, "users");
}

TEST(TokenizerTest, FunctionCallSyntax) {
    auto tokens = tokenize("MAX(age)");
    ASSERT_EQ(tokens.size(), 4u);
    EXPECT_EQ(tokens[0].value, "MAX");
    EXPECT_EQ(tokens[1].value, "(");
    EXPECT_EQ(tokens[2].value, "age");
    EXPECT_EQ(tokens[3].value, ")");
}

TEST(TokenizerTest, InsertWithLiteral) {
    auto tokens = tokenize("INSERT INTO users VALUES('alice',30)");
    ASSERT_EQ(tokens.size(), 9u);
    EXPECT_EQ(tokens[0].type, Token::KEYWORD);    
    EXPECT_EQ(tokens[1].type, Token::KEYWORD);    
    EXPECT_EQ(tokens[2].type, Token::IDENTIFIER); 
    EXPECT_EQ(tokens[3].type, Token::KEYWORD); 
    EXPECT_EQ(tokens[4].type, Token::SYMBOL);    
    EXPECT_EQ(tokens[5].type, Token::LITERAL);    
    EXPECT_EQ(tokens[6].type, Token::SYMBOL);    
    EXPECT_EQ(tokens[7].type, Token::LITERAL);
    EXPECT_EQ(tokens[8].type, Token::SYMBOL);    
}

TEST(TokenizerTest, SelectOrderBy){
    auto tokens = tokenize("SELECT * FROM users WHERE id = '5' ORDER BY name ASC");
    ASSERT_EQ(tokens.size(), 12u);
    EXPECT_EQ(tokens[0].type, Token::KEYWORD);
    EXPECT_EQ(tokens[1].type, Token::IDENTIFIER);
    EXPECT_EQ(tokens[2].type, Token::KEYWORD);
    EXPECT_EQ(tokens[3].type, Token::IDENTIFIER);
    EXPECT_EQ(tokens[4].type, Token::KEYWORD);
    EXPECT_EQ(tokens[5].type, Token::IDENTIFIER);
    EXPECT_EQ(tokens[6].type, Token::SYMBOL);
    EXPECT_EQ(tokens[7].type, Token::LITERAL);
    EXPECT_EQ(tokens[8].type, Token::KEYWORD);
    EXPECT_EQ(tokens[9].type, Token::KEYWORD);
    EXPECT_EQ(tokens[10].type, Token::IDENTIFIER);
    EXPECT_EQ(tokens[11].type, Token::IDENTIFIER);
}