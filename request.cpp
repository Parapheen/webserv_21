#include "request.hpp"

Request::Request(): _headers(std::map<std::string, std::string>()) {};

Request::Request(Request const& copy): _method(copy._method), _uri(copy._uri), _version(copy._version), _headers(copy._headers), _body(copy._body) {};

Request& Request::operator=(Request const& source)
{
    if (this != &source)
    {
        _method = source._method;
        _uri = source._uri;
        _version = source._version;
        _headers = source._headers;
        _body = source._body;
    }
    return *this;
};

Request::~Request() {};

std::string Request::getMethod() const
{
    return _method;
};

std::string Request::getUri() const
{
    return _uri;
};

std::string Request::getVersion() const
{
    return _version;
};

std::string Request::getBody() const
{
    return _body;
};
std::map<std::string, std::string> Request::getHeaders() const
{
    return _headers;
};

void Request::setMethod(std::string method)
{
    _method = method;
};

void Request::setUri(std::string uri)
{
    _uri = uri;
};

void Request::setVersion(std::string version)
{
    _version = version;
};

void Request::setBody(std::string body)
{
    _body = body;
};

void Request::setHeaders(std::map<std::string, std::string> headers)
{
    _headers = headers;
};

bool Request::getHeaders(std::string message)
{
    //add checking if the header is already exist, then just?? add value to saved value
    std::string newline = message;
    std::string header = "";
    std::string value = "";
    size_t pos = message.find("\r\n");
    while (pos != std::string::npos)
    {
        newline = message.substr(0, pos);
        header = newline.substr(0, newline.find_first_of(":"));
        if (header.find(" ") != std::string::npos)
            return 1;
        //value = deleteSpaces(newline.substr(newline.find_first_of(":") + 1));
        value = newline.substr(newline.find_first_of(":") + 1);
        _headers[header] = value;
        message = message.substr(pos + 2);
        pos = message.find("\r\n");
    }
    return 0;
}

Response Request::parse(std::string message)
{
    size_t pos1, pos2;

    if (message.find("\r\n") == std::string::npos)
        return Response("400");
    std::string firstLine = message.substr(0, message.find("\r\n"));
    if ((pos1 = firstLine.find_first_of(" ")) == std::string::npos)
        return Response("400");
    _method = firstLine.substr(0, pos1);
    if ((pos2 = firstLine.find_last_of(" ")) == std::string::npos)
        return Response("400");
    _version = firstLine.substr(pos2 + 1);
    _uri = firstLine.substr(pos1 + 1, pos2 - 1 - pos1);
    if (_version[0] == ' ' || _uri[0] == ' ')
        return Response("400");


    // The presence of a message body in a request is signaled by a Content-Length or Transfer-Encoding header field !!!
    if (message.find("\r\n\r\n") == std::string::npos) // ?? if no \r\n\r\n then there is no headers and shoul be \r\n
    {
        if (message.find("\r\n") == std::string::npos)
            return Response("400");
        else
            _body = message.substr(message.find("\r\n") + 2);
    }
    else
    {
        if (getHeaders(message.substr(message.find("\r\n") + 2, message.find("\r\n\r\n") - message.find("\r\n"))))
            return Response("400");
        _body = message.substr(message.find("\r\n\r\n") + 4);
    }
    return execute();
};

Response Request::execute()
{
    Response resp;
    std::string methods[3] = {"GET", "POST", "DELETE"};
    // printRequest(req);
    if (_version != "HTTP/1.1")
        return Response("505");
    if (_method == methods[0])
        resp = execGet();
    else if (_method == methods[1])
        resp = execPost();
    else if (_method == methods[2])
        resp = execDelete();
    else
        return Response("405");
    return resp;
};

void Request::parseUri()
{
    if (_uri.find("?") == std::string::npos)
        return ;
    std::string findParams = _uri.substr(_uri.find("?") + 1);
    _uri =  _uri.substr(0, _uri.find("?"));
    std::map<std::string, std::string> params;
    std::string tmp;
    while (findParams.find("&") != std::string::npos)
    {
        tmp = findParams.substr(0, findParams.find("&"));
        findParams = findParams.substr(findParams.find("&") + 1);
        params[tmp.substr(0, tmp.find("="))] = tmp.substr(tmp.find("=") + 1);
    }
    if (!findParams.empty())
        params[findParams.substr(0, findParams.find("="))] = findParams.substr(findParams.find("=") + 1);

    // std::map<std::string, std::string>::iterator it;
    // for (it = params.begin(); it != params.end(); it++)
    //     std::cout << "key: " << it->first << " value: " << it->second << std::endl;
};

Response Request::execGet()
{
    //std::cout << "exec get\n";
    std::fstream fs;
    Response resp;
    std::string strh;
    std::string my_str;
    //std::cout << "uri: |" << req._uri << "|\n";
    
    //you need to parse uri because uri include params
    
    parseUri();
    if (_uri == "/" || _uri == "index.html")
        _uri = "index.html";
    fs.open(_uri);
    if (!fs.good())
        return Response("500"); 
    while(!fs.eof())
    {
        std::getline(fs, strh);
        my_str += strh;
        if (!fs.eof())
            my_str += '\n';
    }
    _body = my_str;
    fs.close();
    resp.setHeaders();    
    return resp;
};

Response Request::execPost()
{
    std::ifstream ifs (_uri);
    // check uri is CGI?? need to get PATH_INFO from envp
    // if (!isCGI(_uri))
    // {
    //     if (!ifs.is_open())
    //     {
    //         std::ofstream ofs;
        
    //         ofs.open(_uri.substr(1));
    //         ofs << _body;
    //         ofs.close();
    //     }


    // }

    return Response("200");

};

Response Request::execDelete()
{
    std::ifstream ifs (_uri);
    struct stat buffer;
    if (!stat(_uri.c_str(), &buffer))
    {
        if (buffer.st_mode & S_IFDIR)
            return Response("404");
        if (!(buffer.st_mode & S_IRUSR))
            return Response("403");
        if (!remove(_uri.c_str())) // if file removed 0 will be returned
            return Response("403");
    }
    else
        return Response("404");
    return Response("200"); // return 204 if response without body
};

void Request::printRequest(Request req)
{
    (void)req;
};