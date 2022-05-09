#pragma once
#include "lexical2.h"
#include <optional>

class Poliz;

class Parser
{
public:
    Parser(Scanner& scanner, Poliz& poliz);
    Parser(const Parser& rhs) = delete;
    Parser& operator = (const Parser& rhs) = delete;

    void Analize();

private:
    Lexeme GetLexeme();
    void SaveLexeme(Lexeme&& lexeme);

    void AnalizeProgram(bool defenitions);
    void AnalizeDefenitions();
    void AnalizeDefenition(LexemeType type);
    void AnalizeVariable(LexemeType type);
    void AnalizeOperators();
    bool AnalizeOperator();
    void AnalizeIf();
    void AnalizeWhile();
    void AnalizeRead();
    void AnalizeWrite();
    void AnalizeExpressionOperator();
    void AnalizeExpression();
    void AnalizeExpressionFirstLevel();
    void AnalizeExpressionSecondLevel();
    void AnalizeExpressionThirdLevel();

    Scanner& m_scanner;
    std::optional<Lexeme> m_saved;

    Poliz& m_poliz;
};

class syntax_exception
    : public std::runtime_error
{
public:
    syntax_exception(const char* msg, int line, Lexeme lexeme);
    std::string DebugInfo() const;

private:
    const int m_line{};
    const Lexeme m_lexeme{};
};
