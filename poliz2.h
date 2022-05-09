#pragma once
#include "lexical2.h"
#include "interpreter2.h"
#include <variant>
#include <vector>
#include <unordered_map>

class Poliz
{
public:
    Poliz() = default;
    Poliz(const Poliz& rhs) = delete;
    Poliz& operator = (const Poliz& rhs) = delete;

    void AddIdentifier(const std::string& identifier, const Value& value = {});
    bool HasIdentifier(const std::string& identifier) const;
    size_t AddGoto();
    size_t AddConditionalGoto();
    long long int GetCurrentLabel() const;
    void SetLabel(size_t pos, long long int label);
    void SetLabel(size_t pos);
    void AddLexeme(const Lexeme& lexeme);
    
    const std::vector<Lexeme>& GetProgram() const;
    Interpreter CreateInterpreter() const;

private:
    std::unordered_map<std::string, Value> m_variables;
    std::vector<Lexeme> m_poliz;
};

