#include <iostream>
#include <fstream>
#include "lexical2.h"
#include "syntax2.h"
#include "poliz2.h"

#ifndef DEBUG_INTERPRETER
# define DEBUG_INTERPRETER 0
#endif

void PrintLexemas(std::istream& is)
{
    Scanner scanner(is);

    Lexeme lex;
    while ((lex = scanner.GetLexeme()).type != LexemeType::Eof)
    {
        std::cout << lex << std::endl;
    }
}

void PrintPoliz(std::istream& is)
{
    Scanner scanner(is);

    Poliz poliz;
    Parser parser(scanner, poliz);
    parser.Analize();

    const auto& program = poliz.GetProgram();
    for (size_t i = 0; i < program.size(); ++i)
    {
        std::cout << "i: " << i << ", " << program[i] << std::endl;
    }
}

void ExecuteProgram(std::istream& is)
{
    Scanner scanner(is);

    Poliz poliz;
    Parser parser(scanner, poliz);
    parser.Analize();

    auto interpreter = poliz.CreateInterpreter();
    interpreter.Run(DEBUG_INTERPRETER);
}

int main(int argc, char** argv)
{
    try
    {
        std::fstream f;
        std::istream* input{};
        if (argc > 1)
        {
            f.open(argv[1]);
            input = &f;
        }
        else
        {
            input = &std::cin;
        }
#if defined (LEXICAL)
        PrintLexemas(*input);
#elif defined (POLIZ)
        PrintPoliz(*input);
#else
        ExecuteProgram(*input);
#endif
    }
    catch (lexical_exception& e)
    {
        std::cerr << "lexical error: " << e.what() << std::endl;
        std::cerr << "debug info: " << e.DebugInfo() << std::endl;
        return EXIT_FAILURE;
    }
    catch (syntax_exception& e)
    {
        std::cerr << "syntax error: " << e.what() << std::endl;
        std::cerr << "debug info: " << e.DebugInfo() << std::endl;
        return EXIT_FAILURE;
    }
    catch (std::exception& e)
    {
        std::cerr << "error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
