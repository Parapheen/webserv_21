#include "Response.hpp"

Response::Response(void):
    _version("HTTP/1.1"), _statusCode("200"), _headers(std::map<std::string, std::string>()), _body("")
{
    initHeaders();
    initStatusMessages();
    //setHeaders(); // ???? uri is need, but what is uri for nothing?? maybe there no default constructor??
};

Response::Response(Response const& copy):
    _version(copy._version), _statusCode(copy._statusCode), _headers(copy._headers), _body(copy._body), _statusMessages(copy._statusMessages), _errorPages(copy._errorPages)
{
    initHeaders();
    initStatusMessages();
};

Response::Response(std::string const &statusCode, const std::string &uri, const std::map<std::string, std::string> &errorPages)
{
    initHeaders();
    initStatusMessages();
    _version = "HTTP/1.1";
    _statusCode = statusCode;
    _errorPages = errorPages;
    setHeaders(uri);
};

Response& Response::operator=(Response const& source)
{
    if (this != &source)
    {
        initHeaders();
        initStatusMessages();
        _version = source._version;
        _statusCode = source._statusCode;
        _headers = source._headers;
        _body = source._body;
        _statusMessages = source._statusMessages;
        _errorPages = source._errorPages;
    }
    return *this;
};

Response::~Response() {};

void Response::initStatusMessages()
{
    _statusMessages["100"] = "Continue";
    _statusMessages["200"] = "OK";
    _statusMessages["201"] = "Created";
    _statusMessages["301"] = "Moved Permanently";
    _statusMessages["302"] = "Found";
    _statusMessages["303"] = "See Other";
    _statusMessages["307"] = "Temporary Redirect";
    _statusMessages["308"] = "Permanent Redirect";
    _statusMessages["400"] = "Bad Request";
    _statusMessages["401"] = "Unauthorized"; // ??
    _statusMessages["402"] = "Payment Required"; // ??
    _statusMessages["403"] = "Forbidden";
    _statusMessages["404"] = "Not Found";
    _statusMessages["405"] = "Method Not Allowed";
    _statusMessages["413"] = "Payload Too Large";
    _statusMessages["500"] = "Internal Server Error";
    _statusMessages["505"] = "HTTP Version Not Supported";
};

void Response::initHeaders()
{
    _headers["Connection"] = "keep-alive";
    _headers["Server"] = "Webserv MacOS";
};

void Response::setHeaders(const std::string &uri)
{
    setDate();
    setContentType(uri);
    setContentLength();
    if (_statusCode[0] == '3')
        setLocation(uri);
    else
        setContentLocation(uri); // is it correct? 
    setLastModified(uri);
    setRetryAfter();
    // setTransferEncoding(); // ????
    // setWwwAuthenticate(); // ???? 
};

void Response::setDate()
{
    time_t seconds = time(NULL);
    tm* timeinfo = localtime(&seconds);
    char buffer[100];
    strftime(buffer, 100, "%a, %d %b %Y %H:%M:%S GMT", timeinfo);
    _headers["Date"] = buffer;
};

void Response::setContentType(const std::string &file)
{
    size_t dot = file.find(".");
    std::string exp;
    if (dot != std::string::npos)
    {
        //switch
        exp = file.substr(dot + 1);
        if (exp == "html")
            _headers["Content-Type"] = "text/html";
        else if (exp == "css")
            _headers["Content-Type"] = "text/css";
        else if (exp == "csv")
            _headers["Content-Type"] = "text/csv";
        else if (exp == "xml")
            _headers["Content-Type"] = "text/xml";
        else if (exp == "jpg" || exp == "jpeg")
            _headers["Content-Type"] = "image/jpeg";
        else if (exp == "png")
            _headers["Content-Type"] = "image/png";
        else
            _headers["Content-Type"] = "text/plain";
    }
};

void Response::setContentLength()
{
    std::stringstream oss;
    
    if (_body.size())
    {
        oss << _body.size();
        _headers["Content-Lenght"] = oss.str();
    }
};

void Response::setContentLocation(std::string const& uri){ _headers["Content-Location"] = uri; };

void Response::setContentLanguage(std::string const& lang){ _headers["Content-Language"] = lang; };

void Response::setLastModified(const std::string &file)
{
    struct stat buffer;
    char str[100];
    struct tm * timeinfo;
    if (!stat(file.c_str(), &buffer))
    {
        timeinfo = localtime(&buffer.st_ctime);
        strftime(str, 100, "%a, %d %b %Y %H:%M:%S GMT", timeinfo);
        _headers["Last-Modified"] = str;
    }
};

void Response::setLocation(const std::string &uri)
{
    if ((_statusCode == "201") || (_statusCode[0] == '3'))
        _headers["Location"] = uri;
};

void Response::setRetryAfter(void)
{
    if ((_statusCode == "503") || (_statusCode == "429") || (_statusCode == "301"))
        _headers["Retry-After"] = "10";
};

// void Response::setTransferEncoding()
// {

// };

void Response::setWwwAuthenticate(void)
{
    if (_statusCode == "401")
    {
        _headers["WWW-Authenticate"] = "Basic realm=\"Assecc to the staging site\", charset=\"UTF-8\"";
    }
};

std::string Response::getResponse(void)
{
    std::string response = "";
    response += (_version + " " + _statusCode + " " + _statusMessages[_statusCode] + "\r\n");
    for (std::map<std::string, std::string>::iterator it = _headers.begin(); it != _headers.end(); it++)
        response += (it->first + ": " + it->second + "\r\n");
    
    if (this->_hasDefaultErrorPage(this->_statusCode)) {
        std::ifstream fileStream;
        std::string fileContent;

        fileStream.open(_errorPages.find(_statusCode)->second);

        while(getline(fileStream, fileContent))
            _body += fileContent;
        _body += '\n';
        fileStream.close();
        response += ("\r\n" + _body + "\r\n");
    }
    else if (this->_hasDefaultErrorPage(this->_statusCode)) {

    }
    // All 1xx (Informational), 204 (No Content), and 304 (Not Modified) responses do not include a message body
    else if (!(_statusCode == "204" || _statusCode == "304" || _statusCode[0] == '1'))
        response += ("\r\n" + _body + "\r\n");
    return response;

};

bool    Response::_hasDefaultErrorPage(const std::string &statusCode) {
    std::map<std::string, std::string>::iterator it = this->_errorPages.begin();
    while (it != this->_errorPages.end()) {
        if (it->first == statusCode)
            return true;
    }
    return false;
}

void Response::setBody(const std::string &body) { this->_body = body; };

std::ostream& operator<<(std::ostream &out, Response &response)
{
    out << "Response:\n" << response.getResponse();
    return out;
};