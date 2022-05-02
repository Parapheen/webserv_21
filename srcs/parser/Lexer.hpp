#pragma once
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

enum TOKENS {
    OPEN_BRACKET,
    CLOSE_BRACKET,
    HASHTAG,
    WORD,
};

typedef std::pair<std::string, TOKENS> TokenPair;

class Lexer {
    private:
        std::vector<TokenPair>   _tokens;

        std::string              _trim(const std::string &s) const;
        size_t                   _split(const std::string &s, std::vector<std::string> &out, char ch) const;

    public:
        Lexer(void);
        Lexer(const Lexer &instance);

        Lexer  &operator=(const Lexer &rhs);

        std::vector<TokenPair>          &getTokens(void);

        void                            handleLine(const std::string &line);
        void                            tokenize(const std::string &filePath);

        ~Lexer(void);
};