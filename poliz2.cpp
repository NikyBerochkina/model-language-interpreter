#include "poliz2.h"

void Poliz::AddIdentifier(const std::string& identifier, const Value& value)
{
    if (HasIdentifier(identifier))
    {
        throw std::runtime_error("duplicate identifier");
    }
    else
    {
        m_variables.insert({identifier, value});
    }
}

bool Poliz::HasIdentifier(const std::string& identifier) const
{
    return m_variables.find(identifier) != m_variables.end();
}

size_t Poliz::AddGoto()
{
    m_poliz.push_back({LexemeType::Goto, -1ll});
    return m_poliz.size() - 1;
}

size_t Poliz::AddConditionalGoto()
{
    m_poliz.push_back({LexemeType::ConditionalGoto, -1ll});
    return m_poliz.size() - 1;
}

long long int Poliz::GetCurrentLabel() const
{
    return m_poliz.size();
}

void Poliz::SetLabel(size_t pos, long long int label)
{
    if (m_poliz[pos].type != LexemeType::Goto && m_poliz[pos].type != LexemeType::ConditionalGoto)
    {
        throw std::runtime_error("object is not a label");
    }
    m_poliz[pos].value = label;
}

void Poliz::SetLabel(size_t pos)
{
    SetLabel(pos, GetCurrentLabel());
}

void Poliz::AddLexeme(const Lexeme& lexeme)
{
    m_poliz.push_back(lexeme);
}

const std::vector<Lexeme>& Poliz::GetProgram() const
{
    return m_poliz;
}

Interpreter Poliz::CreateInterpreter() const
{
    return Interpreter{m_poliz, m_variables};
}
