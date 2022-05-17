#include "Parser.hpp"

Parser::Parser(void) : _filePath("./configs/example_1.conf") { return; }

Parser::Parser(const Parser &instance) : _filePath(instance._filePath), _lexer(instance._lexer), _servers(instance._servers) { return; }

Parser  &Parser::operator=(const Parser &rhs) {
    if (this != &rhs) {
        this->_filePath = rhs._filePath;
        this->_lexer = rhs._lexer;
        this->_servers = rhs._servers;
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

void    Parser::_throwError(const std::string &msg) {
    std::ostringstream errMsg;
    errMsg << msg;
    throw std::runtime_error(errMsg.str());
}

std::pair<std::string, std::string>  Parser::_parseErrorPage(size_t *tokenPos, const std::vector<TokenPair> &tokens) {
    if (!this->_isNumber(tokens[*tokenPos].first) || std::atoi(tokens[*tokenPos].first.c_str()) > 600
        || std::atoi(tokens[*tokenPos].first.c_str()) < 100) {
        this->_throwError("Parser error\nstatus code for error_page should be a valid number and in range 100 <= x < 600");
    }
    std::string statusCode = tokens[*tokenPos].first.c_str();
    ++(*tokenPos);
    std::string filePath = tokens[*tokenPos].first;
    return std::pair<std::string, std::string>(statusCode, filePath);
}

LocationCfg    Parser::_parseLocation(size_t *tokenPos, const std::vector<TokenPair> &tokens) {
    LocationCfg location = LocationCfg();

    if (tokens[*tokenPos].first[0] == '/') {
        location.setPath(tokens[*tokenPos].first);
        ++(*tokenPos);
    }
    else {
        this->_throwError("Parser error\nLocation path should start with /");
    }
    if (tokens[*tokenPos].second != OPEN_BRACKET) {
        this->_throwError("Parser error\nBlock directive must be followed by {");
    }
    else
        ++(*tokenPos);
    while (tokens[*tokenPos].second != CLOSE_BRACKET) {
        switch (tokens[*tokenPos].second)
        {
            case WORD:
                if (tokens[*tokenPos].first == "method") {
                    ++(*tokenPos);
                    while (tokens[*tokenPos].second != SEMICOLON) {
                        location.addMethod(tokens[*tokenPos].first);
                        ++(*tokenPos);
                    }
                }
                else if (tokens[*tokenPos].first == "autoindex") {
                    ++(*tokenPos);
                    bool    v = false;
                    if (tokens[*tokenPos].first == "on")
                        v = true;
                    else if (tokens[*tokenPos].first == "off")
                        v = false;
                    else {
                        this->_throwError("Parser error\nautoindex is either 'on' or 'off'");
                    }
                    location.setAutoIndex(v);
                }
                else if (tokens[*tokenPos].first == "client_body_buffer_size") {
                    ++(*tokenPos);
                    if (!this->_isNumber(tokens[*tokenPos].first)) {
                        this->_throwError("Parser error\nclient_body_buffer_size should be a valid positive number");
                    }
                    location.setClientBodyBufferSize(std::atol(tokens[*tokenPos].first.c_str()));
                }
                else if (tokens[*tokenPos].first == "root") {
                    ++(*tokenPos);
                    std::string rootStr = tokens[*tokenPos].first;
                    if (rootStr[rootStr.size() - 1] != '/') {
                        this->_throwError("Root must be a directory and end with /");
                    }
                    location.setRoot(rootStr);
                }
                else if (tokens[*tokenPos].first == "index") {
                    ++(*tokenPos);
                    location.setIndex(tokens[*tokenPos].first);
                }
                else if (tokens[*tokenPos].first == "upload_dir") {
                    ++(*tokenPos);
                    if (tokens[*tokenPos].first[tokens[*tokenPos].first.size() - 1] != '/') {
                        this->_throwError("Root must be a directory and end with /");
                    }
                    location.setUploadDir(tokens[*tokenPos].first);
                }
                else if (tokens[*tokenPos].first == "cgi_extension") {
                    ++(*tokenPos);
                    location.setCgiExtention(tokens[*tokenPos].first);
                }
                else if (tokens[*tokenPos].first == "cgi_path") {
                    ++(*tokenPos);
                    location.setCgiPath(tokens[*tokenPos].first);
                }
                else if (tokens[*tokenPos].first == "return") {
                    ++(*tokenPos);
                    if (!this->_isNumber(tokens[*tokenPos].first) || std::atoi(tokens[*tokenPos].first.c_str()) > 600
                        || std::atoi(tokens[*tokenPos].first.c_str()) < 100) {
                        this->_throwError("Parser error\nRedirection status should be a valid number");
                    }
                    location.setRedirectionCode(tokens[*tokenPos].first);
                    ++(*tokenPos);
                    location.setRedirectionUrl(tokens[*tokenPos].first);
                }
                break;
            case SEMICOLON:
                break;
            default:
                this->_throwError("Parser error\nInvalid token in block directive");
        }
        ++(*tokenPos);
    }
    return location;
}

void    Parser::_parseServer(size_t *tokenPos, const std::vector<TokenPair> &tokens) {
    if (tokens[*tokenPos].second != OPEN_BRACKET) {
        this->_throwError("Parser error\nBlock directive must be followed by {");
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
                    this->_throwError("Parser error\nListen port should be a valid number");
                }
                newServer.setPort(std::atoi(tokens[*tokenPos].first.c_str()));
            }
            else if (tokens[*tokenPos].first == "server_name") {
                ++(*tokenPos);
                newServer.setName(tokens[*tokenPos].first);
            }
            else if (tokens[*tokenPos].first == "root") {
                ++(*tokenPos);
                std::string rootStr = tokens[*tokenPos].first;
                if (rootStr[rootStr.size() - 1] != '/') {
                    this->_throwError("Root must be a directory and end with /");
                }
                newServer.setRoot(rootStr);
            }
            else if (tokens[*tokenPos].first == "error_page") {
                ++(*tokenPos);
                std::pair<std::string, std::string> errorPage = this->_parseErrorPage(tokenPos, tokens);
                newServer.addErrorPage(errorPage.first, errorPage.second);
            }
            else if (tokens[*tokenPos].first == "location") {
                ++(*tokenPos);
                LocationCfg location = this->_parseLocation(tokenPos, tokens);
                newServer.addLocation(location);
            }
            else {
                this->_throwError("Parser error\nUnknown token: '" + tokens[*tokenPos].first + "'" + " Available tokens: listen, server_name, location, root, error_page, client_body_buffer_size, autoindex");
            }
            break;
        case SEMICOLON:
            break;
        default:
            this->_throwError("Parser error\nInvalid token: '" + tokens[*tokenPos].first + "'" + " in block directive");
        }
        ++(*tokenPos);
    }
    std::cout << "---SERVER---\n" << newServer << std::endl;
    if (newServer.getLocations().empty())
        this->_throwError("Parser error\nAt least one location required");
    this->_addServer(newServer);
}

void    Parser::parse(const std::string &filePath) {
    this->_lexer.tokenize(filePath);
    std::vector< std::pair<std::string, TOKENS> > tokens = this->_lexer.getTokens();
    for(size_t i = 0; i < tokens.size(); i++) {
        if (tokens[i].first == "server")
            this->_parseServer(&(++i), tokens);
        else {
            this->_throwError("Parser error\nUnknown directive: '" + tokens[i].first + "'");
        }
    }
}

const   std::vector<ServerCfg>  Parser::getServers(void) const { return this->_servers;}

Parser::~Parser(void) { return; }