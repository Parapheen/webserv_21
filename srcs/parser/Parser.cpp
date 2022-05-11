#include "Parser.hpp"

Parser::Parser(void) : _filePath("./configs/example_1.conf") { return; }

Parser::Parser(const Parser &instance) : _filePath(instance._filePath), _lexer(instance._lexer), _servers(instance._servers) { return; }

Parser  &Parser::operator=(const Parser &rhs) {
    if (this != &rhs) {
        this->_filePath = rhs._filePath;
        this->_lexer = rhs._lexer;
        for (size_t i = 0; i < rhs.getServers().size(); i++)
            this->_servers[i] = rhs.getServers()[i];
    }
    return *this;
}

bool Parser::_isNumber(const std::string &s) const {
    std::string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it)) ++it;
    return !s.empty() && it == s.end();
}

bool Parser::_fileExists(const std::string &path) const {
    std::ifstream f(path.c_str());
    return f.good();
}

void    Parser::_addServer(const ServerCfg &server) {
    this->_servers.push_back(server);
}

std::pair<int, std::string>  Parser::_parseErrorPage(size_t *tokenPos, const std::vector<TokenPair> &tokens) {
    if (!this->_isNumber(tokens[*tokenPos].first) || std::atoi(tokens[*tokenPos].first.c_str()) > 600
        || std::atoi(tokens[*tokenPos].first.c_str()) < 100) {
        std::ostringstream errMsg;
        errMsg << "Parser error\nstatus code for error_page should be a valid number and in range 100 <= x < 600";
        throw std::runtime_error(errMsg.str());
    }
    int statusCode = std::atoi(tokens[*tokenPos].first.c_str());
    ++(*tokenPos);
    std::string filePath = tokens[*tokenPos].first;
    return std::pair<int, std::string>(statusCode, filePath);
}

LocationCfg    Parser::_parseLocation(size_t *tokenPos, const std::vector<TokenPair> &tokens) {
    LocationCfg location = LocationCfg();

    if (tokens[*tokenPos].first[0] == '/') {
        location.setPath(tokens[*tokenPos].first);
        ++(*tokenPos);
    }
    else {
        std::ostringstream errMsg;
        errMsg << "Parser error\nLocation path should start with /";
        throw std::runtime_error(errMsg.str());
    }
    if (tokens[*tokenPos].second != OPEN_BRACKET) {
        std::ostringstream errMsg;
        errMsg << "Parser error\nBlock directive must be followed by {";
        throw std::runtime_error(errMsg.str());
    }
    else
        ++(*tokenPos);
    while (tokens[*tokenPos].second != CLOSE_BRACKET) {
        switch (tokens[*tokenPos].second)
        {
            case WORD:
                if (tokens[*tokenPos].first == "method") {
                    ++(*tokenPos);
                    location.setMethod(tokens[*tokenPos].first);
                }
                else if (tokens[*tokenPos].first == "autoindex") {
                    ++(*tokenPos);
                    bool    v;
                    if (tokens[*tokenPos].first == "on")
                        v = true;
                    else if (tokens[*tokenPos].first == "off")
                        v = false;
                    else {
                        std::ostringstream errMsg;
                        errMsg << "Parser error\nautoindex is either 'on' or 'off'";
                        throw std::runtime_error(errMsg.str());
                    }
                    location.setAutoIndex(v);
                }
                else if (tokens[*tokenPos].first == "client_body_buffer_size") {
                    ++(*tokenPos);
                    if (!this->_isNumber(tokens[*tokenPos].first)) {
                        std::ostringstream errMsg;
                        errMsg << "Parser error\nclient_body_buffer_size should be a valid positive number";
                        throw std::runtime_error(errMsg.str());
                    }
                    location.setClientBodyBufferSize(std::atol(tokens[*tokenPos].first.c_str()));
                }
                else if (tokens[*tokenPos].first == "root") {
                    ++(*tokenPos);
                    location.setRoot(tokens[*tokenPos].first);
                }
                break;
            default:
                std::ostringstream errMsg;
                errMsg << "Parser error\nInvalid token in block directive";
                throw std::runtime_error(errMsg.str());
        }
        ++(*tokenPos);
    }
    return location;
}

void    Parser::_parseServer(size_t *tokenPos, const std::vector<TokenPair> &tokens) {
    if (tokens[*tokenPos].second != OPEN_BRACKET) {
        std::ostringstream errMsg;
        errMsg << "Parser error\nBlock directive must be followed by {";
        throw std::runtime_error(errMsg.str());
    }
    else
        ++(*tokenPos);

    ServerCfg newServer = ServerCfg();
    while (tokens[*tokenPos].second != CLOSE_BRACKET) {
        switch (tokens[*tokenPos].second)
        {
        case WORD:
            if (tokens[*tokenPos].first == "listen") {
                ++(*tokenPos);
                if (!this->_isNumber(tokens[*tokenPos].first)) {
                    std::ostringstream errMsg;
                    errMsg << "Parser error\nListen port should be a valid number";
                    throw std::runtime_error(errMsg.str());
                }
                newServer.setPort(std::atoi(tokens[*tokenPos].first.c_str()));
            }
            else if (tokens[*tokenPos].first == "server_name") {
                ++(*tokenPos);
                newServer.setName(tokens[*tokenPos].first);
            }
            else if (tokens[*tokenPos].first == "root") {
                ++(*tokenPos);
                newServer.setRoot(tokens[*tokenPos].first);
            }
            else if (tokens[*tokenPos].first == "error_page") {
                ++(*tokenPos);
                std::pair<int, std::string> errorPage = this->_parseErrorPage(tokenPos, tokens);
                newServer.addErrorPage(errorPage.first, errorPage.second);
            }
            else if (tokens[*tokenPos].first == "location") {
                ++(*tokenPos);
                LocationCfg location = this->_parseLocation(tokenPos, tokens);
                newServer.addLocation(location);
            }
            else {
                std::ostringstream errMsg;
                errMsg << "Parser error\nUnknown token: '" << tokens[*tokenPos].first << "'" << " Available tokens: listen, server_name, location, root, error_page, client_body_buffer_size, autoindex";
                throw std::runtime_error(errMsg.str());
            }
            break;
        default:
            std::ostringstream errMsg;
            errMsg << "Parser error\nInvalid token: '" << tokens[*tokenPos].first << "'" << " in block directive";
            throw std::runtime_error(errMsg.str());
        }
        ++(*tokenPos);
    }
    std::cout << "---SERVER---\n" << newServer << std::endl;
    this->_addServer(newServer);
}

void    Parser::parse(const std::string &filePath) {
    this->_lexer.tokenize(filePath);
    std::vector< std::pair<std::string, TOKENS> > tokens = this->_lexer.getTokens();
    for(size_t i = 0; i < tokens.size(); i++) {
        if (tokens[i].first == "server")
            this->_parseServer(&(++i), tokens);
        else {
            std::ostringstream errMsg;
            errMsg << "Parser error\nUnknown directive: '" << tokens[i].first << "'";
            throw std::runtime_error(errMsg.str());
        }
    }
}

const   std::vector<ServerCfg>  Parser::getServers(void) const { return this->_servers;}

Parser::~Parser(void) { return; }