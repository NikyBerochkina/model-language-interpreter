#include "interpreter2.h"
#include <iostream>

Interpreter::Interpreter(const std::vector<Lexeme>& program,
                         std::unordered_map<std::string, Value> variables)
    : m_program{program}
    , m_variables{variables}
{
}

void Interpreter::Run()
{
    size_t i{0};
    while (i < m_program.size())
    {
        switch (m_program[i].type)
        {
        case LexemeType::Identifier:
        case LexemeType::Literal:
            m_stack.push(m_program[i]);
            i += 1;
            break;

        case LexemeType::Read:
            HandleRead();
            i += 1;
            break;

        case LexemeType::Write:
            HandleWrite(std::get<long long int>(m_program[i].value));
            i += 1;
            break;
        
        case LexemeType::Goto:
            i = std::get<long long int>(m_program[i].value);
            break;
        
        case LexemeType::ConditionalGoto:
            if (std::get<long long int>(m_stack.top().value) == 0)
            {
                i = std::get<long long int>(m_program[i].value);
            }
            else
            {
                i += 1;
            }
            m_stack.pop();
            break;
        
        case LexemeType::Assign:
            HandleAssign();
            i += 1;
            break;

        case LexemeType::Clear:
            while (!m_stack.empty())
            {
                m_stack.pop();
            }
            i += 1;
            break;
        
        case LexemeType::Plus:
        case LexemeType::Minus:
        case LexemeType::Multiply:
        case LexemeType::Divide:
        case LexemeType::Less:
        case LexemeType::NotLess:
        case LexemeType::Greater:
        case LexemeType::NotGreater:
        case LexemeType::Equal:
        case LexemeType::NotEqual:
        case LexemeType::Or:
        case LexemeType::And:
            HandleBinary(m_program[i].type);
            i += 1;
            break;
        
        case LexemeType::Not:
            HandleUnary(m_program[i].type);
            i += 1;
            break;

        default:
            i += 1;
            break;
        }
    }
}

void Interpreter::HandleRead()
{
    auto lex = m_stack.top();
    m_stack.pop();
    if (lex.type != LexemeType::Identifier)
    {
        throw std::runtime_error("identifier expected");
    }

    std::visit(
        [](auto& v) {
            using T = std::decay_t<decltype(v)>;
            if constexpr (!std::is_same_v<T, std::monostate>)
            {
                std::cin >> v;
            }
        },
        ResolveValue(lex));
}

void Interpreter::HandleWrite(size_t ctr)
{
    std::vector<Lexeme> lexes;
    for (size_t i = 0; i < ctr; ++i)
    {
        lexes.insert(lexes.begin(), m_stack.top());
        m_stack.pop();
    }
    for (auto& lex: lexes)
    {
        std::visit(
            [](auto& v) {
                using T = std::decay_t<decltype(v)>;
                if constexpr (!std::is_same_v<T, std::monostate>)
                {
                    std::cout << v << " ";
                }
            },
            ResolveValue(lex));
    }
    std::cout << std::endl;
}

void Interpreter::HandleAssign()
{
    auto rhs = m_stack.top();
    m_stack.pop();

    Value rhsValue = ResolveValue(rhs);

    auto lhs = m_stack.top();
    m_stack.pop();

    if (lhs.type != LexemeType::Identifier)
    {
        throw std::runtime_error("identifier expected");
    }

    auto& lhsValue = ResolveValue(lhs);
    lhsValue = rhsValue;

    m_stack.push(lhs);
}

void Interpreter::HandleBinary(LexemeType type)
{
    auto rhs = m_stack.top();
    m_stack.pop();
    Value rhsValue = ResolveValue(rhs);

    auto lhs = m_stack.top();
    m_stack.pop();
    Value lhsValue = ResolveValue(lhs);
    
    if (lhsValue.index() != rhsValue.index())
    {
        throw std::runtime_error("type mismatch");
    }

    if (std::holds_alternative<std::string>(lhsValue))
    {
        Lexeme result{LexemeType::Literal, {}};
        const auto lhsStr = std::get<std::string>(lhsValue);
        const auto rhsStr = std::get<std::string>(rhsValue);
        switch (type)
        {
        case LexemeType::Plus:
            result.value = lhsStr + rhsStr;
            break;

        case LexemeType::Less:
            result.value = lhsStr < rhsStr;
            break;

        case LexemeType::NotLess:
            result.value = lhsStr >= rhsStr;
            break;

        case LexemeType::Greater:
            result.value = lhsStr > rhsStr;    
            break;

        case LexemeType::NotGreater:
            result.value = lhsStr <= rhsStr;
            break;

        case LexemeType::Equal:
            result.value = lhsStr == rhsStr;
            break;

        case LexemeType::NotEqual:
            result.value = lhsStr != rhsStr;
            break;    

        default:
            throw std::runtime_error("unsupported string operation");
        }
        m_stack.push(std::move(result));
    }
    else if (std::holds_alternative<long long int>(lhsValue))
    {
        Lexeme result{LexemeType::Literal, {}};
        const auto lhsInt = std::get<long long int>(lhsValue);
        const auto rhsInt = std::get<long long int>(rhsValue);
        switch (type)
        {
        case LexemeType::Plus:
            result.value = lhsInt + rhsInt;
            break;
        
        case LexemeType::Minus:
            result.value = lhsInt - rhsInt;
            break;
        
        case LexemeType::Multiply:
            result.value = lhsInt * rhsInt;
            break;
        
        case LexemeType::Divide:
            result.value = lhsInt / rhsInt;
            break;

        case LexemeType::Less:
            result.value = lhsInt < rhsInt;
            break;

        case LexemeType::NotLess:
            result.value = lhsInt >= rhsInt;
            break;

        case LexemeType::Greater:
            result.value = lhsInt > rhsInt;    
            break;

        case LexemeType::NotGreater:
            result.value = lhsInt <= rhsInt;
            break;

        case LexemeType::Equal:
            result.value = lhsInt == rhsInt;
            break;

        case LexemeType::NotEqual:
            result.value = lhsInt != rhsInt;
            break;
        
        case LexemeType::Or:
            result.value = lhsInt || rhsInt;
            break;
        
        case LexemeType::And:
            result.value = lhsInt && rhsInt;
            break;

        default:
            throw std::runtime_error("unsupported string operation");
        }
        m_stack.push(std::move(result));
    }
    else
    {
        throw std::runtime_error("unknown type");
    }
}

void Interpreter::HandleUnary(LexemeType type)
{
    auto op = m_stack.top();
    m_stack.pop();
    Value opValue = ResolveValue(op);
    
    if (std::holds_alternative<long long int>(opValue))
    {
        const auto opInt = std::get<long long int>(opValue);
        switch (type)
        {
        case LexemeType::Not:
            m_stack.push({LexemeType::Literal, !opInt});
            break;
        
        default:
            throw std::runtime_error("unsupported string operation");
        }
    }
    else
    {
        throw std::runtime_error("unknown type");
    }
}

Value& Interpreter::ResolveValue(Lexeme& lex)
{
    if (lex.type == LexemeType::Literal)
    {
        return lex.value;
    }
    else if (lex.type != LexemeType::Identifier)
    {
        throw std::runtime_error("invalid value");
    }
    if (const auto it = m_variables.find(std::get<std::string>(lex.value));
        it != m_variables.end())
    {
        return it->second;
    }
    throw std::runtime_error("unknown variable");
}
