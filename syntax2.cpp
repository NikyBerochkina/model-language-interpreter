#include "syntax2.h"
#include "poliz2.h"
#include <sstream>

#define THROW(msg, line, lex) throw syntax_exception((msg), (line), (lex))

bool IsOperatorFirstLevel(LexemeType op)
{
    return
        op == LexemeType::Less ||
        op == LexemeType::Greater ||
        op == LexemeType::NotLess ||
        op == LexemeType::NotGreater ||
        op == LexemeType::Equal ||
        op == LexemeType::NotEqual;
}

bool IsOperatorSecondLevel(LexemeType op)
{
    return
        op == LexemeType::Minus ||
        op == LexemeType::Plus ||
        op == LexemeType::Or;
}

bool IsOperatorThirdLevel(LexemeType op)
{
    return
        op == LexemeType::Multiply ||
        op == LexemeType::Divide ||
        op == LexemeType::And;
}

Parser::Parser(Scanner& scanner, Poliz& poliz)
    : m_scanner{scanner}
    , m_poliz{poliz}
{
}

void Parser::Analize()
{
    if (const auto lex = GetLexeme(); lex.type != LexemeType::Program)
    {
        THROW("'program' expected", m_scanner.GetCurrentLine(), lex);
    }
    if (const auto lex = GetLexeme(); lex.type != LexemeType::LeftBrace)
    {
        THROW("'{' expected", m_scanner.GetCurrentLine(), lex);
    }
    AnalizeProgram(true);
}

Lexeme Parser::GetLexeme()
{
    Lexeme lexeme;
    if (m_saved)
    {
        lexeme = std::move(*m_saved);
        m_saved.reset();
    }
    else
    {
        lexeme = m_scanner.GetLexeme();
    }
    return lexeme;
}

void Parser::SaveLexeme(Lexeme&& lexeme)
{
    m_saved = std::move(lexeme);
}

void Parser::AnalizeProgram(bool defenitions)
{
    if (defenitions)
    {
        AnalizeDefenitions();
    }
    AnalizeOperators();
    if (GetLexeme().type != LexemeType::RightBrace)
    {
        throw std::runtime_error("'}' expected");
    }
}

void Parser::AnalizeDefenitions()
{
    Lexeme lexeme = GetLexeme();
    while (lexeme.type == LexemeType::Int  
        || lexeme.type == LexemeType::String 
        || lexeme.type == LexemeType::Boolean)
    {
        AnalizeDefenition(lexeme.type);
        lexeme = GetLexeme();
    }
    SaveLexeme(std::move(lexeme));
}

void Parser::AnalizeDefenition(LexemeType type)
{
    Lexeme lexeme;
    do
    {
        AnalizeVariable(type);
        lexeme = GetLexeme();
    }
    while (lexeme.type == LexemeType::Comma);

    if (lexeme.type != LexemeType::Semicolon)
    {
        THROW("';' expected", m_scanner.GetCurrentLine(), lexeme);
    }
}

void Parser::AnalizeVariable(LexemeType type)
{
    auto var = GetLexeme();
    if (var.type != LexemeType::Identifier)
    {
        THROW("identifier expected", m_scanner.GetCurrentLine(), var);
    }

    const auto identifier = std::get<std::string>(var.value);

    auto assign = GetLexeme();
    if (assign.type != LexemeType::Assign)
    {
        Value defaultValue;
        if (type == LexemeType::String)
        {
            defaultValue = std::string{};
        }
        else if (type == LexemeType::Int)
        {
            defaultValue = 0ll;
        }
        else if (type == LexemeType::Boolean)
        {
            defaultValue = false;
        }
        m_poliz.AddIdentifier(identifier, defaultValue);
        SaveLexeme(std::move(assign));
        return;
    }

    auto value = GetLexeme();
    if (value.type != LexemeType::Literal)
    {
        THROW("literal expected", m_scanner.GetCurrentLine(), value);
    }
    if (type == LexemeType::String && !std::get_if<std::string>(&value.value))
    {
        THROW("string literal expected", m_scanner.GetCurrentLine(), value);
    }
    if (type == LexemeType::Int && !std::get_if<long long int>(&value.value))
    {
        THROW("integral literal expected", m_scanner.GetCurrentLine(), value);
    }
    if (type == LexemeType::Boolean && !std::get_if<bool>(&value.value))
    {
        THROW("boolean literal expected", m_scanner.GetCurrentLine(), value);
    }
    m_poliz.AddIdentifier(identifier, value.value);
}

void Parser::AnalizeOperators()
{
    bool isPresent{false};
    do
    {
        isPresent = AnalizeOperator();
    }
    while (isPresent);
}

bool Parser::AnalizeOperator()
{
    switch (auto lexeme = GetLexeme(); lexeme.type)
    {
    case LexemeType::RightBrace:
        SaveLexeme(std::move(lexeme));
        return false;

    case LexemeType::If:
        AnalizeIf();
        break;

    case LexemeType::While:
        AnalizeWhile();
        break;

    case LexemeType::Read:
        AnalizeRead();
        break;

    case LexemeType::Write:
        AnalizeWrite();
        break;
    
    case LexemeType::Break:
        AnalizeBreak();
        break;

    case LexemeType::LeftBrace:
        AnalizeProgram(false);
        break;

    default:
        SaveLexeme(std::move(lexeme));
        AnalizeExpressionOperator();
        break;
    }

    return true;
}

void Parser::AnalizeIf()
{
    if (const auto lex = GetLexeme(); lex.type != LexemeType::LeftParenthesis)
    {
        THROW("'(' expected", m_scanner.GetCurrentLine(), lex);
    }
    AnalizeExpression();
    auto pos1 = m_poliz.AddConditionalGoto();

    if (const auto lex = GetLexeme(); lex.type != LexemeType::RightParenthesis)
    {
        THROW("')' expected", m_scanner.GetCurrentLine(), lex);
    }

    if (!AnalizeOperator())
    {
        THROW("operator expected", m_scanner.GetCurrentLine(), Lexeme{});
    }

    if (auto lex = GetLexeme(); lex.type == LexemeType::Else)
    {
        auto pos2 = m_poliz.AddGoto();

        m_poliz.SetLabel(pos1);
        if (!AnalizeOperator())
        {
            THROW("operator expected", m_scanner.GetCurrentLine(), Lexeme{});
        }
        m_poliz.SetLabel(pos2);
    }
    else 
    {
        SaveLexeme(std::move(lex));
        m_poliz.SetLabel(pos1);
    }
}

void Parser::AnalizeWhile()
{
    if (const auto lex = GetLexeme(); lex.type != LexemeType::LeftParenthesis)
    {
        THROW("'(' expected", m_scanner.GetCurrentLine(), lex);
    }

    const auto label = m_poliz.GetCurrentLabel();
    AnalizeExpression();
    if (const auto lex = GetLexeme(); lex.type != LexemeType::RightParenthesis)
    {
        THROW("')' expected", m_scanner.GetCurrentLine(), lex);
    }

    auto exitPos = m_poliz.AddConditionalGoto();

    m_breaks.push({});
    if (!AnalizeOperator())
    {
        THROW("operator expected", m_scanner.GetCurrentLine(), Lexeme{});
    }
    auto pos = m_poliz.AddGoto();
    m_poliz.SetLabel(pos, label);
    m_poliz.SetLabel(exitPos);
    for (auto pos: m_breaks.top())
    {
        m_poliz.SetLabel(pos);
    }
    m_breaks.pop();
}

void Parser::AnalizeRead()
{
    if (const auto lex = GetLexeme(); lex.type != LexemeType::LeftParenthesis)
    {
        THROW("'(' expected", m_scanner.GetCurrentLine(), lex);
    }
    auto identifier = GetLexeme();
    if (identifier.type != LexemeType::Identifier)
    {
        THROW("identifier expected", m_scanner.GetCurrentLine(), identifier);
    }
    if (!m_poliz.HasIdentifier(std::get<std::string>(identifier.value)))
    {
        THROW("unknown identifier", m_scanner.GetCurrentLine(), identifier);
    }
    if (const auto lex = GetLexeme(); lex.type != LexemeType::RightParenthesis)
    {
        THROW("')' expected", m_scanner.GetCurrentLine(), lex);
    }
    if (const auto lex = GetLexeme(); lex.type != LexemeType::Semicolon)
    {
        THROW("';' expected", m_scanner.GetCurrentLine(), lex);
    }

    m_poliz.AddLexeme(identifier);
    m_poliz.AddLexeme({LexemeType::Read, {}});
}

void Parser::AnalizeWrite()
{
    if (const auto lex = GetLexeme(); lex.type != LexemeType::LeftParenthesis)
    {
        THROW("'(' expected", m_scanner.GetCurrentLine(), lex);
    }

    long long int counter{0};
    Lexeme lex;
    do
    {
        AnalizeExpression();
        counter += 1;
        lex = GetLexeme();
    } while (lex.type == LexemeType::Comma);

    if (lex.type != LexemeType::RightParenthesis)
    {
        THROW("')' expected", m_scanner.GetCurrentLine(), lex);
    }
    if (const auto lex = GetLexeme(); lex.type != LexemeType::Semicolon)
    {
        THROW("';' expected", m_scanner.GetCurrentLine(), lex);
    }
    m_poliz.AddLexeme({LexemeType::Write, counter});
}

void Parser::AnalizeBreak()
{
    if (m_breaks.empty())
    {
        THROW("break is out of any loop", m_scanner.GetCurrentLine(), Lexeme{LexemeType::Break});
    }
    if (const auto lex = GetLexeme(); lex.type != LexemeType::Semicolon)
    {
        THROW("';' expected", m_scanner.GetCurrentLine(), lex);
    }
    m_breaks.top().push_back(m_poliz.AddGoto());
}

void Parser::AnalizeExpressionOperator()
{
    AnalizeExpression();
    if (const auto lex = GetLexeme(); lex.type != LexemeType::Semicolon)
    {
        THROW("';' expected", m_scanner.GetCurrentLine(), lex);
    }
    m_poliz.AddLexeme({LexemeType::Clear, {}});
}

void Parser::AnalizeExpression()
{
    AnalizeExpressionFirstLevel();

    auto lex = GetLexeme();
    while (IsOperatorFirstLevel(lex.type))
    {
        AnalizeExpressionFirstLevel();
        m_poliz.AddLexeme(lex);

        lex = GetLexeme();
    }
    SaveLexeme(std::move(lex));
}

void Parser::AnalizeExpressionFirstLevel()
{
    AnalizeExpressionSecondLevel();

    auto lex = GetLexeme();
    while (IsOperatorSecondLevel(lex.type))
    {
        AnalizeExpressionSecondLevel();
        m_poliz.AddLexeme(lex);

        lex = GetLexeme();
    }
    SaveLexeme(std::move(lex));
}

void Parser::AnalizeExpressionSecondLevel()
{
    AnalizeExpressionThirdLevel();

    auto lex = GetLexeme();
    while (IsOperatorThirdLevel(lex.type))
    {
        AnalizeExpressionThirdLevel();
        m_poliz.AddLexeme(lex);

        lex = GetLexeme();
    }
    SaveLexeme(std::move(lex));
}

void Parser::AnalizeExpressionThirdLevel()
{
    std::optional<Lexeme> saved;

    auto lex = GetLexeme();
    if (lex.type == LexemeType::Not)
    {
        saved = lex;
        lex = GetLexeme();
    }
    else if (lex.type == LexemeType::Minus)
    {
        saved = {LexemeType::UnaryMinus, {}};
        lex = GetLexeme();
    }
    else if (lex.type == LexemeType::Plus)
    {
        saved = {LexemeType::UnaryPlus, {}};
        lex = GetLexeme();
    }
    if (lex.type == LexemeType::Identifier)
    {
        if (!m_poliz.HasIdentifier(std::get<std::string>(lex.value)))
        {
            THROW("unknown identifier", m_scanner.GetCurrentLine(), lex);
        }
        m_poliz.AddLexeme(lex);

        auto next = GetLexeme();
        if (!saved && next.type == LexemeType::Assign)
        {
            saved = next;
            AnalizeExpression();
        }
        else
        {
            SaveLexeme(std::move(next));
        }
    }
    else if (lex.type == LexemeType::LeftParenthesis)
    {
        AnalizeExpression();
        if (const auto lex = GetLexeme(); lex.type != LexemeType::RightParenthesis)
        {
            THROW("')' expected", m_scanner.GetCurrentLine(), lex);
        }
    }
    else if (lex.type == LexemeType::Literal)
    {
        m_poliz.AddLexeme(lex);
    }
    else
    {
        THROW("unexpected lexeme", m_scanner.GetCurrentLine(), lex);
    }

    if (saved)
    {
        m_poliz.AddLexeme(*saved);
    }
}


syntax_exception::syntax_exception(const char* msg, int line, Lexeme lexeme)
    : std::runtime_error{msg}
    , m_line{line}
    , m_lexeme{lexeme}
{
}

std::string syntax_exception::DebugInfo() const
{
    std::stringstream ss;
    ss << "line " << m_line << ", lexeme " << m_lexeme;
    return ss.str();
}
