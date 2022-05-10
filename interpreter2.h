#pragma once
#include "lexical2.h"
#include <unordered_map>
#include <vector>
#include <stack>

class Interpreter
{
public:
    Interpreter(const std::vector<Lexeme>& program,
                std::unordered_map<std::string, Value>);
    Interpreter(const Interpreter& rhs) = delete;
    Interpreter& operator = (const Interpreter& rhs) = delete;

    void Run(bool debug = false);

private:
    void HandleRead();
    void HandleWrite(size_t ctr);
    void HandleAssign();
    void HandleBinary(LexemeType type);
    void HandleUnary(LexemeType type);
    Value& ResolveValue(Lexeme& lex);

    const std::vector<Lexeme> m_program;
    std::unordered_map<std::string, Value> m_variables;
    std::stack<Lexeme> m_stack;
};
