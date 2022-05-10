#include "lexical2.h"
#include <sstream>

#define THROW(msg, line, ch) throw lexical_exception((msg), (line), (ch))

enum class State
{
    Undefined = 0,
    String,
    Comment,
    Number,
    Identifier,
};

Scanner::Scanner(std::istream& input)
    : m_input{input}
{
}

Lexeme Scanner::GetLexeme()
{
    State state{};
    std::string buffer;
    do
    {
        const auto ungetted = m_input.gcount() == 0;

        char ch{};
        if (m_input.get(ch).eof())
        {
            if (state == State::Undefined)
            {
                return {LexemeType::Eof, {}};
            }
            else
            {
                THROW("unexpected eof", m_currentLine, 0);
            }
        }
        if (!ungetted && ch == '\n')
        {
            m_currentLine += 1;
        }

        switch (state)
        {
        case State::Undefined:
            if (ch == '"')
            {
                state = State::String;
            }
            else if (ch == '/')
            {
                if (m_input.get(ch).eof())
                {
                    return {LexemeType::Divide, {}};
                }
                if (ch == '*')
                {
                    state = State::Comment;
                }
                else
                {
                    m_input.unget();
                    return {LexemeType::Divide, {}};
                }
            }
            else if (ch == '+')
            {
                if (m_input.get(ch).eof())
                {
                    return {LexemeType::Plus, {}};
                }
                if (std::isdigit(ch))
                {
                    buffer += ch;
                    state = State::Number;
                }
                else
                {
                    m_input.unget();
                    return {LexemeType::Plus, {}};
                }
            }
            else if (ch == '-')
            {
                if (m_input.get(ch).eof())
                {
                    return {LexemeType::Minus, {}};
                }
                if (std::isdigit(ch))
                {
                    buffer += '-';
                    buffer += ch;
                    state = State::Number;
                }
                else
                {
                    m_input.unget();
                    return {LexemeType::Minus, {}};
                }
            }
            else if (ch == '<')
            {
                if (m_input.get(ch).eof())
                {
                    return {LexemeType::Less, {}};
                }
                if (ch == '=')
                {
                    return {LexemeType::NotGreater, {}};
                }
                else
                {
                    m_input.unget();
                    return {LexemeType::Less, {}}; 
                }
            }
            else if (ch == '>')
            {
                if (m_input.get(ch).eof())
                {
                    return {LexemeType::Greater, {}};
                }
                if (ch == '=')
                {
                    return {LexemeType::NotLess, {}};
                }
                else
                {
                    m_input.unget();
                    return {LexemeType::Greater, {}}; 
                }
            }
            else if (ch == '=')
            {
                if (m_input.get(ch).eof())
                {
                    return {LexemeType::Assign, {}};
                }
                if (ch == '=')
                {
                    return {LexemeType::Equal, {}};
                }
                else
                {
                    m_input.unget();
                    return {LexemeType::Assign, {}}; 
                }
            }
            else if (ch == '!')
            {
                if (m_input.get(ch).eof())
                {
                    THROW("unexpected symbol", m_currentLine, '!');
                }
                if (ch == '=')
                {
                    return {LexemeType::NotEqual, {}};
                }
                else
                {
                    THROW("unexpected symbol", m_currentLine, '!');
                }
            }
            else if (ch == '*')
            {
                return {LexemeType::Multiply, {}};
            }
            else if (ch == '{')
            {
                return {LexemeType::LeftBrace, {}};
            }
            else if (ch == '}')
            {
                return {LexemeType::RightBrace, {}};
            }
            else if (ch == '(')
            {
                return {LexemeType::LeftParenthesis, {}};
            }
            else if (ch == ')')
            {
                return {LexemeType::RightParenthesis, {}};
            }
            else if (ch == ';')
            {
                return {LexemeType::Semicolon, {}};
            }
            else if (ch == ',')
            {
                return {LexemeType::Comma, {}};
            }
            else if (std::isalpha(ch))
            {
                buffer += ch;
                state = State::Identifier;
            }
            else if (std::isdigit(ch))
            {
                buffer += ch;
                state = State::Number;
            }
            break;

        case State::String:
            if (ch != '"')
            {
                buffer += ch;
            }
            else
            {
                return {LexemeType::Literal, buffer};
            }
            break;
        
        case State::Comment:
            if (ch == '*' && !m_input.get(ch).eof())
            {
                if (ch == '/')
                {
                    state = State::Undefined;
                }
                else
                {
                    m_input.unget();
                }
            }
            break;
        
        case State::Number:
            if (std::isdigit(ch))
            {
                buffer += ch;
            }
            else
            {
                m_input.unget();
                return {LexemeType::Literal, std::stoll(buffer)};
            }
            break;
        
        case State::Identifier:
            if (std::isalpha(ch) || std::isdigit(ch))
            {
                buffer += ch;
            }
            else
            {
                m_input.unget();
                if (buffer == "program")
                {
                    return {LexemeType::Program, {}};
                }
                if (buffer == "int")
                {
                    return {LexemeType::Int, {}};
                }
                if (buffer == "string")
                {
                    return {LexemeType::String, {}};
                }
                if (buffer == "boolean")
                {
                    return {LexemeType::Boolean, {}};
                }
                if (buffer == "true")
                {
                    return {LexemeType::Literal, true};
                }
                if (buffer == "false")
                {
                    return {LexemeType::Literal, false};
                }
                if (buffer == "if")
                {
                    return {LexemeType::If, {}};
                }
                if (buffer == "else")
                {
                    return {LexemeType::Else, {}};
                }
                if (buffer == "while")
                {
                    return {LexemeType::While, {}};
                }
                if (buffer == "not")
                {
                    return {LexemeType::Not, {}};
                }
                if (buffer == "read")
                {
                    return {LexemeType::Read, {}};
                }
                if (buffer == "write")
                {
                    return {LexemeType::Write, {}};
                }
                if (buffer == "and")
                {
                    return {LexemeType::And, {}};
                }
                if (buffer == "or")
                {
                    return {LexemeType::Or, {}};
                }
                if (buffer == "break")
                {
                    return {LexemeType::Break, {}};
                }
                return {LexemeType::Identifier, buffer};
            }
            break;
        }
    }
    while (true);
}

int Scanner::GetCurrentLine() const
{
    return m_currentLine;
}

std::ostream& operator << (std::ostream& os, const Lexeme& lex)
{
    if (lex.type == LexemeType::Undefined)
    {
        os << "invalid lexeme";
        return os;
    }

    os << "type: " << static_cast<uint32_t>(lex.type);
    std::visit(
        [&os](const auto& l) {
            using T = std::decay_t<decltype(l)>;
            if constexpr (!std::is_same_v<T, std::monostate>)
            {
                os << ", value: {" << std::boolalpha << l << '}';
            }
        },
        lex.value);
    return os;
}

lexical_exception::lexical_exception(const char* msg, int line, char ch)
    : std::runtime_error{msg}
    , m_line{line}
    , m_ch{ch}
{
}

std::string lexical_exception::DebugInfo() const
{
    std::stringstream ss;
    ss << "line " << m_line;
    if (m_ch)
    {
        ss << ", symbol '" << m_ch << "'";
    }
    return ss.str();
}

