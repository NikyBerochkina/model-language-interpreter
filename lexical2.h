#pragma once
#include <string>
#include <variant>
#include <istream>

enum class LexemeType
{
    Undefined = 0,
    Program,
    LeftBrace,
    RightBrace,
    Semicolon,
    Comma,
    Int,
    String,
    Boolean,
    Assign,
    If,
    Else,
    While,
    Read,
    Write,
    Literal,
    LeftParenthesis,
    RightParenthesis,
    Not,
    Multiply,
    Divide,
    Plus,
    Minus,
    Less,
    Greater,
    NotLess,
    NotGreater,
    Equal,
    NotEqual,
    And,
    Or,
    Identifier,
    Break,
    UnaryMinus,
    UnaryPlus,
    Goto,
    ConditionalGoto,
    Clear,
    Eof,
};

using Value = std::variant<bool, long long int, std::string>;

struct Lexeme
{
    LexemeType type;
    Value value;
};

class Scanner
{
public:
    Scanner(std::istream& input);
    Scanner(const Scanner& rhs) = delete;
    Scanner& operator = (const Scanner& rhs) = delete;

    Lexeme GetLexeme();
    int GetCurrentLine() const;

private:
    std::istream& m_input;
    int m_currentLine{1};
};

class lexical_exception
    : public std::runtime_error
{
public:
    lexical_exception(const char* msg, int line, char ch);
    std::string DebugInfo() const;

private:
    const int m_line{};
    const char m_ch{};
};

std::ostream& operator << (std::ostream& os, const Lexeme& lex);
