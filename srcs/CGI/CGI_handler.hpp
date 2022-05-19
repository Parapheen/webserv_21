//
// Created by Namor Scarab on 5/13/22.
//

#ifndef CGI_HANDLER_HPP
#define CGI_HANDLER_HPP

#include <unistd.h>

#include "../http/Request.hpp"
#include "../parser/LocationCfg.hpp"
#define BIN_PATH "uploader.cgi"

class CGI_handler {

public:

    CGI_handler(void);                              //canonical default constructor
    // member functions
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
    LocationCfg location;
    std::string rel_path;
    std::string extention;
};

//run in correct directory for relative path file access - YES
//requested file as first argument - YES
//server works with one CGI - php-CGI/Python or just my binary? - ??? - for now simple
//unchunking the request - do i get all sub characters? then i shall delete them - ???

//Input: mark EOF at the end in case of no content length - YES because i get a ready request and do cache
//output: expect eof as end of body if i don't get content lenght - NO - add
//i wont call cgi directly - full path as path info - YES - translated = info unless null
//route -> uploaded file info (body) + where to save (path info) - YES

//scenario should be ready for abnormal termination - YES cause should
//content_length is set by server if and only if there is a body in request - NO - add
// in this case CONTENT_TYPE variable is set to the Internet Media Type - NO - add if
//content type must be set in case if there is content type in client request - NO - add if
//interface - MUST - YES
//path info is a path segment  - we may make some rules to it - YES - cause may
// if no path info is NULL path translated must be set to null - NO - add if
// even if there is no QUERY string it MUST be defined - YES
//remote host and r addr MUST be set - NO - where to get
//request method is a MUST - YES
//script name is a MUST - YES
//server name protocol software and port is a must - YES

//HTTP_ -> - to _ -> no doubles -> authorization, content type or length - YES/No - auth

//no reading more then content length BUT NOT OBLIGATED To read it at all - NO - cut body before input
//transfer codings are not supported on the request body -> delete and recount content length - NO - add
//i must see content length before reading - NO - add
//protocol is added, status only if cgi doesn't, type is a must from cgi - YES/No - protocol

//route is assumably a specific path to get a specific info - cgi route in our case - YES

#endif //CGI_HANDLER_HPP
