#include "Request.hpp"
#include "Autoindex.hpp"
#define BOUNDARY_PREFIX_LEN 2

Request::Request(void): _headers(std::map<std::string, std::string>()) { return; };

Request::Request(Request const &copy): _method(copy._method), _uri(copy._uri), _version(copy._version), _headers(copy._headers), _body(copy._body) { return; };

Request& Request::operator=(Request const& source) {
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
    // size_t  point;
    
    this->_currentLocation = this->chooseLocation();
    this->constructAbsPath();
    // if ((point = _uri.find_last_of(".")) != std::string::npos)
    // {
    //     std::map<std::string, std::string> params; // maybe it isn't nesseccary and put all body to cgi
    //     if (_uri.substr(point + 1) == _currentLocation.getCgiExtention())
    //     {
    //         // Cgi cgi(location, _headers);
    //         // return Response(cgi.result());
    //     }
    //     else
    //         return Response("500" , _uri); // ?????
    // }
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

LocationCfg Request::chooseLocation(void) {
    LocationCfg location;
    for (std::vector<LocationCfg>::const_iterator it = _conf.getLocations().begin(); it != _conf.getLocations().end(); it++)
    {
        if (it->getPath() == "/")
            location = *it;
        else if (it->getPath() == this->getUri()) {
            location = *it;
            break;
        }
    }
    return location;
};

void Request::constructAbsPath(void) {
    std::string root = this->_currentLocation.getRoot();
    bool isRootAbsolutePath = root[0] == '/';

    if (this->_uri != "/") {
        if (isRootAbsolutePath)
            this->_absPath = root + this->_uri.substr(1);
        else
            this->_absPath = "./" + root + this->_uri.substr(1);
    }
    else {
        if (isRootAbsolutePath)
            this->_absPath = root;
        else
            this->_absPath = "./" + root;
    }
}

Response Request::execGet(void) {
    std::ifstream   fileStream;
    Response resp;
    std::string strh;
    std::string fileContent;
    std::string maxPath;

    if (_uri[_uri.size() - 1] == '/' || _isDirectory(_absPath.c_str()))
    {
        if (_currentLocation.getAutoIndex())
        {
            Autoindex autoindex;
            
            std::string page = autoindex.createPage(this->getRoot(this->_currentLocation) + _uri, _uri);
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
            std::string targetFile = "./" + this->getRoot(this->_currentLocation) + index;
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

Response Request::execPost(void) {
    if (this->_currentLocation.getClientBodyBufferSize() < static_cast<long long>(_body.size()))
        return Response("413", this->_uri);

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
    return Response("200", this->_uri);
};

Response Request::_handleFormData(void) {
    typedef std::pair<std::string, std::string> FilePair;
    std::vector<FilePair> filesToAdd;
    std::string boundary = this->_getFormDataBoundaryName();
    size_t start;
    size_t offset;

    if (boundary == "")
        return Response("500", _uri);
    start = this->_body.find(boundary, 0);
    while (start != std::string::npos) {
        if (this->_isEndingBoundary(start, boundary.size()))
            break ;
        offset = start + boundary.size();
        try {
            filesToAdd.push_back(FilePair(this->_getFormDataFileName(offset), this->_getFormDataFileContent(boundary, offset)));
        }
        catch (std::exception &e) {
            return Response("500", this->_uri);
        }
        start = this->_body.find(boundary, offset);
    }
    for (std::vector<FilePair>::iterator it = filesToAdd.begin(); it != filesToAdd.end(); it++) {
        std::ofstream out(this->_currentLocation.getUploadDir() + it->first);
        out << it->second;
        out.close();
    }
    return Response("201", this->_uri);
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

std::string myToLower(const std::string &str) {
    std::string res;
    for (size_t i = 0; i < str.size(); i++)
        res[i] = std::tolower(str[i]);
    return res;
};

std::ostream& operator<<(std::ostream &out, Request request) {
    out << request.getRequest();
    return out;
};