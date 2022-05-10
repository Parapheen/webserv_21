#include "request.hpp"
#include "Autoindex.hpp"

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


ServerCfg Request::getConfig() const
{
    return _conf;
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

void Request::setConfig(ServerCfg const& config)
{
    _conf = config;
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

std::string deleteSpace(std::string str)
{
    size_t start = 0;
    size_t end = str.size() - 1;
    
    while (str[start++] == ' ');
    while (str[end++] == ' ');
    return str.substr(--start, end);
}

Response Request::parse(std::string message)
{
    size_t pos1, pos2;

    if (message.find("\r\n") == std::string::npos)
        return Response("400", _uri);
    std::string firstLine = message.substr(0, message.find("\r\n"));
    if ((pos1 = firstLine.find_first_of(" ")) == std::string::npos)
        return Response("400", _uri);
    _method = firstLine.substr(0, pos1);
    if ((pos2 = firstLine.find_last_of(" ")) == std::string::npos)
        return Response("400", _uri);
    _version = firstLine.substr(pos2 + 1);
    _uri = firstLine.substr(pos1 + 1, pos2 - 1 - pos1);
    if (_version[0] == ' ' || _uri[0] == ' ')
        return Response("400", _uri);
    if (_version != "HTTP/1.1")
        return Response("505", _uri);

    // The presence of a message body in a request is signaled by a Content-Length or Transfer-Encoding header field !!!
    if (message.find("\r\n\r\n") == std::string::npos) // ?? if no \r\n\r\n then there is no headers and shoul be \r\n
    {
        if (message.find("\r\n") == std::string::npos)
            return Response("400", _uri);
        else
            getHeaders(message.substr(message.find("\r\n") + 2));
            
    }
    else
    {
        if (getHeaders(message.substr(message.find("\r\n") + 2, message.find("\r\n\r\n") - message.find("\r\n"))))
            return Response("400", _uri);
        _body = message.substr(message.find("\r\n\r\n") + 4);
    }
    return execute();
};

Response Request::execute()
{
    Response resp;

    if (_method == "GET")
        resp = execGet();
    else if (_method == "POST")
        resp = execPost();
    else if (_method == "DELETE")
        resp = execDelete();
    else
        return Response("405", _uri);
    return resp;
};

// void Request::parseUri()
// {
//     if (_uri.find("?") == std::string::npos)
//         return ;
//     std::string findParams = _uri.substr(_uri.find("?") + 1);
//     _uri =  _uri.substr(0, _uri.find("?"));
//     std::map<std::string, std::string> params;
//     std::string tmp;
//     while (findParams.find("&") != std::string::npos)
//     {
//         tmp = findParams.substr(0, findParams.find("&"));
//         findParams = findParams.substr(findParams.find("&") + 1);
//         params[tmp.substr(0, tmp.find("="))] = tmp.substr(tmp.find("=") + 1);
//     }
//     if (!findParams.empty())
//         params[findParams.substr(0, findParams.find("="))] = findParams.substr(findParams.find("=") + 1);
// };

void Request::initPort()
{
    std::string host;
    std::string port;
    std::string hostPost = _headers.find("Host")->second;
    host = hostPost.substr(0, hostPost.find(":"));
    if (hostPost.find(":") == std::string::npos)
        port = "80";
    else
        port = hostPost.substr(hostPost.find(":") + 1);
};

LocationCfg Request::chooseLocation()
{
    LocationCfg location = *(_conf.getLocations().begin());
    size_t maxLen = 0;
    std::string path = "";
    std::string fullPath = "";
    for (std::vector<LocationCfg>::const_iterator it = _conf.getLocations().begin(); it != _conf.getLocations().end(); it++)
    {
        path = "";
        fullPath = "";
        std::string sub = _uri.substr(0, it->getPath().size());
        if (sub == it->getPath())
            fullPath = it->getPath() + it->getRoot() + _uri;
        if (fullPath.size() > maxLen)
        {
            _absPath = fullPath;
            maxLen = fullPath.size();
            location = *it;
        }
    }
    return location;
};

Response Request::execGet()
{
    std::fstream fs;
    Response resp;
    std::string strh;
    std::string my_str;
    std::string maxPath;

    //parseUri();
    if (_conf.getLocations().empty())
        return Response("500", _uri); //or another error??
    LocationCfg location = chooseLocation();
    if (location.getMethod() != GET)
        return Response("405", _uri);
    if (location.getRedirectionCode() != "")
        return Response(location.getRedirectionCode(), location.getRedirectionUrl());

    if (_uri[_uri.size() - 1] == '/')
    {
        if (location.getAutoIndex())
        {
            Autoindex autoindex;
            
            initPort();
            std::string page = autoindex.createPage(_uri, _host, _port);
            if (page == "")
                return Response("500", _uri); // what response code send ??
            _body = page;
        }
        else
        {
            std::vector<std::string> indexes = location.getIndexes();
            for (size_t i = 0; i < indexes.size(); i++)
            {
                std::fstream f(indexes[i]);
                // _uri += indexes[i]; to correct path
                if (f.good())
                {
                    f.open(_uri);
                    while(!fs.eof())
                    {
                        std::getline(fs, strh);
                        my_str += strh;
                        if (!fs.eof())
                            my_str += '\n';
                    }
                    _body = my_str;
                    f.close();
                    break ;
                }
            }
        }
    }

    fs.open(_uri);
    if (!fs.good())
        return Response("500", _uri); 
    while(!fs.eof())
    {
        std::getline(fs, strh);
        my_str += strh;
        if (!fs.eof())
            my_str += '\n';
    }
    _body = my_str;
    fs.close();
    resp.setHeaders(_uri);
    return resp;
};

std::string myToLower(std::string str)
{
    for (size_t i = 0; i < str.size(); i++)
        str[i] = std::tolower(str[i]);
    return str;
};

Response Request::execPost()
{
    size_t point;

    if (_conf.getLocations().empty())
        return Response("500", _uri); //or another error??
    
    LocationCfg location = chooseLocation();
    if (location.getMethod() != POST)
        return Response("405", _uri);
    if (location.getClientBodyBufferSize() > 0 && location.getClientBodyBufferSize() != static_cast<long long>(_body.size()))
        return Response("413", _uri);
    if (location.getRedirectionCode() != "")
        return Response(location.getRedirectionCode(), location.getRedirectionUrl());

    
    // Когда веб-браузер отправляет POST-запрос с элементами веб-формы, по умолчанию интернет-тип данных медиа — multipart/form-data, типы не чувствительны к регистру
    // if (_headers.find("Content-Type") != _headers.end() && _headers.find("Content-Type")->second == "multipart/form-data")
    // {
    //     // what should be happen?

    // }
    // else 
    if ((point = _uri.find_last_of(".")) != std::string::npos)
    {
        std::map<std::string, std::string> params; // maybe it isn't nesseccary and put all body to cgi
        if (_uri.substr(point + 1) == location.getCgiExpantion())
        {
            // Cgi cgi(location, _headers);
            // return Response(cgi.result());
        }
        else
            return Response("500" , _uri); // ?????
    }
    else
    {
        std::fstream fs;
        fs.open(_uri);
        if (!fs.is_open())
        {
            fs.clear();
            //check filename
            fs.open(_uri, std::ios::out);
            fs.close();
            fs.open(_uri);
        }
        fs << _body;
        fs.close();
    }
    return Response("200", _uri);
};

Response Request::execDelete()
{
    if (_conf.getLocations().empty())
        return Response("500", _uri); //or another error??
    LocationCfg location = chooseLocation();
    if (location.getMethod() != DELETE)
        return Response("405", _uri);
    if (location.getClientBodyBufferSize() > 0 && location.getClientBodyBufferSize() != static_cast<long long>(_body.size()))
        return Response("413", _uri);
    if (location.getRedirectionCode() != "")
        return Response(location.getRedirectionCode(), location.getRedirectionUrl());

    std::ifstream ifs (_uri);
    struct stat buffer;
    if (!stat(_uri.c_str(), &buffer))
    {
        if (buffer.st_mode & S_IFDIR)
            return Response("404", _uri);
        if (!(buffer.st_mode & S_IRUSR))
            return Response("403", _uri);
        if (!remove(_uri.c_str())) // if file removed, 0 will be returned
            return Response("403", _uri);
    }
    else
        return Response("404", _uri);
    return Response("200", _uri); // return 204, if response without body
};

std::string Request::getRequest()
{
    std::string request = "";
    request += (_method + " " + _uri + " " + _version + "\r\n");
    for (std::map<std::string, std::string>::iterator it = _headers.begin(); it != _headers.end(); it++)
        request += (it->first + ": " + it->second + "\r\n"); 
    if (_body != "")
        request += ("\r\n" + _body + "\r\n");
    return request;
};

std::ostream& operator<<(std::ostream &out, Request request)
{
    out << request.getRequest();
    return out;
};