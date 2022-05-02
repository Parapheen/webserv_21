#include "Lexer.hpp"

Lexer::Lexer(void) { return; }

Lexer::Lexer(const Lexer &instance) {
    *this = instance;
}

Lexer &Lexer::operator=(const Lexer &rhs) {
    this->_tokens = rhs._tokens;
    return *this;
}

std::vector< std::pair<std::string, TOKENS> >   &Lexer::getTokens(void) {
    return this->_tokens;
}

std::string Lexer::_trim(const std::string &s) const {
    size_t first = s.find_first_not_of(' ');

    if (std::string::npos == first)
        return s;
    size_t last = s.find_last_not_of(' ');
    return s.substr(first, (last - first + 1));
}

size_t  Lexer::_split(const std::string &s, std::vector<std::string> &out, char ch) const {
    std::string trimmed = this->_trim(s);
    size_t pos = trimmed.find(ch);
    size_t initialPos = 0;

    while (pos != std::string::npos) {
        out.push_back(trimmed.substr(initialPos, pos - initialPos));
        initialPos = pos + 1;
        pos = trimmed.find(ch, initialPos);
    }
    out.push_back(trimmed.substr(initialPos, std::min(pos, trimmed.size()) - initialPos + 1));

    return out.size();
}

void    Lexer::handleLine(const std::string &line) {
    std::vector<std::string>    splitted;
    this->_split(line, splitted, ' ');
    std::vector<std::string>::iterator  it = splitted.begin();

    while (it != splitted.end()) {
        // std::cout << *it << " ";
        if (*it == "{")
            this->_tokens.push_back(std::pair<std::string, TOKENS>(*it, OPEN_BRACKET));
        else if (*it == "}")
            this->_tokens.push_back(std::pair<std::string, TOKENS>(*it, CLOSE_BRACKET));
        else if (*it == "#") // comment line
            break ;
        else
            this->_tokens.push_back(std::pair<std::string, TOKENS>(*it, WORD));
        it++;
    }
}

void    Lexer::tokenize(const std::string &filePath) {
    std::ifstream   fileStream;
    std::string     line;

    fileStream.open(filePath);
    if (!fileStream.is_open()) {
        std::ostringstream errMsg;
        errMsg << "Lexer error\nCannot open file: '" << filePath << "'";
        throw std::runtime_error(errMsg.str());
    }
    while (getline(fileStream, line))
        this->handleLine(line);
    return ;
}

Lexer::~Lexer(void) { return; }