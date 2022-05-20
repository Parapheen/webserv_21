#include "Request.hpp"
#include "../cgi/CgiHandler.hpp"
#define BOUNDARY_PREFIX_LEN 2

Request::Request(void): _method("GET"), _headers(std::map<std::string, std::string>()), _status(EMPTY) { return; };

Request::Request(Request const &copy): _rawRequest(copy._rawRequest), _method(copy._method), _uri(copy._uri), _path(copy._path),
    _absPath(copy._absPath), _version(copy._version), _headers(copy._headers), _body(copy._body),
    _status(copy._status), _parsingFirstLine(copy._parsingFirstLine), _parsingHeaders(copy._parsingHeaders),
    _parsingBody(copy._parsingBody) { return; };

Request& Request::operator=(Request const& source) {
    if (this != &source)
    {
        this->_rawRequest = source._rawRequest;
        this->_method = source._method;
        this->_uri = source._uri;
        this->_path = source._path;
        this->_absPath = source._absPath;
        this->_version = source._version;
        this->_headers = source._headers;
        this->_body = source._body;
        this->_status = source._status;
        this->_parsingFirstLine = source._parsingFirstLine;
        this->_parsingHeaders = source._parsingHeaders;
        this->_parsingBody = source._parsingBody;
    }
    return *this;
};

Request::~Request(void) { return; };

const std::string                           &Request::getMethod(void) const { return this->_method; };
const std::string                           &Request::getUri(void) const { return this->_uri; };
const std::string                           &Request::getVersion(void) const { return this->_version; };
const std::string                           &Request::getBody(void) const { return this->_body; };
const std::string                           &Request::getAbsPath(void) const { return this->_absPath; };
const std::string                           &Request::getRoot(const LocationCfg &location) const {
    if (location.getRoot() != "")
        return location.getRoot();
    return this->_conf.getRoot();
};
const std::map<std::string, std::string>    &Request::getHeaders(void) const { return this->_headers; };
const ServerCfg                             &Request::getConfig() const { return this->_conf; };
const LocationCfg                           &Request::getCurrentLocation() const { return this->_currentLocation; };
const std::string                           &Request::getPath() const { return this->_path; };

void Request::setMethod(const std::string &method) { this->_method = method; };
void Request::setUri(const std::string &uri) { this->_uri = uri; };
void Request::setVersion(const std::string &version) { this->_version = version; };
void Request::setBody(const std::string &body) { this->_body = body; };
void Request::setHeaders(const std::map<std::string, std::string> &headers) { this->_headers = headers; };
void Request::setConfig(ServerCfg const& config) { this->_conf = config; };
void Request::setRawRequest(const char *message, const int &len) {
    this->_rawRequest.append(message, message + len);
}

REQUEST_STATE Request::collectRequest(void)
{
    size_t pos;

    if (_status == EMPTY || _status == IS_FIRST_LINE)
    {
        pos = _rawRequest.find("\r\n");
        if (pos == std::string::npos)
        {
            _parsingFirstLine += _rawRequest;
            _status = IS_FIRST_LINE;
        }
        else
        {
            _parsingFirstLine += _rawRequest.substr(0, pos);
            if ((_error = parseFirstLine(_parsingFirstLine)) != "")
                return ERROR;
            _parsingHeaders = _rawRequest.substr(pos + 2);
            _status = IS_HEADERS;
        }
    }
    if (_status == IS_HEADERS)
    {
        pos = _rawRequest.find("\r\n\r\n");
        if (pos == std::string::npos)
        {
            _parsingHeaders += _rawRequest;
            _status = IS_HEADERS;
        }
        else
        {
            _parsingBody = _rawRequest.substr(pos + 4);
            _status = IS_BODY;
        }
    }
    if (_status == IS_BODY || _status == NOT_FULL_CHUNK)
    {
        if (_headers.find("Transfer-Encoding") == _headers.end())
        {
            size_t length = std::atoi(_headers.find("Content-Length")->second.c_str()); // headers should contain this flied, but if not?
            if (length > _rawRequest.size())
            {
                _parsingBody += _rawRequest.substr(0, length);
                _headers["Content-Length"] = length - _rawRequest.size();
                _status = IS_BODY;
            }
            else
            {
                _parsingBody += _rawRequest.substr(0, length);
                _status = DONE;
            }
        }
        else
        {
            size_t bitsNum = 0;
            std::string newline;

            _parsingBody += _rawRequest;
            newline = nextLine(_parsingBody);
            while (newline != "")
            {
                if (_status == NOT_FULL_CHUNK)
                    _body += newline;
                else
                {
                    _parsingBody = _parsingBody.substr(newline.size());
                    if (std::atoi(newline.c_str()) != 0)
                        bitsNum = std::atoi(newline.c_str());
                    if (std::atoi(newline.c_str()) == 0 && newline != "0")
                    {
                        if (bitsNum <= newline.size())
                        {
                            _body += newline.substr(0, bitsNum);
                            bitsNum = 0;
                        }
                        else
                            _status = NOT_FULL_CHUNK;
                    }
                    else if (std::atoi(newline.c_str()) == 0 && newline == "0")
                        _status = DONE;
                }
                _parsingBody = _parsingBody.substr(newline.size());
                newline = nextLine(_parsingBody);
            }
        }
    }
    return _status;
};

std::string Request::nextLine(std::string const& text)
{
    std::string str;
    size_t pos;

    pos = text.find("\r\n");
    if (pos == std::string::npos)
        return text;
    str = text.substr(0, pos);
    return str;
};

Response Request::parse(void) {
    std::string error = "";

    if (_rawRequest.find("\r\n") == std::string::npos)
        return Response("400", "", _conf.getErrorPages());
    if ((error = parseFirstLine(_rawRequest.substr(0, _rawRequest.find("\r\n")))) != "")
        return Response(error, _uri, _conf.getErrorPages());
    if (_rawRequest.find("\r\n\r\n") == std::string::npos || _rawRequest.substr(_rawRequest.find("\r\n\r\n")) == "")
    {
        if (_rawRequest.find("\r\n") == std::string::npos)
            return Response("400", _uri, _conf.getErrorPages());
        else
            getHeaders(_rawRequest.substr(_rawRequest.find("\r\n") + 2));
    }
    else
    {
        if (getHeaders(_rawRequest.substr(_rawRequest.find("\r\n") + 2, _rawRequest.find("\r\n\r\n") - _rawRequest.find("\r\n"))))
            return Response("400", _uri, _conf.getErrorPages());
        _body = _rawRequest.substr(_rawRequest.find("\r\n\r\n") + 4);
    }
    return execute();
};

std::string Request::parseFirstLine(std::string firstLine) {
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

bool Request::getHeaders(std::string message) {
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
        value = newline.substr(newline.find_first_of(":") + 1);
        _headers[header] = value;
        message = message.substr(pos + 2);
        pos = message.find("\r\n");
    }
    return false;
}

Response Request::execute() {
    Response resp;
    size_t  point;
    std::string cgiResponse;
    
    this->_currentLocation = this->chooseLocation();
    this->constructAbsPath(false);
    if ((point = _uri.find_last_of(".")) != std::string::npos)
    {
        if (_uri.substr(point) == _currentLocation.getCgiExtention())
        {
            CGI_handler cgi = CGI_handler();
            try {
                cgiResponse = cgi.create_response(*this);
                _body = cgiResponse;
                return Response("200", _uri, _conf.getErrorPages());
            }
            catch (const std::exception& e) {
                return Response(e.what(), _uri, _conf.getErrorPages());
            }
        }
    }
    if (_currentLocation.getRedirectionCode() != "")
        return Response(_currentLocation.getRedirectionCode(), _currentLocation.getRedirectionUrl(), _conf.getErrorPages());

    if (_method == "GET" && _currentLocation.methodExists(GET))
        resp = execGet();
    else if (_method == "POST" && _currentLocation.methodExists(POST))
        resp = execPost();
    else if (_method == "DELETE" && _currentLocation.methodExists(DELETE))
        resp = execDelete();
    else
        return Response("405", _uri, _conf.getErrorPages());
    return resp;
};

LocationCfg Request::chooseLocation(void) {
    LocationCfg location = *(_conf.getLocations().begin());
    size_t maxLen = 0;
    std::string path = "";

    for (std::vector<LocationCfg>::const_iterator it = _conf.getLocations().begin(); it != _conf.getLocations().end(); it++)
    {
        path = "";
        std::string sub = _uri.substr(0, it->getPath().size());
        if (_uri == it->getPath()) {
            location = *it;
            _path = it->getRoot() + _uri.substr(1);
            break;
        }
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

    return location;
};

void Request::constructAbsPath(bool isIndex) {
    char cwd[2048];

    getcwd(cwd, 2048);
    _absPath += cwd;
    if (isIndex)
        _absPath += "/" + _currentLocation.getRoot() + _currentLocation.getIndex();
    else
        _absPath += ("/" + _path);

}

Response Request::execGet(void)
{
    std::fstream fs;
    Response resp;
    std::string res;
    std::string newline;
    std::string content;
    struct stat buffer;

    res = this->_autoindex();
    if (res == "500")
        return Response("500", _path, _conf.getErrorPages());
    else if (res == "404")
        return Response("404", _path, _conf.getErrorPages());
    else if (res != "")
    {
        resp.setBody(res);
        return resp;
    }
    if (stat(_path.c_str(), &buffer) == -1)
        return Response("404", _path, _conf.getErrorPages());
    fs.open(_path);
    if (!fs.good())
        return Response("403", _path, _conf.getErrorPages());
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

std::string Request::_autoindex(void)
{
    struct stat s;
    std::string content = "";
    std::string newline;
    std::fstream fs;

    if (stat(_path.c_str(), &s) != -1 && S_ISDIR(s.st_mode))
    {
        if (_currentLocation.getAutoIndex())
        {
            std::string page = _createHtmlPage();
            if (page == "")
                return "500";
            return page;
        }
        else
        {
            content = _indexFile();
            if (content == "")
                return "404";
            return content;
        }
    }
    return "";
};

std::string Request::_createHtmlPage(void)
{
    DIR *dpdf;
    struct dirent *epdf;
    std::string page = "<!DOCTYPE html>\
    <html>\
    <head>\
        <title>Index of " + this->_path + "</title>\
    </head>\
    <body>";

    std::string relativeUri = this->_uri;
    if (relativeUri != "/" && relativeUri[relativeUri.size() - 1] != '/')
        relativeUri += "/";
    dpdf = opendir(this->_path.c_str());
    if (dpdf == NULL)
        return "";
    page += "\t<ul>";
    while ((epdf = readdir(dpdf)))
        page += "\t\t<li><a href=\"" + relativeUri + std::string(epdf->d_name) + "\">" + std::string(epdf->d_name) + "</a></li>\n";
    page += "\t</ul>";

    page += "</body>\n</html>";
    closedir(dpdf);
    return page;
};

std::string Request::_indexFile(void)
{
    std::ifstream fileStream;
    std::string content = "";
    std::string newline;

    std::string index = _currentLocation.getIndex();
    _path = _currentLocation.getRoot() + index;
    constructAbsPath(true);
    std::fstream f(_currentLocation.getRoot() + index);
    fileStream.open(_path);
    if (!fileStream.is_open())
        return "";
    while(getline(fileStream, newline))
        content += newline;
    content += '\n';
    _body = content;
    fileStream.close();
    return content;
};

Response Request::execPost(void) {
    if (this->_currentLocation.getClientBodyBufferSize() < static_cast<long long>(_body.size()))
        return Response("413", _uri, _conf.getErrorPages());

    if (this->_headers.find("Content-Type") != this->_headers.end() && this->_headers.find("Content-Type")->second.find("multipart/form-data;"))
        return this->_handleFormData();
    std::fstream fs;
    fs.open(this->_uri);
    if (!fs.is_open())
    {
        fs.clear();
        fs.open(this->_uri, std::ios::out);
        fs.close();
        fs.open(this->_uri);
    }
    fs << this->_body;
    fs.close();
    return Response("200", _uri, _conf.getErrorPages());
};

Response Request::_handleFormData(void) {
    typedef std::pair<std::string, std::string> FilePair;
    std::vector<FilePair> filesToAdd;
    std::string boundary = this->_getFormDataBoundaryName();
    size_t start;
    size_t offset;

    if (boundary == "")
        return Response("500", this->_uri, this->_conf.getErrorPages());
    start = this->_body.find(boundary, 0);
    while (start != std::string::npos) {
        if (this->_isEndingBoundary(start, boundary.size()))
            break ;
        offset = start + boundary.size();
        try {
            filesToAdd.push_back(FilePair(this->_getFormDataFileName(offset), this->_getFormDataFileContent(boundary, offset)));
        }
        catch (std::exception &e) {
            return Response("500", this->_uri, this->_conf.getErrorPages());
        }
        start = this->_body.find(boundary, offset);
    }
    for (std::vector<FilePair>::iterator it = filesToAdd.begin(); it != filesToAdd.end(); it++) {
        std::ofstream out(this->_currentLocation.getUploadDir() + it->first);
        out << it->second;
        out.close();
    }
    return Response("200", this->_uri, this->_conf.getErrorPages());
}

std::string   Request::_getFormDataBoundaryName(void) {
    size_t  start = this->_headers["Content-Type"].find("boundary=");

    if (start == std::string::npos)
        return "";
    start += 9;
    return this->_headers["Content-Type"].substr(start);
}

bool    Request::_isEndingBoundary(const size_t &startingPos, const size_t &boundarySize) {
    return this->_body[startingPos + boundarySize] == '-' && this->_body[startingPos + boundarySize + 1] == '-';
}

std::string  Request::_getFormDataFileName(const size_t &offset) {
    size_t  fileNamePos = this->_body.find("filename=", offset);
    size_t  i = 0;

    if (fileNamePos == std::string::npos)
        throw;
    fileNamePos += 10; // filename= and '"'
    while (this->_body[fileNamePos + i] != '"') ++i;
    return this->_body.substr(fileNamePos, i);
}

std::string  Request::_getFormDataFileContent(const std::string &boundaryName, const size_t &offset) {
    size_t  startingFileContent = this->_body.find("\r\n\r\n", offset) + 4;
    size_t  endingBoundary = this->_body.find(boundaryName, offset) - BOUNDARY_PREFIX_LEN - 2; // 2 is for '\r\n'
    return this->_body.substr(startingFileContent, endingBoundary - startingFileContent);
}

Response Request::execDelete(void) {
    std::ifstream ifs (this->_absPath);
    struct stat buffer;

    if (!stat(this->_absPath.c_str(), &buffer))
    {
        if (buffer.st_mode & S_IFDIR)
            return Response("404", this->_uri, this->_conf.getErrorPages());
        if (!(buffer.st_mode & S_IRUSR))
            return Response("403", this->_uri, this->_conf.getErrorPages());
        if (remove(this->_absPath.c_str()))
            return Response("403", this->_uri, this->_conf.getErrorPages());
    }
    else
        return Response("404", this->_uri, this->_conf.getErrorPages());
    return Response("200", this->_uri, this->_conf.getErrorPages()); // return 204, if response without body
};

std::string Request::getRequest(void) {
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

std::ostream& operator<<(std::ostream &out, Request request) {
    out << request.getRequest();
    return out;
};