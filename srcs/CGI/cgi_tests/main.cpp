#include <iostream>

#include "CGI_handler.hpp"
#include "LocationCfg.hpp"
#include "ServerCfg.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "Autoindex.hpp"

int main() {
    Request request = Request();
    LocationCfg location = LocationCfg();
    ServerCfg config = ServerCfg();
    std::map<std::string, std::string> headers;
    Response response;

    location.setClientBodyBufferSize(100);
    location.setCgiExpantion(".cgi");

    config.setName("localhost");
    config.addLocation(location);

    /////////////////////////////////////////request///////////////////////////////////
    request.setConfig(config);

    request.setUri("uploader.cgi/destination.dest?add=shit");
    request.setAbsPath("/Users/nscarab/Desktop/clion/projects/cgi_test/uploader.cgi/destination.dest?add=shit");
    request.setPath("uploader.cgi/destination.dest?add=shit");

    request.setMethod("POST");
    request.setVersion("HTTP/1.1");

    headers.insert(std::pair<std::string, std::string>("Host", "localhost:8080"));
    request.setHeaders(headers);

    request.setBody("eee");
    /////////////////////////////////////////request///////////////////////////////////

 CGI_handler cgi = CGI_handler();
 response = cgi.create_response(request, location);
 std::cout << response.getCgiResponse() << std::endl;
    return 0;
}