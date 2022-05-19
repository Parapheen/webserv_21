#include "Request.hpp"
#include "Autoindex.hpp"
#include "../CGI/CGI_handler.hpp"
#define BOUNDARY_PREFIX_LEN 2

Request::Request(void): _headers(std::map<std::string, std::string>()) { return; };

Request::Request(Request const &copy): _method(copy._method), _uri(copy._uri), _path(copy._path), _absPath(copy._absPath), _version(copy._version), _headers(copy._headers), _body(copy._body) { return; };

Request& Request::operator=(Request const& source) {
    if (this != &source)
    {
        this->_method = source._method;
        this->_uri = source._uri;
        this->_path = source._path;
        this->_absPath = source._absPath;
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
const std::string                           &Request::getAbsPath(void) const { return this->_absPath; };
const std::string                           &Request::getRoot(const LocationCfg &location) const {
    if (location.getRoot() != "")
        return location.getRoot();
    return this->_conf.getRoot();
};
const std::map<std::string, std::string>    &Request::getHeaders(void) const { return this->_headers; };
const ServerCfg                             &Request::getConfig() const { return this->_conf; };

void Request::setMethod(const std::string &method) { this->_method = method; };
void Request::setUri(const std::string &uri) { this->_uri = uri; };
void Request::setVersion(const std::string &version) { this->_version = version; };
void Request::setBody(const std::string &body) { this->_body = body; };
void Request::setHeaders(const std::map<std::string, std::string> &headers) { this->_headers = headers; };
void Request::setConfig(ServerCfg const& config) { this->_conf = config; };

Response Request::parse(const std::string &message) {
    std::string error = "";

    if (message.find("\r\n") == std::string::npos)
        return Response("400", _uri, _conf.getErrorPages());
    if ((error = parseFirstLine(message.substr(0, message.find("\r\n")))) != "")
        return Response(error, _uri, _conf.getErrorPages());
    if (message.find("\r\n\r\n") == std::string::npos)
    {
        if (message.find("\r\n") == std::string::npos)
            return Response("400", _uri, _conf.getErrorPages());
        getHeaders(message.substr(message.find("\r\n") + 2));
    }
    else
    {
        if (getHeaders(message.substr(message.find("\r\n") + 2, message.find("\r\n\r\n") - message.find("\r\n"))))
            return Response("400", _uri, _conf.getErrorPages());
        _body = message.substr(message.find("\r\n\r\n") + 4);
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
	std::string cgi_response;
    
    this->_currentLocation = this->chooseLocation();
    this->constructAbsPath();
	//also added getters for current location and path for request method
    if ((point = _path.find(_currentLocation.getCgiExtention())) != std::string::npos)
    {
		CGI_handler cgi = CGI_handler();
		try {
			cgi_response = cgi.create_response(*this);
		}
		catch (const std::exception& e) {
			std::cout << "Error code = " << e.what() << std::endl;
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

void Request::constructAbsPath(void) {
    char cwd[2048];

    getcwd(cwd, 2048);
    _absPath += cwd;
    _absPath += ("/" + _path);
    std::cout << "uri: " << _uri << " path: " << _path << " abs path: " << _absPath << std::endl;
}

Response Request::execGet(void)
{
    std::fstream fs;
    Response resp;
    std::string res;
    std::string newline;
    std::string content;
    struct stat buffer;

    // return Response("500", _conf.getErrorPages(), _path);
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

    if (_uri[_uri.size() - 1] == '/' || (stat(_path.c_str(), &s) == 0 && (s.st_mode & S_IFDIR)))
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
    if (relativeUri != "/")
        relativeUri += "/";
    dpdf = opendir(this->_path.c_str());
    if (dpdf != NULL)
    {
        page += "\t<ul>";
        while ((epdf = readdir(dpdf)))
            page += "\t\t<li><a href=\"" + relativeUri + std::string(epdf->d_name) + "\">" + std::string(epdf->d_name) + "</a></li>\n";
        page += "\t</ul>";
    }

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
    std::fstream f(_currentLocation.getRoot() + index);
    _path += _currentLocation.getIndex();
    _absPath += _currentLocation.getIndex();
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
    return Response("201", this->_uri, this->_conf.getErrorPages());
}

std::string   Request::_getFormDataBoundaryName(void) {
    size_t  start = this->_headers["Content-Type"].find("boundary=");

    if (start == std::string::npos)
        return "";
    start += 9;
    return this->_headers["Content-Type"].substr(start);
}

bool    Request::_isEndingBoundary(const size_t &startingPos, const size_t &boundarySize) {
    std::cout << this->_body[startingPos + boundarySize] << std::endl;
    std::cout << this->_body[startingPos + boundarySize + 1] << std::endl;
    return this->_body[startingPos + boundarySize] == '-' && this->_body[startingPos + boundarySize + 1] == '-';
}

std::string  Request::_getFormDataFileName(const size_t &offset) {
    size_t  fileNamePos = this->_body.find("filename=", offset);
    size_t  i = 0;

    if (fileNamePos == std::string::npos)
        throw;
    fileNamePos += 10; // filename= and '"'
    while (this->_body[fileNamePos + i] != '"') ++i;
    std::cout << this->_body.substr(fileNamePos, i) << std::endl;
    return this->_body.substr(fileNamePos, i);
}

std::string  Request::_getFormDataFileContent(const std::string &boundaryName, const size_t &offset) {
    size_t  startingFileContent = this->_body.find("\r\n\r\n", offset) + 4;
    size_t  endingBoundary = this->_body.find(boundaryName, offset) - BOUNDARY_PREFIX_LEN - 2; // 2 is for '\r\n'
    std::cout << this->_body.substr(startingFileContent, endingBoundary - startingFileContent) << std::endl;
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

const std::string &Request::getPath() const {
    return _path;
}

const LocationCfg &Request::getCurrentLocation() const {
    return _currentLocation;
}

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
