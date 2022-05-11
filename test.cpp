#include <iostream>
#include <string>
#include "http.hpp"
#include <sstream>
#include <sys/stat.h>

#include <algorithm>
#include <filesystem>

#include "response.hpp"
#include "request.hpp"
#include "LocationCfg.hpp"
#include "ServerCfg.hpp"

#include <dirent.h>

int main()
{
    //std::string message = "HTTP/1.1 200 OK\r\nDate: Sat, 09 Oct 2010 14:28:02 GMT\r\nServer: Apache\r\nLast-Modified: Tue, 01 Dec 2009 20:18:22 GMT\r\nETag: \"51142bc1-7449-479b075b2891b\"\r\nAccept-Ranges: bytes\r\nContent-Length: 29769\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE html... (here comes the 29769 bytes of the requested web page)\r\n";
    std::string message = "GET / HTTP/1.1\r\nHost: 127.0.0.1\r\nConnection: keep-alive\r\nsec-ch-ua: \" Not A;Brand\";v=\"99\", \"Chromium\";v=\"96\", \"Google Chrome\";v=\"96\"\r\nsec-ch-ua-mobile: ?0\r\nsec-ch-ua-platform: \"macOS\"\r\nUpgrade-Insecure-Requests: 1\r\nUser-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_14_6) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/96.0.4664.55 Safari/537.36\r\nAccept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.9\r\nSec-Fetch-Site: none\r\nSec-Fetch-Mode: navigate\r\nSec-Fetch-User: ?1\r\nSec-Fetch-Dest: document\r\nAccept-Encoding: gzip, deflate, br\r\nAccept-Language: en-US,en;q=0.9,ru;q=0.8";
    //std::cout << message;
    
    
    ServerCfg conf;
    LocationCfg loc;
    std::vector<std::string> indexes;
    indexes.push_back("index.html");
    indexes.push_back("index.php");

    loc.setRoot("/Users/odomenic/Desktop/webserv");
    loc.setPath("/Users/odomenic/Desktop/");
    loc.setAutoIndex(true);
    loc.setIndexes(indexes);
    loc.setCgiExpantion(".cpp");
    loc.setRedirectionCode("307");
    loc.setRedirectionUrl("/Users/odomenic/");

    conf.addLocation(loc);
    
    Response resp;
    Request req;
    req.setConfig(conf);
    resp = req.parse(message);
    std::cout << resp << std::endl;

    //std::transform(message.begin(), message.end(), message.begin(), [](unsigned char c){ return std::tolower(c); });
    //std::transform(message.begin(), message.end(), message.begin(), std::tolower);

    message = myToLower(message);
    //std::cout << "lower message: " << message << std::endl;

    std::fstream fs;
    fs.open("newfile.txt");
    if (!fs.is_open())
    {
        std::cout << "Can\'t open file\n";
        fs.clear();
        fs.open("newfile.txt", std::ios::out);
        fs.close();
        fs.open("newfile.txt");
    }
    fs << "New message in file\n";
    fs.close();

    std::ifstream iff("filename.dat");
    if (iff.bad() == true)
        std::cout << "file is not present\n";
    else
        std::cout << "file is present\n";    

    std::string mes = "<!DOCTYPE html>\
<html>\
 <head>\
  <meta charset=\"utf-8\">\
  <title>Отправка файла на сервер</title>\
 </head>\
 <body>\
  <form enctype=\"multipart/form-data\" method=\"post\">\
   <p><input type=\"file\" name=\"f\">\
   <input type=\"submit\" value=\"Отправить\"></p>\
  </form>\
 </body>\
</html>";
    size_t pos = mes.find("multipart/form-data");
    if (pos != std::string::npos)
        std::cout << "finded: " << pos << std::endl;
    else
        std::cout << "npos\n";
    return 0;
}