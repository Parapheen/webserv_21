#include "Request.hpp"
#include "Autoindex.hpp"

Request::Request(void): _headers(std::map<std::string, std::string>()) { return; };

Request::Request(Request const &copy): _method(copy._method), _uri(copy._uri), _version(copy._version), _headers(copy._headers), _body(copy._body) { return; };

Request& Request::operator=(Request const& source)
{
    if (this != &source)
    {
        this->_method = source._method;
        this->_uri = source._uri;
        this->_version = source._version;
        this->_headers = source._headers;
        this->_body = source._body;
    }
    return *this;
};

Request::~Request(void) { return; };

const std::string                           &Request::getMethod(void) const { return this->_method; };
const std::string                           &Request::getUri(void) const { return this->_uri; };
const std::string                           &Request::getVersion(void) const { return this->_version; };
const std::string                           &Request::getBody(void) const { return this->_body; };
const std::map<std::string, std::string>    &Request::getHeaders(void) const { return this->_headers; };
const ServerCfg                             &Request::getConfig() const { return this->_conf; };

void Request::setMethod(const std::string &method) { this->_method = method; };
void Request::setUri(const std::string &uri) { this->_uri = uri; };
void Request::setVersion(const std::string &version) { this->_version = version; };
void Request::setBody(const std::string &body) { this->_body = body; };
void Request::setHeaders(const std::map<std::string, std::string> &headers) { this->_headers = headers; };
void Request::setConfig(ServerCfg const& config) { this->_conf = config; };

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
            return true;
        //value = deleteSpaces(newline.substr(newline.find_first_of(":") + 1));
        value = newline.substr(newline.find_first_of(":") + 1);
        _headers[header] = value;
        message = message.substr(pos + 2);
        pos = message.find("\r\n");
    }
    return false;
}

std::string deleteSpace(std::string str)
{
    size_t start = 0;
    size_t end = str.size() - 1;
    
    while (str[start++] == ' ');
    while (str[end++] == ' ');
    return str.substr(--start, end);
}

std::string Request::parseFirstLine(std::string firstLine)
{
    size_t pos1, pos2;

    if ((pos1 = firstLine.find_first_of(" ")) == std::string::npos)
        return "400";
    _method = firstLine.substr(0, pos1);
    if ((pos2 = firstLine.find_last_of(" ")) == std::string::npos)
        return "400";
    _version = firstLine.substr(pos2 + 1);
    _uri = firstLine.substr(pos1 + 1, pos2 - 1 - pos1);
    if (_version[0] == ' ' || _uri[0] == ' ')
        return "400";
    if (_version != "HTTP/1.1")
        return "505";
    return "";
}

Response Request::parse(const std::string &message)
{
    std::string error = "";

    if (message.find("\r\n") == std::string::npos)
        return Response("400");
    if ((error = parseFirstLine(message.substr(0, message.find("\r\n")))) != "")
        return Response(error, _uri);
    if (message.find("\r\n\r\n") == std::string::npos)
    {
        if (message.find("\r\n") == std::string::npos)
            return Response("400", _uri);
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
    
    _currentLocation = chooseLocation();

    if (_currentLocation.getRedirectionCode() != "")
        return Response(_currentLocation.getRedirectionCode(), _currentLocation.getRedirectionUrl());
    if (_method == "GET" && _currentLocation.methodExists(GET))
        resp = execGet();
    else if (_method == "POST" && _currentLocation.methodExists(POST))
        resp = execPost();
    else if (_method == "DELETE" && _currentLocation.methodExists(DELETE))
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

void Request::initPort(void)
{
    std::string hostPost = _headers.find("Host")->second;
    _host = hostPost.substr(1, hostPost.find(":") - 1);
    if (hostPost.find(":") == std::string::npos)
        _port = "80";
    else
        _port = hostPost.substr(hostPost.find(":") + 1);
};

LocationCfg Request::chooseLocation(void)
{
    LocationCfg location = *(_conf.getLocations().begin());
    size_t maxLen = 0;
    for (std::vector<LocationCfg>::const_iterator it = _conf.getLocations().begin(); it != _conf.getLocations().end(); it++)
    {
        std::string sub = _uri.substr(0, it->getPath().size());
        if (sub == it->getPath()) {
            if (it->getPath() == "/" && _uri == "/")
                _absPath = "." + _uri;
            else
                _absPath = "./" + it->getRoot() + _uri;
        }
        if (_absPath.size() > maxLen)
        {
            maxLen = _absPath.size();
            location = *it;
        }
    }
    return location;
};

Response Request::execGet(void)
{
    std::ifstream   fileStream;
    Response resp;
    std::string strh;
    std::string fileContent;
    std::string maxPath;

    //parseUri();

    if (_uri[_uri.size() - 1] == '/' || _isDirectory(_absPath.c_str()))
    {
        if (_currentLocation.getAutoIndex())
        {
            Autoindex autoindex;
            
            std::string page = autoindex.createPage(_currentLocation.getRoot() + _uri, _uri);
            if (page == "")
                return Response("500", _uri);
            _body = page;
            resp.setHeaders(_uri);
            resp.setBody(_body);
            return resp;
        }
        else
        {
            std::string index = _currentLocation.getIndex();
            std::string targetFile = "./" + _currentLocation.getRoot() + index;
            fileStream.open(targetFile);
            if (!fileStream.is_open())
                return Response("404", _uri);
            while(getline(fileStream, strh))
                fileContent += strh;
            fileContent += '\n';
            _body = fileContent;
            fileStream.close();
            resp.setHeaders(_uri);
            resp.setBody(_body);
            return resp;
        }
    }

    fileStream.open(_absPath);
    if (!fileStream.is_open())
        return Response("404", _uri);
    while(getline(fileStream, strh))
        fileContent += strh;
    fileContent += '\n';
    _body = fileContent;
    fileStream.close();
    resp.setHeaders(_uri);
    resp.setBody(_body);
    return resp;
};

std::string myToLower(const std::string &str)
{
    std::string res;
    for (size_t i = 0; i < str.size(); i++)
        res[i] = std::tolower(str[i]);
    return res;
};

Response Request::execPost(void)
{
    size_t point;

    if (_currentLocation.getClientBodyBufferSize() > 0 && _currentLocation.getClientBodyBufferSize() != static_cast<long long>(_body.size()))
        return Response("413", _uri);

    
    // Когда веб-браузер отправляет POST-запрос с элементами веб-формы, по умолчанию интернет-тип данных медиа — multipart/form-data, типы не чувствительны к регистру
    if (_headers.find("Content-Type") != _headers.end() && _headers.find("Content-Type")->second == "multipart/form-data")
    {

    }
    else {

    }
    if ((point = _uri.find_last_of(".")) != std::string::npos)
    {
        std::map<std::string, std::string> params; // maybe it isn't nesseccary and put all body to cgi
        if (_uri.substr(point + 1) == _currentLocation.getCgiExtention())
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

Response Request::execDelete(void)
{
    if (_currentLocation.getClientBodyBufferSize() > 0 && _currentLocation.getClientBodyBufferSize() != static_cast<long long>(_body.size()))
        return Response("413", _uri);

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

std::string Request::getRequest(void)
{
    std::string request = "";
    request += (_method + " " + _uri + " " + _version + "\r\n");
    for (std::map<std::string, std::string>::iterator it = _headers.begin(); it != _headers.end(); it++)
        request += (it->first + ": " + it->second + "\r\n"); 
    if (_body != "")
        request += ("\r\n" + _body + "\r\n");
    return request;
};

bool Request::_isDirectory(const char *path) {
   struct stat statbuf;
   if (stat(path, &statbuf) == 0)
       return (statbuf.st_mode & S_IFDIR);
   return false;
}

std::ostream& operator<<(std::ostream &out, Request request)
{
    out << request.getRequest();
    return out;
};