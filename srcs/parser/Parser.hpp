#pragma once
#include "Lexer.hpp"
#include "ServerCfg.hpp"
#include <fstream>
#include <sstream>
#include <iostream>

class Parser {
    private:
        std::string                 _filePath;
        std::vector<TOKENS>         _bracketStack;
        Lexer                       _lexer;

        std::vector<ServerCfg>      _servers;

        bool                        _isNumber(const std::string &s) const;
        bool                        _fileExists(const std::string &path) const;
        void                        _addServer(const ServerCfg &server);
        std::pair<int, std::string> _parseErrorPage(size_t *tokenPos, const std::vector<TokenPair> &tokens);
        LocationCfg                 _parseLocation(size_t *tokenPos, const std::vector<TokenPair> &tokens);
        void                        _parseServer(size_t *tokenPos, const std::vector<TokenPair> &tokens);
        void                        _throwError(const std::string &msg);

    public:
        Parser(void);
        Parser(const Parser &instance);

        Parser  &operator=(const Parser &rhs);

        ~Parser(void);
        const   std::vector<ServerCfg>  getServers(void) const;
        void    parse(const std::string &filePath);
};