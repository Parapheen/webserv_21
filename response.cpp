#include "response.hpp"

Response::Response():
    _version("HTTP/1.1"), _statusCode("200"), _headers(std::map<std::string, std::string>()), _body("") {};

Response::Response(Response const& copy):
    _version(copy._version), _statusCode(copy._statusCode), _headers(copy._headers), _body(copy._body) {};

Response::Response(std::string const statusCode)
{
    std::cout << "st. code: " << statusCode << std::endl;
    //create response depends on status code, maybe same response.error(statusCode) ?? 
};

Response& Response::operator=(Response const& source)
{
    if (this != &source)
    {
        _version = source._version;
        _statusCode = source._statusCode;
        _headers = source._headers;
        _body = source._body;
    }
    return *this;
};

Response::~Response() {};

void Response::initStatusMessages()
{
    _statusMessages["100"] = "Continue";
    _statusMessages["200"] = "OK";
    _statusMessages["201"] = "Created";
    _statusMessages["400"] = "Bad Request";
    _statusMessages["401"] = "Unauthorized";
    _statusMessages["402"] = "Payment Required";
    _statusMessages["403"] = "Forbidden";
    _statusMessages["404"] = "Not Found";
    _statusMessages["405"] = "Method Not Allowed";
    _statusMessages["505"] = "HTTP Version Not Supported";
};

void Response::initHeaders()
{
    _headers["Allow"] = "";
    //_headers["Host"] = "127.0.0.1";
    //_headers["Port"] = "80";
    _headers["Connection"] = "keep-alive";
    _headers["Date"] = "";
    _headers["Last-Modified"] = "";
    _headers["User-Agent"] = "Chrome/96.0.4664.55";
    _headers["Accept-Encoding"] = "";
    _headers["Accept-Language"] = "";
    _headers["Server"] = "Webserv jrandee && odomenic && nscarab (MacOS)";
    _headers["Content-Type"] = "";
    _headers["Content-Encoding"] = "";
    _headers["Content-Lenght"] = "";
    _headers["Content-Location"] = "";
    _headers["Content-Language"] = "";

    //control data 7.1 in RFC
    _headers["Location"] = "";
    _headers["Transfer-Encoding"] = ""; // chanked
    _headers["Retry-After"] = "";
    _headers["WWW-Authenticate"] = ""; // for what??

    // _headers["Very"] = ""; // this header is nesseccary??

};

void Response::setHeaders()
{
    setAllow();
    setDate();
    //setContentType();
    setContentLenght();
    // _response._headers["Last-Modified"] = "";
    // _response._headers["Accept-Encoding"] = "";
    // _response._headers["Accept-Language"] = "";
    // _response._headers["Content-Encoding"] = "";
    // _response._headers["Content-Location"] = "";
};

void Response::setDate()
{
    time_t seconds = time(NULL);
    tm* timeinfo = localtime(&seconds);
    char buffer[100];
    strftime(buffer, 100, "%a, %d %b %Y %H:%M:%S GMT", timeinfo);
    _headers["Date"] = buffer;
    //std::cout << "Date: " << _headers["Date"] << std::endl;
};

void Response::setContentType(std::string file)
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
            _headers["Content-Type"] = "text/plain"; //what default value set?
        //what else ??

        //should set the charset??
    }
};

void Response::setContentLenght()
{
    std::stringstream oss;
    std::map<std::string, std::string>::iterator find_it = _headers.find("Content-Length");

    if (find_it == _headers.end())
    {
        oss << _body.size();
        _headers["Content-Lenght"] = oss.str();
    }
    else
        find_it->second = _body.size();
};

void Response::setContentEncoding()
{};

void Response::setContentLocation()
{};

void Response::setContentLanguage()
{};

void Response::setAllow()
{
    // size_t i = 0;
    // std::string methods[3] = {"GET", "POST", "DELETE"};
    // for (i = 0; i < 2; i++)
    //     _headers["Allow"] += (methods[i] + ", ");
    // _headers["Allow"] += methods[i];
    _headers["Allow"] = "GET, POST, DELETE";
};

void Response::setLastModified(std::string file)
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

void Response::setAcceptLanguage()
{};

void Response::setAcceptEncoding()
{};

void Response::setLocation(std::string uri)
{
    if ((_statusCode == "201") || (_statusCode[0] == '3'))
        _headers["Location"] = uri;
    else
        return ; // remove header ??
};

void Response::setRetryAfter()
{
    if ((_statusCode == "503") || (_statusCode == "429") || (_statusCode == "301"))
        _headers["Retry-After"] = "10";
    else
        _headers["Retry-After"] = "0"; // or remove this header ??
};

void Response::setUserAgent()
{

};

void Response::setTransferEncoding()
{

};

void Response::setWWWAuthenticate()
{

};

void Response::error(std::string statusCode)
{
    (void)statusCode;
};

std::string Response::getResponse()
{
    std::string response = "";
    response += (_version + " " + _statusCode + " " + _statusMessages[_statusCode] + "\r\n");
    for (std::map<std::string, std::string>::iterator it = _headers.begin(); it != _headers.end(); it++)
        response += (it->first + ": " + it->second + "\r\n"); 
    
    // All 1xx (Informational), 204 (No Content), and 304 (Not Modified) responses do not include a message body !!!
    if (!(_statusCode == "204" || _statusCode == "304" || _statusCode[0] == '1'))
        response += ("\r\n" + _body + "\r\n");
    return response;

};