#pragma once
#include "../parser/ServerCfg.hpp"
#include "../http/Request.hpp"
#include "../http/Response.hpp"
#include <fcntl.h>
//#define BIN_PATH "cgi_bin"
#define BIN_PATH "/usr/bin/python"

class CGI_handler {
public:

    CGI_handler(void);

    std::string create_response(Request &request);

    class ResourceError : public std::exception{
        const char *what() const throw() {
            return ("500");
        }
    };

    class ForkError : public std::exception{
        const char *what() const throw() {
            return ("500");
        }
    };

    class ExecveError : public std::exception{
        const char *what() const throw() {
        return ("500");
        }
    };

    class PayLoadError : public std::exception{
        const char *what() const throw() {
            return ("413");
        }
    };

    class IncorrectResponseError : public std::exception{
        const char *what() const throw() {
            return ("500");
        }
    };

    private:
        char **create_argv() const;
        char **create_envp(Request &request) const;

    std::string script_path_;
    std::string full_script_path_;
    LocationCfg location;
    std::string rel_path;
    std::string abs_path;
    std::string extention;
};
