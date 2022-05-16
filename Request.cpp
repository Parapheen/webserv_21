#include "Request.hpp"

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
const std::string                           &Request::getPath(void) const { return this->_path; };
const std::string                           &Request::getAbsPath(void) const { return this->_absPath; };
const std::string                           &Request::getVersion(void) const { return this->_version; };
const std::string                           &Request::getBody(void) const { return this->_body; };
const std::map<std::string, std::string>    &Request::getHeaders(void) const { return this->_headers; };
const ServerCfg                             &Request::getConfig() const { return this->_conf; };
const LocationCfg                           &Request::getCurrentLocation(void) const { return _currentLocation; };

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

std::string Request::parseFirstLine(std::string const& firstLine)
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
        return Response("400", _conf.getErrorPages(), "");
    if ((error = parseFirstLine(message.substr(0, message.find("\r\n")))) != "")
        return Response(error, _conf.getErrorPages(), _uri);
    if (message.find("\r\n\r\n") == std::string::npos)
    {
        if (message.find("\r\n") == std::string::npos)
            return Response("400", _conf.getErrorPages(), _uri);
        else
            getHeaders(message.substr(message.find("\r\n") + 2));
    }
    else
    {
        if (getHeaders(message.substr(message.find("\r\n") + 2, message.find("\r\n\r\n") - message.find("\r\n"))))
            return Response("400", _conf.getErrorPages(), _uri);
        _body = message.substr(message.find("\r\n\r\n") + 4);
    }
    return execute();
};

bool Request::findMethod(int method)
{
    std::vector<METHOD> methods = _currentLocation.getMethods();

    if (std::find(methods.begin(), methods.end(), method) != methods.end())
        return true;
    return false;
};

Response Request::execute()
{
    Response resp;

    _currentLocation = chooseLocation();

    if (_currentLocation.getRedirectionCode() != "")
        return Response(_currentLocation.getRedirectionCode(), _conf.getErrorPages(), _currentLocation.getRedirectionUrl());
    if (_method != "GET" && _body.size() > 1 && _currentLocation.getClientBodyBufferSize() > 0
        && _currentLocation.getClientBodyBufferSize() != static_cast<long long>(_body.size())) // trash in body, why?? and body.size() != 0
        return Response("413", _conf.getErrorPages(), _path);

    if (_method == "GET" && findMethod(GET))
        resp = execGet();
    else if (_method == "POST" && findMethod(POST))
        resp = execPost();
    else if (_method == "DELETE" && findMethod(DELETE))
        resp = execDelete();
    else
        return Response("405", _conf.getErrorPages(), _path);
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
    
    _host = deleteSpace(hostPost.substr(0, hostPost.find(":")));
    if (hostPost.find(":") == std::string::npos)
        _port = "80";
    else
        _port = deleteSpace(hostPost.substr(hostPost.find(":") + 1));
};

LocationCfg Request::chooseLocation(void)
{
    LocationCfg location = *(_conf.getLocations().begin());
    size_t maxLen = 0;
    std::string path = "";
    char cwd[2048];
    
    for (std::vector<LocationCfg>::const_iterator it = _conf.getLocations().begin(); it != _conf.getLocations().end(); it++)
    {
        path = "";
        std::string sub = _uri.substr(0, it->getPath().size());
        if (_uri.substr(0, it->getPath().size()) == it->getPath())
        {
            if (it->getRoot()[it->getRoot().size() - 1] == '/' && _uri[0] == '/')
                _path = it->getRoot() + _uri.substr(1);
            else
                _path = it->getRoot() + _uri;
        }
        if (_path.size() > maxLen)
        {
            maxLen = _path.size();
            location = *it;
        }
    }
    
    getcwd(cwd, 2048);
    _absPath += cwd;
    _absPath += ("/" + _path);
    std::cout << "uri: " << _uri << " path: " << _path << " abs path: " << _absPath << std::endl;
    return location;
};

std::string Request::createHtmlPage()
{
    DIR *dir;
    struct dirent *epdf;
    std::string page = "<!DOCTYPE html>\
    <html>\
    <head>\
        <title>Index of " + _path + "</title>\
    </head>\
    <body>\
        <h1>Index of " + _path + "</h1>";


    dir = opendir(_absPath.c_str());
    if (!dir)
        return "";
    if (dir != NULL)
    {
        while ((epdf = readdir(dir)))
            page += "\t<p><a href=\"http://" + _host + ":" + _port + "/" +
            std::string(epdf->d_name) + "\">" + std::string(epdf->d_name) + "</a></p>\n";
    }
    page += "</body>\n</html>";
    closedir(dir);
    return page;
};

std::string Request::indexFile()
{
    std::fstream fs;
    std::string content = "";
    std::string newline;

    // std::vector<std::string> indexes = _currentLocation.getIndexes();
    // for (size_t i = 0; i < indexes.size(); i++)
    // {
    //     std::cout << "index file: " << indexes[i] << std::endl;
    //     std::fstream f(indexes[i]);
    //     if (f.good())
    //     {
    //         f.open(_uri);
    //         while(!fs.eof())
    //         {
    //             std::getline(fs, newline);
    //             content += newline;
    //             if (!fs.eof())
    //                 content += '\n';
    //         }
    //         f.close();
    //         _path += indexes[i];
    //         return content;
    //     }
    // }

    std::string index = _currentLocation.getIndex();
    std::fstream f(index);
    if (!f.good())
        return "";
    _path += _currentLocation.getIndex();
    _absPath += _currentLocation.getIndex();
    f.open(_path);
    while(!fs.eof())
    {
        std::getline(fs, newline);
        content += newline;
        if (!fs.eof())
            content += '\n';
    }
    f.close();
    return content;
};

std::string Request::autoindex()
{
    struct stat s;
    std::string content = "";
    std::string newline;
    std::fstream fs;

    if (_uri[_uri.size() - 1] == '/' || (stat(_path.c_str(), &s) == 0 && (s.st_mode & S_IFDIR)))
    {
        if (_currentLocation.getAutoIndex())
        {
            initPort();
            std::string page = createHtmlPage();
            if (page == "")
                return "500";
            return page;
        }
        else
        {
            content = indexFile();
            if (content == "")
                return "404";
        }
    }
    return "";
};

Response Request::execGet(void)
{
    std::fstream fs;
    Response resp;
    std::string res;
    std::string newline;
    std::string content;
    struct stat buffer;

    // return Response("500", _conf.getErrorPages(), _path);
    res = autoindex();
    if (res == "500")
        return Response("500", _conf.getErrorPages(), _path);
    else if (res == "404")
        return Response("404", _conf.getErrorPages(), _path);
    else if (res != "")
    {
        resp.setBody(res);
        return resp;
    }
    if (stat(_path.c_str(), &buffer) == -1)
        return Response("404", _conf.getErrorPages(), _path);
    fs.open(_path);
    if (!fs.good())
        return Response("500", _conf.getErrorPages(), _path);
    while(!fs.eof())
    {
        std::getline(fs, newline);
        content += newline;
        if (!fs.eof())
            content += '\n';
    }
    fs.close();
    resp.setBody(content);
    resp.setHeaders(_path);
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

    if ((point = _uri.find_last_of(".")) != std::string::npos)
    {
        std::map<std::string, std::string> params; // maybe it isn't nesseccary and put all body to cgi
        if (_uri.substr(point + 1) == _currentLocation.getCgiExtention())
        {
            CGI_handler cgi = CGI_handler();
            try
            {
                Response response = cgi.create_response(*this, _currentLocation); // remove _currentLocation
            }
            catch(const std::exception& e)
            {
                std::cerr << e.what() << '\n';
                return Response(e.what(), _conf.getErrorPages(), _path);
            }
        }
        else
            return Response("500" , _conf.getErrorPages(), _path); // ?????
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
    return Response("200", _conf.getErrorPages(), _path);
};

Response Request::execDelete(void)
{
    std::ifstream ifs (_uri);
    struct stat buffer;
    
    if (!stat(_uri.c_str(), &buffer))
    {
        if (buffer.st_mode & S_IFDIR)
            return Response("404", _conf.getErrorPages(), _absPath);
        if (!(buffer.st_mode & S_IRUSR))
            return Response("403", _conf.getErrorPages(), _path);
        if (!remove(_path.c_str())) // if file removed, 0 will be returned
            return Response("403", _conf.getErrorPages(), _path);
    }
    else
        return Response("404", _conf.getErrorPages(), _path);
    return Response("200", _conf.getErrorPages(), _path); // return 204, if response without body
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

std::ostream& operator<<(std::ostream &out, Request request)
{
    out << request.getRequest();
    return out;
};