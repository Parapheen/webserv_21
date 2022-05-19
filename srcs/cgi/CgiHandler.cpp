#include "CgiHandler.hpp"

CGI_handler::CGI_handler() {}

std::string CGI_handler::create_response(Request &request) {

    location = request.getCurrentLocation();
    rel_path = request.getPath();
    extention = location.getCgiExtention();

    //find path to script from path
    if (rel_path.find_first_of("/") == 0)
        rel_path.erase(0, 1);
    if (rel_path.find(extention) != std::string::npos)
        script_path_ = rel_path.substr(0, rel_path.find(extention) + extention.length());
//    std::cout << "script path: " << script_path_ << std::endl; //DEBUG

    //Create cache files - better on base of fd
    std::string request_file_name = ".request_cache";
    std::string response_file_name = ".response_cache";
    std::fstream request_file(request_file_name.c_str(), std::ios_base::trunc | std::ios_base::in
                                               | std::ios_base::out);
    request_file.close();
    std::fstream response_file(response_file_name.c_str(), std::ios_base::trunc | std::ios_base::in
                                                         | std::ios_base::out);
    response_file.close();

    // Put body to cache
    std::ofstream out(request_file_name.c_str(), std::ofstream::out);
    if (out.is_open()) {
        out << request.getBody();
    }
    out.close();

    //create fd for cache files
    int request_fd = open(request_file_name.c_str(), O_RDWR);
    if (request_fd < 0) {
        throw ResourceError();
    }
    int response_fd = open(response_file_name.c_str(), O_RDWR);
    if (response_fd < 0) {
        throw ResourceError();
    }

    // form execve input
    char **argv = create_argv();
    char **envp = create_envp(request);

    //change dir before starting because we use full path
    char buf[256];
    int i = 0;
    getcwd(buf, 256);
    buf[255] = '\0';
    std::string current_dir;
    while (buf[i]){
        current_dir.push_back(buf[i]);
        i++;
    }
    chdir("/");
//    getcwd(buf, 256); //DEBUG
//    std::cout << "current dir: " << buf << std::endl; //DEBUG
    std::string execve_dir = current_dir + "/" + BIN_PATH;
    if (execve_dir.find_first_of("/") == 0)
        execve_dir.erase(0, 1);
    if (execve_dir.find_first_of("/") == 0)
        execve_dir.erase(0, 1);
//    std::cout << "execve dir: " << execve_dir << std::endl; //DEBUG

    //fork!
    pid_t pid;
    int status = 0;

    pid = fork();
    if (pid == -1) {
        close(request_fd);
        close(response_fd);
        throw ForkError();
    }
    if (pid) {
        close(request_fd);
        close(response_fd);
    }
    if (!pid) {
        dup2(request_fd, 0);
        dup2(response_fd, 1);
        status = execve(execve_dir.c_str(), argv, envp);
        throw ExecveError();
    }
    waitpid(pid, &status, 0);
//    std::cout << status << std::endl; //DEBUG


//change dir back
    chdir(current_dir.c_str());
//    getcwd(buf, 256); //DEBUG
//    std::cout << "current dir: " << buf << std::endl; //DEBUG

    //read from response cache
    std::ifstream in(response_file_name);
    std::string   result_str;
    std::string   line;

    if (in.is_open()) {
        while (std::getline(in, line)) {
            result_str.append(line);
            if (!in.eof())
                result_str.append("\n");
        }
    }
    in.close();
//    std::cout << result_str << std::endl; //DEBUG

    //free argv and envp, delete cache
    char **tmp = argv;
    while (*argv) {
        free(*argv);
        ++argv;
    }
    free(tmp);
    tmp = envp;
    while (*envp) {
        free(*envp);
        ++envp;
    }
    free(tmp);
    remove(response_file_name.c_str());
    remove(request_file_name.c_str());

    //analyze response from cgi and form a string
    std::string content_length;
    std::string body_string;
    if (result_str.find("\r\n\r\n") != std::string::npos)
        body_string = result_str.substr(result_str.find("\r\n\r\n") + 4);
    else {
        throw IncorrectResponseError();
    }

    if (static_cast<long long>(body_string.size()) > location.getClientBodyBufferSize()) {
        throw PayLoadError();
    }

    //parse headers
    size_t offset = 0;
    size_t string_index = 0;
    size_t semicolumn_index = 0;
    std::string single_header;
    std::map<std::string, std::string> headers;
    std::string headers_string = result_str.substr(0, result_str.find("\r\n\r\n") + 2);
    while ((string_index = headers_string.find("\r\n", offset)) != std::string::npos) {
        single_header = headers_string.substr(offset, string_index - offset);
        semicolumn_index = single_header.find(": ");
        headers.insert(std::make_pair(single_header.substr(0, semicolumn_index),
                                     single_header.substr(semicolumn_index + 2, std::string::npos)));
        offset = offset + string_index + 2;
    }
//    for(std::map<std::string, std::string>::iterator it = headers.begin(); //DEBUG
//    it != headers.end(); it++)
//    {
//        std::cout << "header: " << it->first << " name: " << it->second << std::endl;
//    }
//    content_length = "Content-Length: " + std::to_string(body_string.size());

//if no content length - we signal about end of body
    if (!headers.count("Content-Length"))
        body_string = body_string + "0\r\n\r\n";

    //add protocol and status
    result_str = "HTTP/1.1 200 OK\r\n"
                 + result_str.substr(0, result_str.find("\r\n\r\n") + 4)
                 + body_string;

    return result_str;
}

char **CGI_handler::create_argv() const {
    char **args = static_cast<char**>(malloc(sizeof(char*) * (1 + 1)));

    args[0] = strdup(BIN_PATH);
    args[1] = nullptr;
    return args;
}

char **CGI_handler::create_envp(Request &request) const {
    std::map<std::string, std::string> envp;
    std::map<std::string, std::string> headers(request.getHeaders());

    envp["GATEWAY_INTERFACE"] = "CGI/1.1"; //MUST
    envp["SERVER_PROTOCOL"]   = "HTTP/1.1"; //MUST
    envp["SERVER_SOFTWARE"]   = "webserv_les/1.0"; //MUST
    envp["SERVER_NAME"] = request.getConfig().getName();
    envp["SERVER_PORT"] = request.getConfig().getPort();


    //how do i understand that there is no body? if no body Must no length
    envp["CONTENT_LENGTH"] = std::to_string(request.getBody().length()); //if present
    envp["CONTENT_TYPE"] = headers.count("Content-Type")
                          ? headers["Content-Type"] : "";

    //in mean full uri from request including ?... is it so?
    envp["REQUEST_URI"]  = request.getUri();
//    std::cout << envp["REQUEST_URI"] << std::endl; //DEBUG
    envp["QUERY_STRING"] = "";

    size_t questionmark_index;
    questionmark_index = envp["REQUEST_URI"].find_first_of('?');
    if (questionmark_index != std::string::npos) {
        envp["QUERY_STRING"] = envp["REQUEST_URI"].substr(questionmark_index + 1);
        envp["REQUEST_URI"].resize(questionmark_index);
    }

    envp["REQUEST_METHOD"] = request.getMethod();
    envp["SCRIPT_NAME"]    = script_path_;

    //utochnit with paths - where can a find ? path - is she counting on it?
    std::string path_info;
    path_info = rel_path.substr(script_path_.length(),
                                rel_path.find_first_of('?') - script_path_.length());
    if (path_info.find_first_of("/") == 0)
        path_info.erase(0, 1);
    envp["PATH_INFO"]         = path_info;
    envp["PATH_TRANSLATED"]   = envp["PATH_INFO"]; //considering subject but see rules

    envp["AUTH_TYPE"] = headers.count("Authorization")
                        ? headers["Authorization"] : ""; //will never be used - not a MUST

    //replace all - with _ in header name and add HTTP_
    size_t sub_index;
    for (std::map<std::string, std::string>::iterator it = headers.begin();
         it != headers.end(); ++it) {
        std::string header(it->first);
        sub_index = header.find('-');
        if (sub_index != std::string::npos) {
            header.replace(sub_index, 1, "_");
        }
        envp["HTTP_" + header] = it->second;
    }

    char **env_result = static_cast<char**>(malloc(sizeof(char*)
                                                   * (envp.size() + headers.size() + 1)));
    if (!env_result) {
        throw CGI_handler::ResourceError();
    }
    //add clients headers to map
    size_t i = 0;
    for (std::map<std::string, std::string>::iterator it = headers.begin();
         it != headers.end(); ++it) {
        env_result[i] = strdup(std::string(it->first + "=" + it->second).c_str());
        ++i;
    }
    //move our map to char **
    for (std::map<std::string, std::string>::iterator it = envp.begin();
         it != envp.end(); ++it) {
//        std::cout << it->first << ": " << it->second << std::endl; //DEBUG
        env_result[i] = strdup(std::string(it->first + "=" + it->second).c_str());
        ++i;
    }
    env_result[i] = nullptr;
//    std::cout  << "===========================" << std::endl; //DEBUG
    return env_result;
}