#include "server/Webserver.hpp"
#include "parser/Parser.hpp"

int main(int argc, char **argv)
{
    Parser      parser = Parser();
    Webserver   webserv = Webserver();
    std::string configPath = "./configs/example_1.conf";

    if (argc != 2)
        configPath = argv[1];
    try {
        parser.parse(configPath);
        webserv.run(parser.getServers());
    }
    catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
    return (0);
}