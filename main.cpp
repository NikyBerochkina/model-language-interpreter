#include <iostream>
#include <fstream>
#include "lexical2.h"
#include "syntax2.h"
#include "poliz2.h"

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

        Scanner scanner(*input);
        Poliz poliz;

        Parser parser(scanner, poliz);
        parser.Analize();

        auto interpreter = poliz.CreateInterpreter();
        interpreter.Run();
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
