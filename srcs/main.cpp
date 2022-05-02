#include "server/Webserver.hpp"
#include "parser/Parser.hpp"

int main(int argc, char **argv)
{
    Parser      parser = Parser();
    Webserver   webserv = Webserver();

    if (argc != 2) {
        std::cerr << "Invalid arguments" << std::endl;
        return (1);
    }
    try {
        parser.parse(argv[1]);
    }
    catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
    // webserv.run();
    return (0);
}